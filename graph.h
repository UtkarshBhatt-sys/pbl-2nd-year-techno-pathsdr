#ifndef GRAPH_H
#define GRAPH_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_LOCATIONS 50
#define MAX_LOCATION_NAME 60
#define INF 1000000000

typedef struct {
    int vertexCount;

    char locations[MAX_LOCATIONS][MAX_LOCATION_NAME];

    
    int adjacencyMatrix[MAX_LOCATIONS][MAX_LOCATIONS];

} Graph;


void initGraph(Graph *graph);

int addLocation(Graph *graph, const char *name);


int findLocation(const Graph *graph, const char *name);


int addRoad(Graph *graph,
            int source,
            int destination,
            int distance);


int removeRoad(Graph *graph,
               int source,
               int destination);


void displayLocations(const Graph *graph);


void displayGraph(const Graph *graph);

int shortestDistance(const Graph *graph,
                     int source,
                     int destination);

int displayShortestPath(const Graph *graph,
                        int source,
                        int destination);

#ifdef __cplusplus
}
#endif

#endif