#include <stdio.h>
#include <string.h>
#include "graph.h"

void initGraph(Graph *graph) {
    if (graph == NULL)
        return;

    graph->vertexCount = 0;

    for (int i = 0; i < MAX_LOCATIONS; i++) {
        graph->locations[i][0] = '\0';

        for (int j = 0; j < MAX_LOCATIONS; j++) {
            if (i == j)
                graph->adjacencyMatrix[i][j] = 0;
            else
                graph->adjacencyMatrix[i][j] = INF;
        }
    }
}

int findLocation(const Graph *graph, const char *name) {
    if (graph == NULL || name == NULL)
        return -1;

    for (int i = 0; i < graph->vertexCount; i++) {
        if (strcmp(graph->locations[i], name) == 0)
            return i;
    }

    return -1;
}

int addLocation(Graph *graph, const char *name) {
    if (graph == NULL || name == NULL)
        return -1;

    size_t length = strlen(name);

    if (length == 0 || length >= MAX_LOCATION_NAME)
        return -1;

    if (graph->vertexCount >= MAX_LOCATIONS)
        return -1;

    if (findLocation(graph, name) != -1)
        return -1;

    int index = graph->vertexCount;

    strcpy(graph->locations[index], name);

    graph->vertexCount++;

    return index;
}

int addRoad(Graph *graph, int source,
            int destination, int distance) {

    if (graph == NULL)
        return 0;

    int n = graph->vertexCount;

    if (source < 0 || destination < 0 ||
        source >= n || destination >= n ||
        source == destination ||
        distance <= 0 || distance >= INF)
        return 0;


    graph->adjacencyMatrix[source][destination] = distance;
    graph->adjacencyMatrix[destination][source] = distance;

    return 1;
}

int removeRoad(Graph *graph, int source,
               int destination) {

    if (graph == NULL)
        return 0;

    int n = graph->vertexCount;

    if (source < 0 || destination < 0 ||
        source >= n || destination >= n ||
        source == destination)
        return 0;

    graph->adjacencyMatrix[source][destination] = INF;
    graph->adjacencyMatrix[destination][source] = INF;

    return 1;
}

void displayLocations(const Graph *graph) {
    if (graph == NULL || graph->vertexCount == 0) {
        printf("\nNo locations registered.\n");
        return;
    }

    printf("\n========== LOCATIONS ==========\n");

    for (int i = 0; i < graph->vertexCount; i++) {
        printf("%d. %s\n", i, graph->locations[i]);
    }
}

void displayGraph(const Graph *graph) {
    if (graph == NULL || graph->vertexCount == 0) {
        printf("\nGraph is empty.\n");
        return;
    }

    int n = graph->vertexCount;

    printf("\n========== ROAD NETWORK ==========\n");

    printf("%8s", "");

    for (int i = 0; i < n; i++)
        printf("%8d", i);

    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("%8d", i);

        for (int j = 0; j < n; j++) {
            if (graph->adjacencyMatrix[i][j] == INF)
                printf("%8s", "INF");
            else
                printf("%8d",
                       graph->adjacencyMatrix[i][j]);
        }

        printf("\n");
    }
}

static int runDijkstra(const Graph *graph,
                       int source,
                       int destination,
                       int parent[]) {

    if (graph == NULL)
        return -1;

    int n = graph->vertexCount;

    if (source < 0 || destination < 0 ||
        source >= n || destination >= n)
        return -1;

    int distance[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS];

    for (int i = 0; i < n; i++) {
        distance[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }

    distance[source] = 0;

    for (int count = 0; count < n; count++) {
        int u = -1;
        int minimum = INF;

    
        for (int i = 0; i < n; i++) {
            if (!visited[i] && distance[i] < minimum) {
                minimum = distance[i];
                u = i;
            }
        }

        
        if (u == -1)
            break;

        visited[u] = 1;

        if (u == destination)
            break;

    
        for (int v = 0; v < n; v++) {
            int weight = graph->adjacencyMatrix[u][v];

            if (!visited[v] &&
                weight != INF &&
                distance[u] <= INF - weight &&
                distance[u] + weight < distance[v]) {

                distance[v] = distance[u] + weight;
                parent[v] = u;
            }
        }
    }

    if (distance[destination] == INF)
        return -1;

    return distance[destination];
}


int shortestDistance(const Graph *graph,
                     int source,
                     int destination) {

    int parent[MAX_LOCATIONS];

    return runDijkstra(graph, source,
                       destination, parent);
}

int displayShortestPath(const Graph *graph,
                        int source,
                        int destination) {

    int parent[MAX_LOCATIONS];

    int distance = runDijkstra(graph, source,
                               destination, parent);

    if (distance == -1) {
        printf("\nNo valid route found.\n");
        return -1;
    }

    int path[MAX_LOCATIONS];
    int pathLength = 0;
    int current = destination;


    while (current != -1 && pathLength < MAX_LOCATIONS) {
        path[pathLength++] = current;
        current = parent[current];
    }

    printf("\n========== SHORTEST ROUTE ==========\n");

    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%s", graph->locations[path[i]]);

        if (i > 0)
            printf(" -> ");
    }

    printf("\nTotal Distance: %d km\n", distance);

    return distance;
}