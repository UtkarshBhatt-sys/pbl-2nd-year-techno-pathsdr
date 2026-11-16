#ifndef STRUCTURES_H
#define STRUCTURES_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CAMP_NAME 60
#define MAX_DISPATCHES 100


typedef struct CampNode {
    int campId;
    char campName[MAX_CAMP_NAME];
    int locationIndex;
    int capacity;

    struct CampNode *left;
    struct CampNode *right;
} CampNode;


CampNode *insertCamp(CampNode *root,
                     int campId,
                     const char *campName,
                     int locationIndex,
                     int capacity);

CampNode *searchCamp(CampNode *root, int campId);

void displayCamps(CampNode *root);

int countCamps(CampNode *root);

void freeCampTree(CampNode *root);



typedef struct {
    int dispatchId;
    int requestId;
    int campId;
    int supplyId;
    int quantity;
    int distance;
} Dispatch;

typedef struct {
    Dispatch items[MAX_DISPATCHES];
    int front;
    int rear;
    int count;
} DispatchQueue;

void initDispatchQueue(DispatchQueue *queue);

int enqueueDispatch(DispatchQueue *queue,
                    Dispatch dispatch);

int dequeueDispatch(DispatchQueue *queue,
                    Dispatch *dispatch);

int peekDispatch(const DispatchQueue *queue,
                 Dispatch *dispatch);
void displayDispatchQueue(const DispatchQueue *queue);


typedef struct {
    Dispatch items[MAX_DISPATCHES];
    int top;
} DispatchStack;

void initDispatchStack(DispatchStack *stack);

int pushDispatch(DispatchStack *stack,
                 Dispatch dispatch);

int popDispatch(DispatchStack *stack,
                Dispatch *dispatch);


void displayDispatchHistory(const DispatchStack *stack);

#ifdef __cplusplus
}
#endif

#endif