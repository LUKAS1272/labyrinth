#include <stdio.h>
#include <stdlib.h>

// Linked list
typedef struct node {
    int y;
    int x;
    struct node* next;
} node;

node* CreateNode(int yIndex, int xIndex) {
    node *result = malloc(sizeof(node));

    result->y = yIndex;
    result->x = xIndex;
    result->next = NULL;
    
    return result;
}


// Backtrack path
typedef struct path {
    int y;
    int x;
    int shortestPath;
    struct path* from;
} path;

path* CreatePath(int yIndex, int xIndex) {
    path *result = malloc(sizeof(path));

    result->y = yIndex;
    result->x = xIndex;
    // result->shortestPath = -1;
    result->from = NULL;

    return result;
}


int main() {
    char currentChar;
    char **field;
    int xIndex = 0, xLen = 1, yIndex = 0, yLen = 1;

    // Allocates field with 1 row
    field = malloc(yLen * sizeof(char **));
    field[0] = malloc(xLen * sizeof(char *));

    while (scanf("%c", &currentChar) && currentChar != '-' && currentChar != '+') { // Reads input, until the first phase is complete
        if (currentChar == '#' || currentChar == '.') {
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

    node *linkedListField[yLen][xLen];

    // Parsing 2D array
    for (int yAxis = 0; yAxis < yLen; yAxis++) {
    // for (int yAxis = 0; yAxis < 1; yAxis++) {
        for (int xAxis = 0; xAxis < xLen; xAxis++) {
            linkedListField[yAxis][xAxis] = NULL;

            // if (yAxis != 0 && yAxis != yLen - 1 && xAxis != 0 && xAxis != xLen - 1) { // If element is not on the sides
            if (field[yAxis][xAxis] == '.') {
                // printf("A\n");
                if (field[yAxis - 1][xAxis] == '.') { // Is path on the top
                    // printf("Top\n");
                    node *temp = linkedListField[yAxis][xAxis];
                    linkedListField[yAxis][xAxis] = CreateNode(yAxis - 1, xAxis);
                    linkedListField[yAxis][xAxis]->next = temp;
                }

                if (field[yAxis][xAxis + 1] == '.') { // Is path on the right
                    // printf("Right\n");
                    node *temp = linkedListField[yAxis][xAxis];
                    linkedListField[yAxis][xAxis] = CreateNode(yAxis, xAxis + 1);
                    linkedListField[yAxis][xAxis]->next = temp;
                }

                if (field[yAxis + 1][xAxis] == '.') { // Is path on the bottom
                    // printf("Bottom\n");
                    node *temp = linkedListField[yAxis][xAxis];
                    linkedListField[yAxis][xAxis] = CreateNode(yAxis + 1, xAxis);
                    linkedListField[yAxis][xAxis]->next = temp;
                }

                if (field[yAxis][xAxis - 1] == '.') { // Is path on the left
                    // printf("Left\n");
                    node *temp = linkedListField[yAxis][xAxis];
                    linkedListField[yAxis][xAxis] = CreateNode(yAxis, xAxis - 1);
                    linkedListField[yAxis][xAxis]->next = temp;
                }
                // printf("B\n");
            } else {
                // printf("Border\n");
            }

            if (linkedListField[yAxis][xAxis] == NULL) {
                linkedListField[yAxis][xAxis] = CreateNode(-1, -1);
            }
        }

        // printf("\n");
    }


    // Second phase
    int xCordStart, xCordEnd, yCordStart, yCordEnd;
    while (scanf("%d %d %d %d", &yCordStart, &xCordStart, &yCordEnd, &xCordEnd) != EOF) {
        if (field[yCordStart][xCordStart] == '#' || field[yCordEnd][xCordEnd] == '#') { // Coordinates do not exist - invalid path
            printf("-1\n");
        } else if (yCordStart == yCordEnd && xCordStart == xCordEnd) { // Coordinates are the same
            printf("0\n");
        } else {
            // Creates a queue
            node *queue = CreateNode(yCordStart, xCordStart);
            node *queueTail = queue;

            int visited[yLen][xLen]; // Creates a 2D array to check whether the vertex has been already processed
            path *paths[yLen][xLen]; // Creates a 2D array with paths

            // Initializes default values to prevent access errors
            for (int y = 0; y < yLen; y++) {
                for (int x = 0; x < xLen; x++) {
                    visited[y][x] = 0;
                    paths[y][x] = NULL;
                }
            }

            // int pathsCreated = 1;
            // printf("Create path %d\n", pathsCreated);
            paths[yCordStart][xCordStart] = CreatePath(yCordStart, xCordStart); // Creates starting tile path
            visited[yCordStart][xCordStart] = 1;


            while (queue) { // While there are items in queue
                node *currentNeighbor = linkedListField[queue->y][queue->x];

                while (currentNeighbor) { // While there are more neighbors
                    // Enqueue
                    if (visited[currentNeighbor->y][currentNeighbor->x] != 1) { // If the tile hasn't been visited yet
                        // printf("neighbor [%d, %d] of [%d, %d]\n", currentNeighbor->y, currentNeighbor->x, queue->y, queue->x);
                        queueTail->next = CreateNode(currentNeighbor->y, currentNeighbor->x);
                        queueTail = queueTail->next;
                        visited[currentNeighbor->y][currentNeighbor->x] = 1; // To prevent double pathing

                        // pathsCreated++;
                        // printf("Create path %d - [%d, %d]\n", pathsCreated, currentNeighbor->y, currentNeighbor->x);
                        paths[currentNeighbor->y][currentNeighbor->x] = CreatePath(currentNeighbor->y, currentNeighbor->x);
                        paths[currentNeighbor->y][currentNeighbor->x]->from = paths[queue->y][queue->x];
                    }

                    currentNeighbor = currentNeighbor->next; // Passes to the next neighbor
                }
                
                // Dequeue
                node *temp = queue;
                queue = queue->next;
                free(temp);
            }

            // Backtrack
            int distance = 0;
            path *currentPath = paths[yCordEnd][xCordEnd];
            while (currentPath != paths[yCordStart][xCordStart]) {
                if (currentPath == NULL) { distance = -1; break; }
                currentPath = currentPath->from;
                distance++;
            }

            // printf("The distance is: %d\n", distance);
            printf("%d\n", distance);

            // Frees the path
            for (int y = 0; y < yLen; y++) {
                for (int x = 0; x < xLen; x++) {
                    if (paths[y][x]) {
                        free(paths[y][x]);
                    }
                }
            }
        }
    }



    // Default field free
    for (int index = 0; index < yLen; index++) { free(field[index]); }
    free(field);

    // Node field free
    for (int yAxis = 0; yAxis < yLen; yAxis++) {
        for (int xAxis = 0; xAxis < xLen; xAxis++) {
            node *currentNode = linkedListField[yAxis][xAxis];
            node *temp;

            while (currentNode->next != NULL) {
                temp = currentNode;
                // printf("freed node [%d, %d] from [%d, %d]\n", temp->y, temp->x, currentNode->y, currentNode->x);
                currentNode = currentNode->next;
                free(temp);
            }
            free(currentNode);
            // printf("\n");

            // free(linkedListField[yAxis][xAxis]);
        }
    }

    return 0;
}