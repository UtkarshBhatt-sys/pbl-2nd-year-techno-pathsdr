#ifndef MODELS_HPP
#define MODELS_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>

using namespace std;

class ReliefEntity {
protected:
    int id;
    string name;

public:
    
    ReliefEntity(int entityId, const string& entityName)
        : id(entityId), name(entityName) {

        if (id <= 0 || name.empty())
            throw invalid_argument("Invalid entity details");
    }

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    virtual void display() const = 0;

    // Virtual destructor
    virtual ~ReliefEntity() = default;
};

class Supply : public ReliefEntity {
private:
    int quantity;
    string category;

public:
    Supply(int supplyId,
           const string& supplyName,
           int supplyQuantity,
           const string& supplyCategory)
        : ReliefEntity(supplyId, supplyName),
          quantity(supplyQuantity),
          category(supplyCategory) {

        if (quantity < 0)
            throw invalid_argument("Invalid supply quantity");
    }

    int getQuantity() const {
        return quantity;
    }

    string getCategory() const {
        return category;
    }

    void setQuantity(int newQuantity) {
        if (newQuantity < 0)
            throw invalid_argument("Quantity cannot be negative");

        quantity = newQuantity;
    }

    // Function overriding
    void display() const override {
        cout << "\nSupply ID: " << id
             << "\nName: " << name
             << "\nCategory: " << category
             << "\nQuantity: " << quantity << "\n";
    }
};


class Camp : public ReliefEntity {
private:
    int locationIndex;
    int capacity;
    int population;

public:
    Camp(int campId,
         const string& campName,
         int location,
         int campCapacity,
         int campPopulation = 0)
        : ReliefEntity(campId, campName),
          locationIndex(location),
          capacity(campCapacity),
          population(campPopulation) {

        if (locationIndex < 0 ||
            capacity <= 0 ||
            population < 0 ||
            population > capacity)
            throw invalid_argument("Invalid camp details");
    }

    int getLocationIndex() const {
        return locationIndex;
    }

    int getCapacity() const {
        return capacity;
    }

    int getPopulation() const {
        return population;
    }

    void setPopulation(int newPopulation) {
        if (newPopulation < 0 || newPopulation > capacity)
            throw invalid_argument("Invalid camp population");

        population = newPopulation;
    }

    void display() const override {
        cout << "\nCamp ID: " << id
             << "\nCamp Name: " << name
             << "\nLocation Index: " << locationIndex
             << "\nCapacity: " << capacity
             << "\nPopulation: " << population << "\n";
    }
};


class ReliefRequest {
private:
    int requestId;
    int campId;
    int supplyId;
    int quantity;
    int priority;
    long long arrivalOrder;

public:
    ReliefRequest(int rId,
                  int cId,
                  int sId,
                  int qty,
                  int priorityLevel,
                  long long order)
        : requestId(rId),
          campId(cId),
          supplyId(sId),
          quantity(qty),
          priority(priorityLevel),
          arrivalOrder(order) {

        if (requestId <= 0 ||
            campId <= 0 ||
            supplyId <= 0 ||
            quantity <= 0 ||
            priority < 1 ||
            priority > 3 ||
            arrivalOrder < 0)
            throw invalid_argument("Invalid relief request");
    }

    int getRequestId() const {
        return requestId;
    }

    int getCampId() const {
        return campId;
    }

    int getSupplyId() const {
        return supplyId;
    }

    int getQuantity() const {
        return quantity;
    }

    int getPriority() const {
        return priority;
    }

    long long getArrivalOrder() const {
        return arrivalOrder;
    }

    
    bool operator<(const ReliefRequest& other) const {
        if (priority != other.priority)
            return priority < other.priority;

        // Earlier requests receive higher precedence
        return arrivalOrder > other.arrivalOrder;
    }

    void display() const {
        cout << "\nRequest ID: " << requestId
             << "\nCamp ID: " << campId
             << "\nSupply ID: " << supplyId
             << "\nQuantity: " << quantity
             << "\nPriority: " << priority
             << "\nArrival Order: " << arrivalOrder
             << "\n";
    }
};


class Delivery {
private:
    int dispatchId;
    int requestId;
    int campId;
    int distance;
    string status;

public:
    Delivery(int dId,
             int rId,
             int cId,
             int routeDistance)
        : dispatchId(dId),
          requestId(rId),
          campId(cId),
          distance(routeDistance),
          status("Pending") {

        if (dispatchId <= 0 ||
            requestId <= 0 ||
            campId <= 0 ||
            distance < 0)
            throw invalid_argument("Invalid delivery details");
    }

    void updateStatus(const string& newStatus) {
        if (newStatus != "Pending" &&
            newStatus != "In Transit" &&
            newStatus != "Delivered")
            throw invalid_argument("Invalid delivery status");

        status = newStatus;
    }

    string getStatus() const {
        return status;
    }

    int getDispatchId() const {
        return dispatchId;
    }

    void display() const {
        cout << "\nDispatch ID: " << dispatchId
             << "\nRequest ID: " << requestId
             << "\nCamp ID: " << campId
             << "\nDistance: " << distance << " km"
             << "\nStatus: " << status << "\n";
    }
};



inline void displayEntity(const ReliefEntity& entity) {
    entity.display();
}

#endif