#include <stdio.h>
#include <stdlib.h>


// Neighbors linked list
typedef struct node {
    int y;
    int x;
    int distance;
    struct node* next;
} node;

node* CreateNode(int yIndex, int xIndex, int nodeDistance) {
    node *result = malloc(sizeof(node));

    result->y = yIndex;
    result->x = xIndex;
    result->distance = nodeDistance;
    result->next = NULL;
    
    return result;
}


// Heap queue
typedef struct queue {
    int y;
    int x;
    int shortestDistance;
} queue;

queue* CreateQueue(int yIndex, int xIndex, int nodeDistance) {
    queue* result = malloc(sizeof(queue));

    result->y = yIndex;
    result->x = xIndex;
    result->shortestDistance = nodeDistance;

    return result;
}


// Backtrack path
typedef struct path {
    int y;
    int x;
    int shortestDistance;
    struct path* from;
} path;

path* CreatePath(int yIndex, int xIndex) {
    path *result = malloc(sizeof(path));

    result->y = yIndex;
    result->x = xIndex;
    result->shortestDistance = 2147483647; // Maximum value, an int can store - "infinity" 
    result->from = NULL;

    return result;
}


// Neighbor parsing
void addNeighbor(node **target, int neighborY, int neighborX, char pathType) {
    int distance = 0;
    int wasSet = 0;

    if (pathType == '.') {
        distance = 1;
        wasSet = 1;
    } else if (pathType == 'f') {
        distance = 5;
        wasSet = 1;
    }
    
    if (wasSet != 0) {
        node *temp = *target;
        (*target) = CreateNode(neighborY, neighborX, distance);
        (*target)->next = temp;
    }
}

int valueOf(queue *item) {
    if (item) {
        return item->shortestDistance;
    } else {
        return -1;
    }
}


