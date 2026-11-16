#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstddef>

#include "models.hpp"

extern "C" {
    #include "inventory.h"
    #include "graph.h"
    #include "structures.h"
}

using namespace std;

struct RequestComparator {
    bool operator()(const ReliefRequest& a,
                    const ReliefRequest& b) const {
        return a < b;
    }
};

class DisasterSystem {
private:


    Inventory inventory;
    Graph graph;
    CampNode* campRoot;

    DispatchQueue dispatchQueue;
    DispatchStack dispatchHistory;

    priority_queue<
        ReliefRequest,
        vector<ReliefRequest>,
        RequestComparator
    > requestQueue;

    vector<ReliefRequest> pendingRequests;


    int nextRequestId;
    int nextDispatchId;
    long long arrivalCounter;
    int warehouseIndex;


    void printHeading(const string& title) const;

    bool isValidCamp(int campId) const;
    bool isValidSupply(int supplyId) const;

    bool canFulfillRequest(
        const ReliefRequest& request
    ) const;

    bool createDispatch(
        const ReliefRequest& request
    );

    void retryPendingRequests();

    DisasterSystem(const DisasterSystem&) = delete;
    DisasterSystem& operator=(const DisasterSystem&) = delete;

public:
    
    DisasterSystem();
    ~DisasterSystem();


    void addNewSupply();
    void updateSupplyStock();
    void removeSupply();
    void searchSupplyRecord();
    void showInventory() const;

    void registerCamp();
    void searchCampRecord() const;
    void showAllCamps() const;


    void registerLocation();
    void connectLocations();
    void removeRoadConnection();
    void showRoadNetwork() const;
    void findDeliveryRoute() const;
    void setWarehouseLocation();

    

    void createReliefRequest();
    void showPendingRequests() const;
    void processPriorityRequest();


    void showDispatchQueue() const;
    void completeNextDispatch();
    void showDispatchHistory() const;


    void showSystemReport() const;

    bool saveData() const;
    bool loadData();


    void run();
};

#endif