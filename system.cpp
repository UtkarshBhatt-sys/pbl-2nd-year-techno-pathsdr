#include "system.hpp"

#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <climits>

using namespace std;

static int readInt(const string& prompt) {
    string line;
    int value;
    char extra;

    while (true) {
        cout << prompt;

        if (!getline(cin, line))
            throw runtime_error("Input closed");

        istringstream input(line);

        if (input >> value && !(input >> extra))
            return value;

        cout << "Invalid input. Enter an integer.\n";
    }
}


static string readText(const string& prompt) {
    string value;

    while (true) {
        cout << prompt;

        if (!getline(cin, value))
            throw runtime_error("Input closed");

        if (!value.empty())
            return value;

        cout << "Input cannot be empty.\n";
    }
}


DisasterSystem::DisasterSystem()
    : campRoot(nullptr),
      nextRequestId(501),
      nextDispatchId(1001),
      arrivalCounter(0),
      warehouseIndex(-1) {

    initInventory(&inventory);
    initGraph(&graph);
    initDispatchQueue(&dispatchQueue);
    initDispatchStack(&dispatchHistory);
}

DisasterSystem::~DisasterSystem() {
    freeInventory(&inventory);
    freeCampTree(campRoot);
}



void DisasterSystem::printHeading(
    const string& title) const {

    cout << "\n========================================\n";
    cout << " " << title << "\n";
    cout << "========================================\n";
}

bool DisasterSystem::isValidCamp(int campId) const {
    return searchCamp(campRoot, campId) != nullptr;
}

bool DisasterSystem::isValidSupply(int supplyId) const {
    return searchSupply(&inventory, supplyId) != nullptr;
}

bool DisasterSystem::canFulfillRequest(
    const ReliefRequest& request) const {

    SupplyNode* supply =
        searchSupply(&inventory, request.getSupplyId());

    if (supply == nullptr)
        return false;

    return supply->quantity >= request.getQuantity();
}



void DisasterSystem::addNewSupply() {
    printHeading("ADD / RESTOCK SUPPLY");

    int id = readInt("Supply ID: ");
    string name = readText("Supply name: ");
    int quantity = readInt("Quantity: ");

    if (name.size() >= MAX_SUPPLY_NAME) {
        cout << "Supply name is too long.\n";
        return;
    }

    if (addSupply(&inventory, id,
                  name.c_str(), quantity)) {

        cout << "Supply added/restocked successfully.\n";

        retryPendingRequests();
    }
    else {
        cout << "Failed to add supply.\n";
        cout << "Check ID, name and quantity.\n";
    }
}

void DisasterSystem::updateSupplyStock() {
    printHeading("UPDATE SUPPLY STOCK");

    int id = readInt("Supply ID: ");
    int quantity = readInt("New quantity: ");

    if (updateSupply(&inventory, id, quantity)) {
        cout << "Stock updated successfully.\n";
        retryPendingRequests();
    }
    else {
        cout << "Update failed.\n";
    }
}

void DisasterSystem::removeSupply() {
    printHeading("REMOVE SUPPLY");

    int id = readInt("Supply ID: ");


    auto copy = requestQueue;

    while (!copy.empty()) {
        if (copy.top().getSupplyId() == id) {
            cout << "Supply has an active request.\n";
            return;
        }
        copy.pop();
    }

    for (const auto& request : pendingRequests) {
        if (request.getSupplyId() == id) {
            cout << "Supply has a pending request.\n";
            return;
        }
    }

    if (deleteSupply(&inventory, id))
        cout << "Supply removed successfully.\n";
    else
        cout << "Supply not found.\n";
}

void DisasterSystem::searchSupplyRecord() {
    printHeading("SEARCH SUPPLY");

    int id = readInt("Supply ID: ");

    SupplyNode* supply = searchSupply(&inventory, id);

    if (supply == nullptr) {
        cout << "Supply not found.\n";
        return;
    }


    Supply object(supply->id, supply->name,
                  supply->quantity, "Relief Material");

    displayEntity(object);
}

void DisasterSystem::showInventory() const {
    printHeading("SUPPLY INVENTORY");
    displaySortedInventory(&inventory);
}



