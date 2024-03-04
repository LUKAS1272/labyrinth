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
    int distance;
} queue;

queue* CreateQueue(int yIndex, int xIndex, int nodeDistance) {
    queue* result = malloc(sizeof(queue));

    result->y = yIndex;
    result->x = xIndex;
    result->distance = nodeDistance;

    return result;
}


// Backtrack path
typedef struct path {
    int y;
    int x;
    int shortestDistance;
    struct path* from;
} path;

path* CreatePath(int yIndex, int xIndex, int distance) {
    path *result = malloc(sizeof(path));

    result->y = yIndex;
    result->x = xIndex;
    result->shortestDistance = distance; 
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
        distance = 4;
        wasSet = 1;
    }
    
    if (wasSet != 0) {
        node *temp = *target;
        (*target) = CreateNode(neighborY, neighborX, distance);
        (*target)->next = temp;
    }
}

int valueOf(queue *node, path ***targetPath) {
    if (node) {
        return targetPath[node->y][node->x]->shortestDistance;
    } else {
        return -1;
    }
}


int main() {
    // 0 - Only distance solution will be showing
    // 1 - UI solution will be shown as well
    int showSolution = 1;

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
        char **solutionField;
        if (showSolution) {
            // Allocates solutionField
            solutionField = malloc(yLen * sizeof(char **));
            for (int i = 0; i < yLen; i++)
                solutionField[i] = malloc(xLen * sizeof(char *));

            // Copies data from field to solutionField
            for (int y = 0; y < yLen; y++)
                for (int x = 0; x < xLen; x++)
                    solutionField[y][x] = field[y][x];
        }

        if (field[yCordStart][xCordStart] != '.' || field[yCordEnd][xCordEnd] != '.') { // Coordinates do not exist - invalid path
            printf("-1 - invalid path, check input file\n");
        } else if (yCordStart == yCordEnd && xCordStart == xCordEnd && !showSolution) { // Coordinates are the same - no need to move
            printf("0\n");
        } else {
            // Creates a queue
            int queueLength = 1, queueAlloced = 100;
            queue **heapQueue = malloc(queueAlloced * sizeof(queue **));

            int **visited = malloc(yLen * sizeof(int **)); // Creates a 2D array to check whether the vertex has been already processed
            path ***paths = malloc(yLen * sizeof(path ***)); // Creates a 2D array with paths

            // Initializes default values to prevent access errors
            for (int y = 0; y < yLen; y++) {
                visited[y] = malloc(xLen * sizeof(int *));
                paths[y] = malloc(xLen * sizeof(path **));

                for (int x = 0; x < xLen; x++) {
                    visited[y][x] = 0;
                    paths[y][x] = NULL;
                }
            }

            paths[yCordStart][xCordStart] = CreatePath(yCordStart, xCordStart, 0); // Creates starting tile path
            visited[yCordStart][xCordStart] = 1; // Sets starting cord to visited, so it doesn't get staged by queue
            heapQueue[0] = CreateQueue(yCordStart, xCordStart, 0);
      

            while (heapQueue[0]) { // While there are items in queue
                node *currentNeighbor = neighborsField[heapQueue[0]->y][heapQueue[0]->x];
                
                while (currentNeighbor) { // While there are more neighbors
                    if (visited[currentNeighbor->y][currentNeighbor->x] != 1) { // If the tile hasn't been visited / enqueued yet
                        if (queueLength >= queueAlloced) { // Doubles the memory allocated to the binary heap, if needed
                            queueAlloced *= 2;
                            heapQueue = realloc(heapQueue, queueAlloced * sizeof(queue *));
                        }

                        // Enqueues and sets as visited
                        heapQueue[queueLength] = CreateQueue(currentNeighbor->y, currentNeighbor->x, currentNeighbor->distance);
                        visited[currentNeighbor->y][currentNeighbor->x] = 1;

                        // Creates path and sets its shortestDistance
                        paths[currentNeighbor->y][currentNeighbor->x] = CreatePath(currentNeighbor->y, currentNeighbor->x, paths[heapQueue[0]->y][heapQueue[0]->x]->shortestDistance + currentNeighbor->distance);

                        if (showSolution) {
                            paths[currentNeighbor->y][currentNeighbor->x]->from = paths[heapQueue[0]->y][heapQueue[0]->x];
                        }

                        // Bubbles up to prevent min heap value violations
                        int currentIndex = queueLength;
                        int currentParent = (currentIndex - 1) / 2;
                        while (currentParent >= 0 && paths[heapQueue[currentParent]->y][heapQueue[currentParent]->x]->shortestDistance > paths[heapQueue[currentIndex]->y][heapQueue[currentIndex]->x]->shortestDistance) {
                            queue *temp = heapQueue[currentParent];
                            heapQueue[currentParent] = heapQueue[currentIndex];
                            heapQueue[currentIndex] = temp;
                            
                            currentIndex = currentParent;
                            currentParent = (currentIndex - 1) / 2;
                        }

                        queueLength++;
                    }

                    currentNeighbor = currentNeighbor->next; // Passes to the next neighbor
                }

                if (heapQueue[0]->y == yCordEnd && heapQueue[0]->x == xCordEnd) {
                    for (int index = 0; index < queueLength; index++) {
                        free(heapQueue[index]);
                    }

                    break;
                }
                
                // Swaps first node with last one
                queue *temp = heapQueue[0];
                heapQueue[0] = heapQueue[queueLength - 1];
                heapQueue[queueLength - 1] = temp;

                // Deletes last node
                free(heapQueue[queueLength - 1]);
                heapQueue[--queueLength] = NULL;

                // Bubble down
                int currentIndex = 0, leftChild = 1, rightChild = 2;
                while (leftChild < queueLength && paths[heapQueue[currentIndex]->y][heapQueue[currentIndex]->x]->shortestDistance > paths[heapQueue[leftChild]->y][heapQueue[leftChild]->x]->shortestDistance || rightChild < queueLength && paths[heapQueue[currentIndex]->y][heapQueue[currentIndex]->x]->shortestDistance > paths[heapQueue[rightChild]->y][heapQueue[rightChild]->x]->shortestDistance) {                    
                    queue *temp = heapQueue[currentIndex];

                    if (heapQueue[rightChild] && valueOf(heapQueue[rightChild], paths) < valueOf(heapQueue[leftChild], paths)) { // If node has both children (there is right child)
                        heapQueue[currentIndex] = heapQueue[rightChild];
                        heapQueue[rightChild] = temp;
                        currentIndex = rightChild;
                    } else { // If node has only left child
                        heapQueue[currentIndex] = heapQueue[leftChild];
                        heapQueue[leftChild] = temp;
                        currentIndex = leftChild;
                    }

                    leftChild = currentIndex * 2 + 1;
                    rightChild = currentIndex * 2 + 2;
                }
            }

            // Outputs the shortest distance
            if (paths[yCordEnd][xCordEnd])
                printf("%d\n", paths[yCordEnd][xCordEnd]->shortestDistance);
            else
                printf("-1\n");

            if (showSolution) {
                path* currentPath = paths[yCordEnd][xCordEnd];
                while (currentPath && currentPath->from) {
                    solutionField[currentPath->y][currentPath->x] = 'x';
                    currentPath = currentPath->from;
                }
                solutionField[yCordStart][xCordStart] = 'S';
                solutionField[yCordEnd][xCordEnd] = 'E';

                // Prints out solutionField
                for (int y = 0; y < yLen; y++) {
                    for (int x = 0; x < xLen; x++) {
                        if (solutionField[y][x] == 'x') {
                            printf("\033[1;3;40m%c\033[0m", field[y][x]);
                        } else if (solutionField[y][x] == 'S') {
                            printf("\033[1;3;40;33m%c\033[0m", solutionField[y][x]);
                        } else if (solutionField[y][x] == 'E') {
                            printf("\033[1;3;40;32m%c\033[0m", solutionField[y][x]);
                        } else {
                            printf("\033[2;40;37m%c\033[0m", solutionField[y][x]);
                        }
                    }

                    printf("\n");    
                }
                printf("\n");

                for (int i = 0; i < yLen; i++) { free(solutionField[i]); }
                free(solutionField);
            }

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
            if (heapQueue) { free(heapQueue); }
        }
    }



    // Default field free
    for (int index = 0; index < yLen; index++) { free(field[index]); }
    free(field);

    // Neighbors field free
    for (int yAxis = 0; yAxis < yLen; yAxis++) {
        for (int xAxis = 0; xAxis < xLen; xAxis++) {
            if (!neighborsField[yAxis][xAxis]) { continue; }
            node *currentNode = neighborsField[yAxis][xAxis];

            while (currentNode->next) {
                node *temp = currentNode;
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