int main() {
    char currentChar;
    char **field;
    int xIndex = 0, xLen = 1, yIndex = 0, yLen = 1;

    // Allocates field with 1 row
    field = malloc(yLen * sizeof(char **));
    field[0] = malloc(xLen * sizeof(char *));

    while (scanf("%c", &currentChar) && currentChar != '-' && currentChar != '+') { // Reads input, until the first phase is complete
        if (currentChar == '#' || currentChar == '.' || currentChar == 'f') {
            field[yIndex][xIndex] = currentChar;
            xIndex++;
            if (yIndex == 0 && xIndex > xLen) { field[0] = realloc(field[0], (xLen *= 2) * sizeof(char *)); } // Doubles the number of columns - needed only on the first row
        } else if (currentChar == '\n') {
            if (yIndex == 0) { field[yIndex] = realloc(field[yIndex], (xLen = xIndex) * sizeof(char *)); } // Shirnks the first row - needed only on the first row

            if (++yIndex >= yLen) { // If next index is out of allocated array
                field = realloc(field, (yLen *= 2) * sizeof(char **));
            }

            field[yIndex] = malloc(xLen * sizeof(char *)); // Allocates next row
            xIndex = 0;
        } 
    }

    free(field[yIndex]); // Frees last allocated row - is not needed
    field = realloc(field, (yLen = yIndex) * sizeof(char **)); // Retrieves only needed rows

    // Creates field with linked list of neighbors for each tile
    node ***neighborsField = malloc(yLen * sizeof(node ***));
    for (int y = 0; y < yLen; y++) { neighborsField[y] = malloc(xLen * sizeof(node **)); }

    // Parsing 2D array
    for (int yAxis = 0; yAxis < yLen; yAxis++) {
        for (int xAxis = 0; xAxis < xLen; xAxis++) {
            neighborsField[yAxis][xAxis] = NULL;

            if (field[yAxis][xAxis] == '.' || field[yAxis][xAxis] == 'f') {
                // Is path on the top
                if (yAxis == 0) // If on the very top - pacman style
                    addNeighbor(&neighborsField[yAxis][xAxis], yLen - 1, xAxis, field[yLen - 1][xAxis]);
                else
                    addNeighbor(&neighborsField[yAxis][xAxis], yAxis - 1, xAxis, field[yAxis - 1][xAxis]);

                // Is path on the right
                if (xAxis == xLen - 1) // If on the very right - pacman style
                    addNeighbor(&neighborsField[yAxis][xAxis], yAxis, 0, field[yAxis][0]);
                else
                    addNeighbor(&neighborsField[yAxis][xAxis], yAxis, xAxis + 1, field[yAxis][xAxis + 1]);

                // Is path on the bottom
                if (yAxis == yLen - 1) // If on the very bottom - pacman style
                    addNeighbor(&neighborsField[yAxis][xAxis], 0, xAxis, field[0][xAxis]);
                else
                    addNeighbor(&neighborsField[yAxis][xAxis], yAxis + 1, xAxis, field[yAxis + 1][xAxis]);

                // Is path on the left
                if (xAxis == 0) // If on the very left
                    addNeighbor(&neighborsField[yAxis][xAxis], yAxis, xLen - 1, field[yAxis][xLen - 1]);
                else
                    addNeighbor(&neighborsField[yAxis][xAxis], yAxis, xAxis - 1, field[yAxis][xAxis - 1]);
            }
        }
    }


    // Second phase
    int xCordStart, xCordEnd, yCordStart, yCordEnd;
    while (scanf("%d %d %d %d", &yCordStart, &xCordStart, &yCordEnd, &xCordEnd) != EOF) {
        if (field[yCordStart][xCordStart] == '#' || field[yCordEnd][xCordEnd] == '#') { // Coordinates do not exist - invalid path
            printf("-1\n");
        } else if (yCordStart == yCordEnd && xCordStart == xCordEnd) { // Coordinates are the same - no need to move
            printf("0\n");
        } else {
            // Creates a queue
            int queueLength = 1, queueAlloced = 100;
            queue **heapQueue = malloc(queueAlloced * sizeof(queue **));

            int **visited = malloc(yLen * sizeof(int **)); // Creates a 2D array to check whether the vertex has been already processed
            path ***paths = malloc(yLen * sizeof(path ***)); // Creates a 2D array with paths

            // Initializes default values to prevent access errors
            for (int i = 0; i < queueAlloced; i++) { heapQueue[i] = NULL; }
            for (int y = 0; y < yLen; y++) {
                visited[y] = malloc(xLen * sizeof(int *));
                paths[y] = malloc(xLen * sizeof(path **));

                for (int x = 0; x < xLen; x++) {
                    visited[y][x] = 0;
                    paths[y][x] = NULL;
                }
            }

            paths[yCordStart][xCordStart] = CreatePath(yCordStart, xCordStart); // Creates starting tile path
            paths[yCordStart][xCordStart]->shortestDistance = 0; // Creates starting tile path
            visited[yCordStart][xCordStart] = 1; // Sets starting cord to visited, so it doesn't get staged by queue
            heapQueue[0] = CreateQueue(yCordStart, xCordStart, 0);

        
            while (heapQueue[0]) { // While there are items in queue
                printf("iteration\n");
                node *currentNeighbor = neighborsField[heapQueue[0]->y][heapQueue[0]->x];

                while (currentNeighbor) { // While there are more neighbors
                    // Enqueue
                    if (visited[currentNeighbor->y][currentNeighbor->x] != 1) { // If the tile hasn't been visited / enqueued yet
                        if (queueLength >= queueAlloced) { // Doubles the memory allocated to the binary heap, if needed
                            queueAlloced *= 2;
                            heapQueue = realloc(heapQueue, queueAlloced * sizeof(queue *));
                        }

                        heapQueue[queueLength] = CreateQueue(currentNeighbor->y, currentNeighbor->x, currentNeighbor->distance);
                        printf("added [%d, %d] to queue end - %d\n", currentNeighbor->y, currentNeighbor->x, queueLength + 1);
                        // printf("current neighbor distance - %d\n", currentNeighbor->distance);
                        visited[currentNeighbor->y][currentNeighbor->x] = 1; // CHECK WHETER TO MOVE SOMEWHERE ELSE

                        // Bubbles up to prevent value violations
                        int currentIndex = queueLength;
                        int currentParent = (currentIndex + 1) / 2;
                        while (currentParent >= 0 && heapQueue[currentParent]->shortestDistance > heapQueue[currentIndex]->shortestDistance) {
                            printf("bubble up\n");
                            queue *temp = heapQueue[currentParent];
                            heapQueue[currentParent] = heapQueue[currentIndex];
                            heapQueue[currentIndex] = temp;
                            
                            currentIndex = currentParent;
                            currentParent = (currentIndex + 1) / 2;
                        }

                        // If the path doesn't exist, create it
                        if (!paths[currentNeighbor->y][currentNeighbor->x])
                            paths[currentNeighbor->y][currentNeighbor->x] = CreatePath(currentNeighbor->y, currentNeighbor->x);

                        // If current neighbor shortest distance is grater than the sum of current queue shortest distance and the distance from queue to the neighbor
                        if (paths[currentNeighbor->y][currentNeighbor->x]->shortestDistance > paths[heapQueue[0]->y][heapQueue[0]->x]->shortestDistance + currentNeighbor->distance) {
                            // Update it
                            paths[currentNeighbor->y][currentNeighbor->x]->shortestDistance = paths[heapQueue[0]->y][heapQueue[0]->x]->shortestDistance + currentNeighbor->distance;
                            paths[currentNeighbor->y][currentNeighbor->x]->from = paths[heapQueue[0]->y][heapQueue[0]->x];
                        }

                        queueLength++;
                    }

                    currentNeighbor = currentNeighbor->next; // Passes to the next neighbor
                }
                
                // Swaps first node with last one
                queue *temp = heapQueue[0];
                heapQueue[0] = heapQueue[queueLength - 1];
                heapQueue[queueLength - 1] = temp;

                // Deletes last node
                heapQueue[--queueLength] = NULL;

                // // Bubble down
                // int currentIndex = 0, leftChild = 1, rightChild = 2;;
                // // add checking whether the nodes exist before looking for its value
                // while (currentIndex < queueLength && valueOf(heapQueue[currentIndex]) > valueOf(heapQueue[leftChild]) || valueOf(heapQueue[currentIndex]) > valueOf(heapQueue[rightChild])) {
                //     printf("bubble down\n");
                //     printf("%d %d\n", heapQueue[currentIndex]->shortestDistance, heapQueue[leftChild]->shortestDistance);
                //     queue *temp = heapQueue[currentIndex];

                //     // if node has both children
                //     // if node has only left child
                //     if (heapQueue[rightChild]) { // If node has both children (there is right child)
                //         if (valueOf(heapQueue[rightChild]) > valueOf(heapQueue[leftChild])) {

                //         } else if (valueOf(heapQueue[leftChild]) > valueOf(heapQueue[rightChild])) {

                //         }
                //     } else if () { // If node has only left child
                //         heapQueue[currentIndex] = heapQueue[leftChild];
                //         heapQueue[currentIndex * 2 + 1] = temp;
                //         currentIndex = currentIndex * 2 + 1;
                //     }

                //     if (heapQueue[leftChild] < heapQueue[rightChild]) { // Swaps parent with left child
                //         heapQueue[currentIndex] = heapQueue[leftChild];
                //         heapQueue[currentIndex * 2 + 1] = temp;
                //         currentIndex = currentIndex * 2 + 1;
                //     } else { // Swaps parent with right child
                //         heapQueue[currentIndex] = heapQueue[rightChild];
                //         heapQueue[rightChild] = temp;
                //         currentIndex = currentIndex * 2 + 2;
                //     }
                // }
            }

            // Backtrack
            int distance = 0;
            path *currentPath = paths[yCordEnd][xCordEnd];
            while (currentPath != paths[yCordStart][xCordStart]) {
                if (currentPath == NULL) { distance = -1; break; }
                currentPath = currentPath->from;
                distance++;
            }

            printf("%d\n", distance);

            // Frees the path
            for (int y = 0; y < yLen; y++) {
                for (int x = 0; x < xLen; x++) {
                    if (paths[y][x]) {
                        free(paths[y][x]);
                    }
                }

                if (visited[y]) { free(visited[y]); }
                if (paths[y]) { free(paths[y]); }
            }

            if (paths) { free(paths); }
            if (visited) { free(visited); }
        }
    }



    // Default field free
    for (int index = 0; index < yLen; index++) { free(field[index]); }
    free(field);

    // Node field free
    for (int yAxis = 0; yAxis < yLen; yAxis++) {
        for (int xAxis = 0; xAxis < xLen; xAxis++) {
            if (!neighborsField[yAxis][xAxis]) { continue; }

            node *currentNode = neighborsField[yAxis][xAxis];
            node *temp;

            while (currentNode->next) {
                temp = currentNode;
                currentNode = currentNode->next;
                free(temp);
            }

            free(currentNode);
        }

        free(neighborsField[yAxis]);
    }

    free(neighborsField);

    return 0;
}