void DisasterSystem::registerCamp() {
    printHeading("REGISTER RELIEF CAMP");

    int id = readInt("Camp ID: ");
    string name = readText("Camp name: ");
    int capacity = readInt("Camp capacity: ");

    displayLocations(&graph);

    int location = readInt("Location index: ");

    if (id <= 0 || capacity <= 0 ||
        name.size() >= MAX_CAMP_NAME ||
        location < 0 ||
        location >= graph.vertexCount) {

        cout << "Invalid camp information.\n";
        return;
    }

    if (isValidCamp(id)) {
        cout << "Camp ID already exists.\n";
        return;
    }

    int before = countCamps(campRoot);

    campRoot = insertCamp(campRoot, id,
                          name.c_str(),
                          location, capacity);

    if (countCamps(campRoot) == before + 1) {
        cout << "Camp registered successfully.\n";
        retryPendingRequests();
    }
    else {
        cout << "Camp registration failed.\n";
    }
}

void DisasterSystem::searchCampRecord() const {
    printHeading("SEARCH RELIEF CAMP");

    int id = readInt("Camp ID: ");

    CampNode* node = searchCamp(campRoot, id);

    if (node == nullptr) {
        cout << "Camp not found.\n";
        return;
    }

    Camp object(node->campId,
                node->campName,
                node->locationIndex,
                node->capacity);

    displayEntity(object);

    cout << "Location: "
         << graph.locations[node->locationIndex]
         << "\n";
}

void DisasterSystem::showAllCamps() const {
    printHeading("REGISTERED RELIEF CAMPS");

    if (campRoot == nullptr) {
        cout << "No camps registered.\n";
        return;
    }

    cout << left
         << setw(10) << "Camp ID"
         << setw(25) << "Camp Name"
         << setw(12) << "Location"
         << setw(10) << "Capacity" << "\n";

    displayCamps(campRoot);
}


void DisasterSystem::registerLocation() {
    printHeading("REGISTER LOCATION");

    string name = readText("Location name: ");

    int index = addLocation(&graph, name.c_str());

    if (index >= 0) {
        cout << "Location registered.\n";
        cout << "Location index: " << index << "\n";
    }
    else {
        cout << "Failed to register location.\n";
    }
}

void DisasterSystem::connectLocations() {
    printHeading("CONNECT LOCATIONS");

    displayLocations(&graph);

    int source = readInt("Source index: ");
    int destination = readInt("Destination index: ");
    int distance = readInt("Road distance (km): ");

    if (addRoad(&graph, source, destination, distance)) {
        cout << "Road connected successfully.\n";
        retryPendingRequests();
    }
    else {
        cout << "Invalid road information.\n";
    }
}

void DisasterSystem::removeRoadConnection() {
    printHeading("REMOVE ROAD");

    displayLocations(&graph);

    int source = readInt("Source index: ");
    int destination = readInt("Destination index: ");

    if (removeRoad(&graph, source, destination))
        cout << "Road removed successfully.\n";
    else
        cout << "Failed to remove road.\n";
}

void DisasterSystem::showRoadNetwork() const {
    printHeading("ROAD NETWORK");

    displayLocations(&graph);
    displayGraph(&graph);
}

void DisasterSystem::setWarehouseLocation() {
    printHeading("SET WAREHOUSE LOCATION");

    displayLocations(&graph);

    int index = readInt("Warehouse location index: ");

    if (index < 0 || index >= graph.vertexCount) {
        cout << "Invalid location index.\n";
        return;
    }

    warehouseIndex = index;

    cout << "Warehouse set to: "
         << graph.locations[index] << "\n";

    retryPendingRequests();
}

void DisasterSystem::findDeliveryRoute() const {
    printHeading("FIND SHORTEST ROUTE");

    if (warehouseIndex < 0) {
        cout << "Set warehouse location first.\n";
        return;
    }

    displayLocations(&graph);

    int destination = readInt("Destination index: ");

    displayShortestPath(&graph,
                        warehouseIndex, destination);
}


void DisasterSystem::createReliefRequest() {
    printHeading("CREATE RELIEF REQUEST");

    int campId = readInt("Camp ID: ");
    int supplyId = readInt("Supply ID: ");
    int quantity = readInt("Required quantity: ");
    int priority = readInt(
        "Priority (1=Normal, 2=High, 3=Critical): "
    );

    if (!isValidCamp(campId)) {
        cout << "Camp does not exist.\n";
        return;
    }

    if (!isValidSupply(supplyId)) {
        cout << "Supply does not exist.\n";
        return;
    }

    if (quantity <= 0 || priority < 1 || priority > 3) {
        cout << "Invalid request details.\n";
        return;
    }

    ReliefRequest request(nextRequestId,
                          campId,
                          supplyId,
                          quantity,
                          priority,
                          arrivalCounter);

    requestQueue.push(request);

    cout << "Request created successfully.\n";
    cout << "Request ID: " << nextRequestId << "\n";

    nextRequestId++;
    arrivalCounter++;
}

