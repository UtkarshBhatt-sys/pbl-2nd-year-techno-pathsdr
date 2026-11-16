#include "system.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <climits>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <unordered_set>

using namespace std;

namespace {

const char* DATA_FILE = "drastic_vantage.dat";
const char* TEMP_FILE = "drastic_vantage.dat.tmp";

void writeCamps(ostream& out, const CampNode* root) {
    if (!root)
        return;

    writeCamps(out, root->left);

    out << root->campId << ' '
        << quoted(string(root->campName)) << ' '
        << root->locationIndex << ' '
        << root->capacity << '\n';

    writeCamps(out, root->right);
}


bool validDispatch(
    const Dispatch& d,
    const Inventory& inv,
    CampNode* camps
) {
    return d.dispatchId > 0 &&
           d.requestId > 0 &&
           d.campId > 0 &&
           d.supplyId > 0 &&
           d.quantity > 0 &&
           d.distance >= 0 &&
           searchCamp(camps, d.campId) &&
           searchSupply(&inv, d.supplyId);
}

}

bool DisasterSystem::saveData() const {

    ofstream out(TEMP_FILE, ios::trunc);

    if (!out)
        return false;

    out << "DV1\n";

    out << nextRequestId << ' '
        << nextDispatchId << ' '
        << arrivalCounter << ' '
        << warehouseIndex << '\n';


    out << inventory.count << '\n';

    for (SupplyNode* p = inventory.head;
         p != nullptr;
         p = p->next) {

        out << p->id << ' '
            << quoted(string(p->name)) << ' '
            << p->quantity << '\n';
    }


    out << graph.vertexCount << '\n';

    for (int i = 0; i < graph.vertexCount; ++i) {
        out << quoted(string(graph.locations[i]))
            << '\n';
    }

    for (int i = 0; i < graph.vertexCount; ++i) {
        for (int j = 0; j < graph.vertexCount; ++j) {
            out << graph.adjacencyMatrix[i][j] << ' ';
        }
        out << '\n';
    }


    out << countCamps(campRoot) << '\n';

    writeCamps(out, campRoot);


    auto heapCopy = requestQueue;

    out << heapCopy.size() << '\n';

    while (!heapCopy.empty()) {

        const ReliefRequest& r = heapCopy.top();

        out << r.getRequestId() << ' '
            << r.getCampId() << ' '
            << r.getSupplyId() << ' '
            << r.getQuantity() << ' '
            << r.getPriority() << ' '
            << r.getArrivalOrder() << '\n';

        heapCopy.pop();
    }

   

    out << pendingRequests.size() << '\n';

    for (const auto& r : pendingRequests) {

        out << r.getRequestId() << ' '
            << r.getCampId() << ' '
            << r.getSupplyId() << ' '
            << r.getQuantity() << ' '
            << r.getPriority() << ' '
            << r.getArrivalOrder() << '\n';
    }

 
    out << dispatchQueue.count << '\n';

    for (int i = 0; i < dispatchQueue.count; ++i) {

        const Dispatch& d =
            dispatchQueue.items[
                (dispatchQueue.front + i) % MAX_DISPATCHES
            ];

        out << d.dispatchId << ' '
            << d.requestId << ' '
            << d.campId << ' '
            << d.supplyId << ' '
            << d.quantity << ' '
            << d.distance << '\n';
    }

   

    out << dispatchHistory.top + 1 << '\n';

    for (int i = 0; i <= dispatchHistory.top; ++i) {

        const Dispatch& d = dispatchHistory.items[i];

        out << d.dispatchId << ' '
            << d.requestId << ' '
            << d.campId << ' '
            << d.supplyId << ' '
            << d.quantity << ' '
            << d.distance << '\n';
    }


    out.flush();

    if (!out) {
        out.close();
        remove(TEMP_FILE);
        return false;
    }

    out.close();

    if (!out) {
        remove(TEMP_FILE);
        return false;
    }

    
    if (rename(TEMP_FILE, DATA_FILE) != 0) {
        remove(TEMP_FILE);
        return false;
    }

    return true;
}


