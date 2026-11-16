#ifndef INVENTORY_H
#define INVENTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SUPPLY_NAME 50

typedef struct SupplyNode {
    int id;
    char name[MAX_SUPPLY_NAME];
    int quantity;
    struct SupplyNode *next;
} SupplyNode;

typedef struct {
    SupplyNode *head;
    int count;
} Inventory;

void initInventory(Inventory *inventory);

int addSupply(Inventory *inventory,
              int id,
              const char *name,
              int quantity);
SupplyNode *searchSupply(const Inventory *inventory,
                         int id);
int updateSupply(Inventory *inventory,
                 int id,
                 int newQuantity);

int deductSupply(Inventory *inventory,
                 int id,
                 int quantity);
int deleteSupply(Inventory *inventory,
                 int id);
void displayInventory(const Inventory *inventory);
void displaySortedInventory(const Inventory *inventory);
int countSupplies(const Inventory *inventory);
void freeInventory(Inventory *inventory);

#ifdef __cplusplus
}
#endif

#endif