void DisasterSystem::showPendingRequests() const {
    printHeading("PENDING RELIEF REQUESTS");

    auto copy = requestQueue;

    if (copy.empty() && pendingRequests.empty()) {
        cout << "No pending requests.\n";
        return;
    }

    cout << "\n--- PRIORITY QUEUE ---\n";

    while (!copy.empty()) {
        copy.top().display();
        copy.pop();
    }

    cout << "\n--- WAITING FOR RESOURCES ---\n";

    for (const auto& request : pendingRequests)
        request.display();
}



bool DisasterSystem::createDispatch(
    const ReliefRequest& request) {

    if (!canFulfillRequest(request))
        return false;

    if (warehouseIndex < 0)
        return false;

    CampNode* camp =
        searchCamp(campRoot, request.getCampId());

    if (camp == nullptr)
        return false;

    int distance = shortestDistance(
        &graph, warehouseIndex, camp->locationIndex
    );

    if (distance < 0)
        return false;

    // Do not lose dispatches when queue/history is full
    if (dispatchQueue.count >= MAX_DISPATCHES ||
        dispatchHistory.top >= MAX_DISPATCHES - 1)
        return false;

    Dispatch dispatch{};

    dispatch.dispatchId = nextDispatchId;
    dispatch.requestId = request.getRequestId();
    dispatch.campId = request.getCampId();
    dispatch.supplyId = request.getSupplyId();
    dispatch.quantity = request.getQuantity();
    dispatch.distance = distance;

    // Reserve the stock
    if (!deductSupply(&inventory,
                      dispatch.supplyId,
                      dispatch.quantity))
        return false;

    if (!enqueueDispatch(&dispatchQueue, dispatch)) {
        // Restore inventory if queue insertion fails
        SupplyNode* supply =
            searchSupply(&inventory, dispatch.supplyId);

        if (supply != nullptr)
            supply->quantity += dispatch.quantity;

        return false;
    }

    cout << "\nDispatch created successfully!\n";
    cout << "Dispatch ID: " << nextDispatchId << "\n";
    cout << "Distance: " << distance << " km\n";

    displayShortestPath(&graph,
                        warehouseIndex,
                        camp->locationIndex);

    nextDispatchId++;
    return true;
}



void DisasterSystem::processPriorityRequest() {
    printHeading("PROCESS PRIORITY REQUEST");

    if (requestQueue.empty()) {
        cout << "No new requests.\n";
        return;
    }

    ReliefRequest request = requestQueue.top();
    requestQueue.pop();

    cout << "\nSelected request:\n";
    request.display();

    if (createDispatch(request)) {
        cout << "Request allocated successfully.\n";
    }
    else {
        cout << "Request cannot be fulfilled now.\n";
        cout << "Keeping it pending.\n";

        pendingRequests.push_back(request);
    }
}



void DisasterSystem::retryPendingRequests() {
    if (pendingRequests.empty())
        return;


    for (const auto& request : pendingRequests)
        requestQueue.push(request);

    pendingRequests.clear();

    cout << "\nWaiting requests returned to priority queue.\n";
    cout << "They can now be processed again.\n";
}


void DisasterSystem::showDispatchQueue() const {
    printHeading("ACTIVE DISPATCHES");
    displayDispatchQueue(&dispatchQueue);
}

