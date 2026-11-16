#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"


CampNode *insertCamp(CampNode *root,
                     int campId,
                     const char *campName,
                     int locationIndex,
                     int capacity) {

    if (campId <= 0 || campName == NULL ||
        strlen(campName) == 0 ||
        strlen(campName) >= MAX_CAMP_NAME ||
        locationIndex < 0 || capacity <= 0)
        return root;

    if (root == NULL) {
        CampNode *newNode =
            (CampNode *)malloc(sizeof(CampNode));

        if (newNode == NULL) {
            printf("Memory allocation failed.\n");
            return NULL;
        }

        newNode->campId = campId;
        strcpy(newNode->campName, campName);
        newNode->locationIndex = locationIndex;
        newNode->capacity = capacity;

        newNode->left = NULL;
        newNode->right = NULL;

        return newNode;
    }

    if (campId < root->campId) {
        root->left = insertCamp(root->left, campId,
                                campName, locationIndex,
                                capacity);
    }
    else if (campId > root->campId) {
        root->right = insertCamp(root->right, campId,
                                 campName, locationIndex,
                                 capacity);
    }
    else {
        printf("Camp ID already exists.\n");
    }

    return root;
}


CampNode *searchCamp(CampNode *root, int campId) {
    while (root != NULL) {
        if (campId == root->campId)
            return root;

        if (campId < root->campId)
            root = root->left;
        else
            root = root->right;
    }

    return NULL;
}

void displayCamps(CampNode *root) {
    if (root == NULL)
        return;

    displayCamps(root->left);

    printf("%-10d %-25s %-12d %-10d\n",
           root->campId,
           root->campName,
           root->locationIndex,
           root->capacity);

    displayCamps(root->right);
}


int countCamps(CampNode *root) {
    if (root == NULL)
        return 0;

    return 1 + countCamps(root->left)
             + countCamps(root->right);
}


void freeCampTree(CampNode *root) {
    if (root == NULL)
        return;

    freeCampTree(root->left);
    freeCampTree(root->right);

    free(root);
}

void initDispatchQueue(DispatchQueue *queue) {
    if (queue == NULL)
        return;

    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
}


int enqueueDispatch(DispatchQueue *queue,
                    Dispatch dispatch) {

    if (queue == NULL ||
        queue->count >= MAX_DISPATCHES)
        return 0;

    queue->rear =
        (queue->rear + 1) % MAX_DISPATCHES;

    queue->items[queue->rear] = dispatch;
    queue->count++;

    return 1;
}

int dequeueDispatch(DispatchQueue *queue,
                    Dispatch *dispatch) {

    if (queue == NULL || dispatch == NULL ||
        queue->count == 0)
        return 0;

    *dispatch = queue->items[queue->front];

    queue->front =
        (queue->front + 1) % MAX_DISPATCHES;

    queue->count--;

    if (queue->count == 0) {
        queue->front = 0;
        queue->rear = -1;
    }

    return 1;
}

int peekDispatch(const DispatchQueue *queue,
                 Dispatch *dispatch) {

    if (queue == NULL || dispatch == NULL ||
        queue->count == 0)
        return 0;

    *dispatch = queue->items[queue->front];

    return 1;
}


void displayDispatchQueue(const DispatchQueue *queue) {
    if (queue == NULL || queue->count == 0) {
        printf("\nNo pending dispatches.\n");
        return;
    }

    printf("\n========== DISPATCH QUEUE ==========\n");

    printf("%-10s %-10s %-10s %-10s %-10s\n",
           "Dispatch", "Request", "Camp",
           "Supply", "Quantity");

    for (int i = 0; i < queue->count; i++) {
        int index =
            (queue->front + i) % MAX_DISPATCHES;

        Dispatch d = queue->items[index];

        printf("%-10d %-10d %-10d %-10d %-10d\n",
               d.dispatchId, d.requestId,
               d.campId, d.supplyId, d.quantity);
    }
}

void initDispatchStack(DispatchStack *stack) {
    if (stack == NULL)
        return;

    stack->top = -1;
}

int pushDispatch(DispatchStack *stack,
                 Dispatch dispatch) {

    if (stack == NULL ||
        stack->top >= MAX_DISPATCHES - 1)
        return 0;

    stack->top++;
    stack->items[stack->top] = dispatch;

    return 1;
}

// 13. Pop most recent dispatch
int popDispatch(DispatchStack *stack,
                Dispatch *dispatch) {

    if (stack == NULL || dispatch == NULL ||
        stack->top == -1)
        return 0;

    *dispatch = stack->items[stack->top];
    stack->top--;

    return 1;
}

void displayDispatchHistory(const DispatchStack *stack) {
    if (stack == NULL || stack->top == -1) {
        printf("\nNo completed dispatches.\n");
        return;
    }

    printf("\n========== DISPATCH HISTORY ==========\n");

    printf("%-10s %-10s %-10s %-10s %-10s\n",
           "Dispatch", "Request", "Camp",
           "Supply", "Quantity");

    for (int i = stack->top; i >= 0; i--) {
        Dispatch d = stack->items[i];

        printf("%-10d %-10d %-10d %-10d %-10d\n",
               d.dispatchId, d.requestId,
               d.campId, d.supplyId, d.quantity);
    }
}