bool DisasterSystem::loadData() {

    ifstream in(DATA_FILE);

    if (!in)
        return false;

    // Temporary structures for safe loading
    Inventory newInventory;
    Graph newGraph;
    CampNode* newCamps = nullptr;

    DispatchQueue newDispatchQueue;
    DispatchStack newHistory;

    priority_queue<
        ReliefRequest,
        vector<ReliefRequest>,
        RequestComparator
    > newRequests;

    vector<ReliefRequest> newPending;

    initInventory(&newInventory);
    initGraph(&newGraph);
    initDispatchQueue(&newDispatchQueue);
    initDispatchStack(&newHistory);

    // Free temporary data if loading fails
    auto cleanup = [&]() {
        freeInventory(&newInventory);
        freeCampTree(newCamps);
        newCamps = nullptr;
    };

    try {

       

        string version;

        if (!(in >> version) || version != "DV1")
            throw runtime_error("Invalid file format");

        int newRequestId;
        int newDispatchId;
        int newWarehouse;
        long long newArrival;

        if (!(in >> newRequestId
                 >> newDispatchId
                 >> newArrival
                 >> newWarehouse) ||
            newRequestId <= 0 ||
            newDispatchId <= 0 ||
            newArrival < 0) {

            throw runtime_error("Invalid counters");
        }

        int n;

        if (!(in >> n) || n < 0 || n > 10000)
            throw runtime_error("Invalid inventory count");

        for (int i = 0; i < n; ++i) {

            int id, qty;
            string name;

            if (!(in >> id >> quoted(name) >> qty) ||
                !addSupply(
                    &newInventory,
                    id,
                    name.c_str(),
                    qty
                ) ||
                newInventory.count != i + 1) {

                throw runtime_error("Invalid inventory");
            }
        }


        if (!(in >> n) ||
            n < 0 ||
            n > MAX_LOCATIONS) {

            throw runtime_error("Invalid location count");
        }

        for (int i = 0; i < n; ++i) {

            string name;

            if (!(in >> quoted(name)) ||
                addLocation(
                    &newGraph,
                    name.c_str()
                ) != i) {

                throw runtime_error("Invalid location");
            }
        }


        for (int i = 0; i < n; ++i) {

            for (int j = 0; j < n; ++j) {

                int weight;

                if (!(in >> weight) ||
                    weight < 0 ||
                    weight > INF ||
                    (i == j && weight != 0) ||
                    (i != j && weight == 0)) {

                    throw runtime_error("Invalid road weight");
                }

                newGraph.adjacencyMatrix[i][j] = weight;
            }
        }

    
        for (int i = 0; i < n; ++i) {

            for (int j = i + 1; j < n; ++j) {

                if (newGraph.adjacencyMatrix[i][j] !=
                    newGraph.adjacencyMatrix[j][i]) {

                    throw runtime_error(
                        "Asymmetric road matrix"
                    );
                }
            }
        }

        if (newWarehouse < -1 || newWarehouse >= n)
            throw runtime_error("Invalid warehouse");



        int campCount;

        if (!(in >> campCount) ||
            campCount < 0 ||
            campCount > 10000) {

            throw runtime_error("Invalid camp count");
        }

        for (int i = 0; i < campCount; ++i) {

            int id, loc, capacity;
            string name;

            if (!(in >> id
                     >> quoted(name)
                     >> loc
                     >> capacity) ||
                loc < 0 ||
                loc >= n ||
                searchCamp(newCamps, id)) {

                throw runtime_error("Invalid camp");
            }

            newCamps = insertCamp(
                newCamps,
                id,
                name.c_str(),
                loc,
                capacity
            );

            if (countCamps(newCamps) != i + 1)
                throw runtime_error(
                    "Camp insertion failed"
                );
        }

    

        unordered_set<int> requestIds;
        unordered_set<int> dispatchIds;

        long long highestArrival = -1;

        int highestRequestId = 0;
        int highestDispatchId = 0;

        auto readRequest = [&]() -> ReliefRequest {

            int id, camp, supply, qty, priority;
            long long order;

            if (!(in >> id
                     >> camp
                     >> supply
                     >> qty
                     >> priority
                     >> order) ||
                !searchCamp(newCamps, camp) ||
                !searchSupply(&newInventory, supply) ||
                !requestIds.insert(id).second) {

                throw runtime_error("Invalid request");
            }

            ReliefRequest r(
                id,
                camp,
                supply,
                qty,
                priority,
                order
            );

            highestRequestId =
                max(highestRequestId, id);

            highestArrival =
                max(highestArrival, order);

            return r;
        };

        

        int requestCount;

        if (!(in >> requestCount) ||
            requestCount < 0 ||
            requestCount > 10000) {

            throw runtime_error("Invalid request count");
        }

        for (int i = 0; i < requestCount; ++i) {
            newRequests.push(readRequest());
        }

     

        int pendingCount;

        if (!(in >> pendingCount) ||
            pendingCount < 0 ||
            pendingCount > 10000) {

            throw runtime_error("Invalid pending count");
        }

        for (int i = 0; i < pendingCount; ++i) {
            newPending.push_back(readRequest());
        }

       

        auto readDispatch = [&]() -> Dispatch {

            Dispatch d{};

            if (!(in >> d.dispatchId
                     >> d.requestId
                     >> d.campId
                     >> d.supplyId
                     >> d.quantity
                     >> d.distance) ||
                !validDispatch(
                    d,
                    newInventory,
                    newCamps
                ) ||
                !dispatchIds.insert(d.dispatchId).second ||
                requestIds.count(d.requestId)) {

                throw runtime_error("Invalid dispatch");
            }

            highestDispatchId =
                max(highestDispatchId, d.dispatchId);

            highestRequestId =
                max(highestRequestId, d.requestId);

            return d;
        };

        

        int activeCount;

        if (!(in >> activeCount) ||
            activeCount < 0 ||
            activeCount > MAX_DISPATCHES) {

            throw runtime_error(
                "Invalid dispatch queue size"
            );
        }

        for (int i = 0; i < activeCount; ++i) {

            if (!enqueueDispatch(
                    &newDispatchQueue,
                    readDispatch()
                )) {

                throw runtime_error(
                    "Dispatch queue overflow"
                );
            }
        }

    

        int historyCount;

        if (!(in >> historyCount) ||
            historyCount < 0 ||
            historyCount > MAX_DISPATCHES) {

            throw runtime_error("Invalid history size");
        }

        for (int i = 0; i < historyCount; ++i) {

            if (!pushDispatch(
                    &newHistory,
                    readDispatch()
                )) {

                throw runtime_error("History overflow");
            }
        }


        string extra;

        if (in >> extra)
            throw runtime_error("Unexpected extra data");

        if (!in.eof())
            throw runtime_error("Read failure");

        if (newRequestId <= highestRequestId ||
            newDispatchId <= highestDispatchId ||
            newArrival <= highestArrival) {

            throw runtime_error(
                "Inconsistent saved counters"
            );
        }



        freeInventory(&inventory);
        freeCampTree(campRoot);

        inventory = newInventory;

        newInventory.head = nullptr;
        newInventory.count = 0;

        graph = newGraph;

        campRoot = newCamps;
        newCamps = nullptr;

        dispatchQueue = newDispatchQueue;
        dispatchHistory = newHistory;

        requestQueue.swap(newRequests);
        pendingRequests.swap(newPending);

        nextRequestId = newRequestId;
        nextDispatchId = newDispatchId;
        arrivalCounter = newArrival;
        warehouseIndex = newWarehouse;

        return true;
    }
    catch (const exception&) {

        cleanup();
        return false;
    }
}