void DisasterSystem::completeNextDispatch() {
    printHeading("COMPLETE NEXT DISPATCH");

    Dispatch dispatch{};

    if (!peekDispatch(&dispatchQueue, &dispatch)) {
        cout << "No active dispatches.\n";
        return;
    }

    if (dispatchHistory.top >= MAX_DISPATCHES - 1) {
        cout << "Dispatch history is full.\n";
        return;
    }

    cout << "Dispatch ID: " << dispatch.dispatchId << "\n";
    cout << "Camp ID: " << dispatch.campId << "\n";
    cout << "Quantity: " << dispatch.quantity << "\n";

    int confirmation =
        readInt("Confirm delivery? (1=Yes, 0=No): ");

    if (confirmation != 1) {
        cout << "Delivery not confirmed.\n";
        return;
    }

    if (!pushDispatch(&dispatchHistory, dispatch)) {
        cout << "Unable to record delivery.\n";
        return;
    }

    Dispatch removed{};

    if (!dequeueDispatch(&dispatchQueue, &removed)) {
    
        Dispatch rollback{};
        popDispatch(&dispatchHistory, &rollback);
        cout << "Unable to complete delivery.\n";
        return;
    }

    cout << "Delivery completed successfully.\n";

    retryPendingRequests();
}

void DisasterSystem::showDispatchHistory() const {
    printHeading("DELIVERY HISTORY");
    displayDispatchHistory(&dispatchHistory);
}


void DisasterSystem::showSystemReport() const {
    printHeading("SYSTEM REPORT");

    cout << "Registered supply types: "
         << countSupplies(&inventory) << "\n";

    cout << "Registered camps: "
         << countCamps(campRoot) << "\n";

    cout << "Registered locations: "
         << graph.vertexCount << "\n";

    cout << "New requests in priority queue: "
         << requestQueue.size() << "\n";

    cout << "Waiting requests: "
         << pendingRequests.size() << "\n";

    cout << "Active dispatches: "
         << dispatchQueue.count << "\n";

    cout << "Completed deliveries in history: "
         << dispatchHistory.top + 1 << "\n";

    if (warehouseIndex >= 0)
        cout << "Warehouse: "
             << graph.locations[warehouseIndex] << "\n";
    else
        cout << "Warehouse: Not configured\n";
}


void DisasterSystem::run() {
    int choice;

    do {
        printHeading("DRASTIC VANTAGE");

        cout << "1.  Add / Restock Supply\n";
        cout << "2.  Update Supply Quantity\n";
        cout << "3.  Remove Supply\n";
        cout << "4.  Search Supply\n";
        cout << "5.  Display Inventory\n";
        cout << "6.  Register Relief Camp\n";
        cout << "7.  Search Camp\n";
        cout << "8.  Display Camps\n";
        cout << "9.  Register Location\n";
        cout << "10. Connect Locations\n";
        cout << "11. Remove Road\n";
        cout << "12. Display Road Network\n";
        cout << "13. Set Warehouse Location\n";
        cout << "14. Find Shortest Route\n";
        cout << "15. Create Relief Request\n";
        cout << "16. View Pending Requests\n";
        cout << "17. Process Priority Request\n";
        cout << "18. View Dispatch Queue\n";
        cout << "19. Complete Next Delivery\n";
        cout << "20. View Delivery History\n";
        cout << "21. Generate Report\n";
        cout << "22. Save Data\n";
        cout << "23. Load Data\n";
        cout << "0.  Exit\n";

        try {
            choice = readInt("\nEnter your choice: ");

            switch (choice) {
                case 1: addNewSupply(); break;
                case 2: updateSupplyStock(); break;
                case 3: removeSupply(); break;
                case 4: searchSupplyRecord(); break;
                case 5: showInventory(); break;

                case 6: registerCamp(); break;
                case 7: searchCampRecord(); break;
                case 8: showAllCamps(); break;

                case 9: registerLocation(); break;
                case 10: connectLocations(); break;
                case 11: removeRoadConnection(); break;
                case 12: showRoadNetwork(); break;
                case 13: setWarehouseLocation(); break;
                case 14: findDeliveryRoute(); break;

                case 15: createReliefRequest(); break;
                case 16: showPendingRequests(); break;
                case 17: processPriorityRequest(); break;

                case 18: showDispatchQueue(); break;
                case 19: completeNextDispatch(); break;
                case 20: showDispatchHistory(); break;

                case 21: showSystemReport(); break;

                case 22:
                    if (saveData())
                        cout << "Data saved successfully.\n";
                    else
                        cout << "Save failed.\n";
                    break;

                case 23:
                    if (loadData())
                        cout << "Data loaded successfully.\n";
                    else
                        cout << "Load failed.\n";
                    break;

                case 0:
                    cout << "\nExiting Drastic Vantage...\n";
                    break;

                default:
                    cout << "Invalid menu choice.\n";
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";

            if (!cin)
                return;
        }

    } while (choice != 0);
}