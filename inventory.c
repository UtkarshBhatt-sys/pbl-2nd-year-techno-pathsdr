#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"

void initInventory(Inventory *inventory) {
    if (inventory == NULL)
        return;

    inventory->head = NULL;
    inventory->count = 0;
}


SupplyNode *searchSupply(const Inventory *inventory, int id) {
    if (inventory == NULL)
        return NULL;

    SupplyNode *temp = inventory->head;

    while (temp != NULL) {
        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}


int addSupply(Inventory *inventory, int id,
              const char *name, int quantity) {

    if (inventory == NULL || name == NULL ||
        id <= 0 || quantity <= 0 ||
        strlen(name) == 0 ||
        strlen(name) >= MAX_SUPPLY_NAME)
        return 0;

    SupplyNode *existing = searchSupply(inventory, id);

    if (existing != NULL) {
        if (strcmp(existing->name, name) != 0)
            return 0;

        if (quantity > 2147483647 - existing->quantity)
            return 0;

        existing->quantity += quantity;
        return 1;
    }

    SupplyNode *newNode =
        (SupplyNode *)malloc(sizeof(SupplyNode));

    if (newNode == NULL)
        return 0;

    newNode->id = id;
    strcpy(newNode->name, name);
    newNode->quantity = quantity;
    newNode->next = NULL;

    if (inventory->head == NULL) {
        inventory->head = newNode;
    } else {
        SupplyNode *temp = inventory->head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newNode;
    }

    inventory->count++;
    return 1;
}


int updateSupply(Inventory *inventory,
                 int id, int newQuantity) {

    if (newQuantity < 0)
        return 0;

    SupplyNode *supply = searchSupply(inventory, id);

    if (supply == NULL)
        return 0;

    supply->quantity = newQuantity;
    return 1;
}


int deductSupply(Inventory *inventory,
                 int id, int quantity) {

    if (quantity <= 0)
        return 0;

    SupplyNode *supply = searchSupply(inventory, id);

    if (supply == NULL || supply->quantity < quantity)
        return 0;

    supply->quantity -= quantity;
    return 1;
}


int deleteSupply(Inventory *inventory, int id) {
    if (inventory == NULL)
        return 0;

    SupplyNode *temp = inventory->head;
    SupplyNode *previous = NULL;

    while (temp != NULL && temp->id != id) {
        previous = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return 0;

    if (previous == NULL)
        inventory->head = temp->next;
    else
        previous->next = temp->next;

    free(temp);
    inventory->count--;
    return 1;
}


void displayInventory(const Inventory *inventory) {
    if (inventory == NULL || inventory->head == NULL) {
        printf("\nInventory is empty.\n");
        return;
    }

    printf("\n========== SUPPLY INVENTORY ==========\n");
    printf("%-10s %-25s %-10s\n",
           "ID", "Supply Name", "Quantity");

    printf("----------------------------------------------\n");

    SupplyNode *temp = inventory->head;

    while (temp != NULL) {
        printf("%-10d %-25s %-10d\n",
               temp->id, temp->name, temp->quantity);

        temp = temp->next;
    }
}


void displaySortedInventory(const Inventory *inventory) {
    if (inventory == NULL || inventory->head == NULL) {
        printf("\nInventory is empty.\n");
        return;
    }

    int n = inventory->count;

    SupplyNode **arr =
        (SupplyNode **)malloc(n * sizeof(SupplyNode *));

    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    SupplyNode *temp = inventory->head;

    for (int i = 0; i < n; i++) {
        arr[i] = temp;
        temp = temp->next;
    }

    for (int i = 1; i < n; i++) {
        SupplyNode *key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j]->id > key->id) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }

    printf("\n========== SORTED INVENTORY ==========\n");
    printf("%-10s %-25s %-10s\n",
           "ID", "Supply Name", "Quantity");

    printf("----------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        printf("%-10d %-25s %-10d\n",
               arr[i]->id, arr[i]->name, arr[i]->quantity);
    }

    free(arr);
}

int countSupplies(const Inventory *inventory) {
    if (inventory == NULL)
        return 0;

    return inventory->count;
}

void freeInventory(Inventory *inventory) {
    if (inventory == NULL)
        return;

    SupplyNode *temp = inventory->head;

    while (temp != NULL) {
        SupplyNode *nextNode = temp->next;
        free(temp);
        temp = nextNode;
    }

    inventory->head = NULL;
    inventory->count = 0;
}