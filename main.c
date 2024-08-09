#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_CITIES 100
#define MAX_NAME_LENGTH 50
#define INFINITY INT_MAX

typedef struct {
    char name[MAX_NAME_LENGTH];
} City;

typedef struct {
    int distance;
    int cost;
    int time; // Time in minutes
} Edge;

City cities[MAX_CITIES];
Edge graph[MAX_CITIES][MAX_CITIES];
int numCities = 0;

int cityIndex(const char *name) {
    for (int i = 0; i < numCities; i++) {
        if (strcmp(cities[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void addCity(const char *name) {
    if (cityIndex(name) == -1) {
        strcpy(cities[numCities].name, name);
        numCities++;
    }
}

void addFlight(const char *src, const char *dest, int distance, int cost, int time) {
    int srcIndex = cityIndex(src);
    int destIndex = cityIndex(dest);
    if (srcIndex != -1 && destIndex != -1) {
        graph[srcIndex][destIndex].distance = distance;
        graph[srcIndex][destIndex].cost = cost;
        graph[srcIndex][destIndex].time = time;
    }
}

void findPaths(int source, int dest, int maxBudget, int currentCost, int currentDistance, int currentTime, int visited[], int path[], int pathIndex) {
    visited[source] = 1;
    path[pathIndex] = source;
    pathIndex++;

    if (source == dest) {
        if (currentCost <= maxBudget) {
            printf("Path: ");
            for (int i = 0; i < pathIndex; i++) {
                printf("%s ", cities[path[i]].name);
                if (i < pathIndex - 1) {
                    printf("(Distance: %d km, Cost: $%d, Time: %d mins) -> ", graph[path[i]][path[i + 1]].distance, graph[path[i]][path[i + 1]].cost, graph[path[i]][path[i + 1]].time);
                }
            }
            printf("\nTotal Distance: %d km, Total Cost: $%d, Total Time: %d mins\n\n", currentDistance, currentCost, currentTime);
        }
    } else {
        for (int i = 0; i < numCities; i++) {
            if (!visited[i] && graph[source][i].distance != 0 && currentCost + graph[source][i].cost <= maxBudget) {
                findPaths(i, dest, maxBudget, currentCost + graph[source][i].cost, currentDistance + graph[source][i].distance, currentTime + graph[source][i].time, visited, path, pathIndex);
            }
        }
    }

    pathIndex--;
    visited[source] = 0;
}

void dijkstra(int source, int dest, int maxBudget) {
    int distances[MAX_CITIES];
    int costs[MAX_CITIES];
    int times[MAX_CITIES];
    int visited[MAX_CITIES];
    int parent[MAX_CITIES];

    for (int i = 0; i < numCities; i++) {
        distances[i] = INFINITY;
        costs[i] = INFINITY;
        times[i] = INFINITY;
        visited[i] = 0;
        parent[i] = -1;
    }

    distances[source] = 0;
    costs[source] = 0;
    times[source] = 0;

    for (int count = 0; count < numCities - 1; count++) {
        int u = -1;
        for (int v = 0; v < numCities; v++) {
            if (!visited[v] && (u == -1 || distances[v] < distances[u])) {
                u = v;
            }
        }

        if (u == -1 || u == dest) {
            break;
        }

        visited[u] = 1;

        for (int v = 0; v < numCities; v++) {
            if (!visited[v] && graph[u][v].distance &&
                distances[u] != INFINITY && costs[u] + graph[u][v].cost <= maxBudget &&
                distances[u] + graph[u][v].distance < distances[v]) {
                distances[v] = distances[u] + graph[u][v].distance;
                costs[v] = costs[u] + graph[u][v].cost;
                times[v] = times[u] + graph[u][v].time;
                parent[v] = u;
            }
        }
    }

    // Print shortest path within budget
    if (distances[dest] != INFINITY) {
        printf("Shortest path from %s to %s within budget $%d:\n", cities[source].name, cities[dest].name, maxBudget);
        printf("Total Distance: %d km\n", distances[dest]);
        printf("Total Cost: $%d\n", costs[dest]);
        printf("Total Time: %d mins\n", times[dest]);
        printf("Path: ");
        int current = dest;
        int path[MAX_CITIES];
        int pathIndex = 0;

        while (current != -1) {
            path[pathIndex++] = current;
            current = parent[current];
        }

        for (int i = pathIndex - 1; i >= 0; i--) {
            printf("%s", cities[path[i]].name);
            if (i > 0) {
                printf(" -> ");
            }
        }
        printf("\n\n");
    } else {
        printf("No shortest path found from %s to %s within budget $%d\n", cities[source].name, cities[dest].name, maxBudget);
    }

    // Print all paths within budget
    printf("All paths from %s to %s within budget $%d:\n", cities[source].name, cities[dest].name, maxBudget);
    int path[MAX_CITIES];
    memset(visited, 0, sizeof(visited));
    findPaths(source, dest, maxBudget, 0, 0, 0, visited, path, 0);
}

void printFlightDetails(int source, int dest) {
    printf("(Distance: %d km, Cost: $%d, Time: %d mins) ", graph[source][dest].distance, graph[source][dest].cost, graph[source][dest].time);
}

void bfs(int source, int maxDistance) {
    int visited[MAX_CITIES] = {0};
    int queue[MAX_CITIES];
    int front = 0, rear = 0;

    queue[rear++] = source;
    visited[source] = 1;

    printf("Cities reachable from %s within %d km:\n", cities[source].name, maxDistance);

    while (front < rear) {
        int current = queue[front++];
        printf("%s ", cities[current].name);

        for (int i = 0; i < numCities; i++) {
            if (!visited[i] && graph[current][i].distance != 0 && graph[current][i].distance <= maxDistance) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }
    }

    printf("\n");
}

void readData(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    char src[MAX_NAME_LENGTH], dest[MAX_NAME_LENGTH];
    int distance, cost, time;

    while (fscanf(file, "%s %s %d %d %d", src, dest, &distance, &cost, &time) == 5) {
        addCity(src);
        addCity(dest);
        addFlight(src, dest, distance, cost, time);
    }

    fclose(file);
}

int main() {
    readData("flights.txt");

    printf("Travel Planner\n");
    printf("==============\n");

    // Display available cities
    printf("Available cities:\n");
    for (int i = 0; i < numCities; i++) {
        printf("%d. %s\n", i + 1, cities[i].name);
    }
    printf("\n");

    // Select source city
    int sourceIndex;
    char sourceName[MAX_NAME_LENGTH];
    printf("Enter the number of the source city: ");
    scanf("%d", &sourceIndex);
    if (sourceIndex < 1 || sourceIndex > numCities) {
        printf("Invalid source city number.\n");
        return 1;
    }
    strcpy(sourceName, cities[sourceIndex - 1].name);

    // Select destination city
    int destIndex;
    char destName[MAX_NAME_LENGTH];
    printf("Enter the number of the destination city: ");
    scanf("%d", &destIndex);
    if (destIndex < 1 || destIndex > numCities) {
        printf("Invalid destination city number.\n");
        return 1;
    }
    strcpy(destName, cities[destIndex - 1].name);

    // Specify maximum budget
    int maxBudget;
    printf("Enter your maximum budget: $");
    scanf("%d", &maxBudget);

    printf("\n");

    int source = cityIndex(sourceName);
    int dest = cityIndex(destName);

    if (source != -1 && dest != -1) {
        dijkstra(source, dest, maxBudget);
        printf("\n");
        bfs(source, 500); // Example distance limit
    } else {
        printf("City not found\n");
    }

    return 0;
}
