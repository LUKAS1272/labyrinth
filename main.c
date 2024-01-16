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
    struct path* from;
} path;

path* CreatePath(int yIndex, int xIndex) {
    path *result = malloc(sizeof(path));

    result->y = yIndex;
    result->x = xIndex;
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

    // Creates field with linked list of neighbors for each tile
    node ***neighborsField = malloc(yLen * sizeof(node ***));
    for (int y = 0; y < yLen; y++) { neighborsField[y] = malloc(xLen * sizeof(node **)); }

    // Parsing 2D array
    for (int yAxis = 0; yAxis < yLen; yAxis++) {
        for (int xAxis = 0; xAxis < xLen; xAxis++) {
            neighborsField[yAxis][xAxis] = NULL;

            if (field[yAxis][xAxis] == '.') {
                if (field[yAxis - 1][xAxis] == '.') { // Is path on the top
                    node *temp = neighborsField[yAxis][xAxis];
                    neighborsField[yAxis][xAxis] = CreateNode(yAxis - 1, xAxis);
                    neighborsField[yAxis][xAxis]->next = temp;
                }

                if (field[yAxis][xAxis + 1] == '.') { // Is path on the right
                    node *temp = neighborsField[yAxis][xAxis];
                    neighborsField[yAxis][xAxis] = CreateNode(yAxis, xAxis + 1);
                    neighborsField[yAxis][xAxis]->next = temp;
                }

                if (field[yAxis + 1][xAxis] == '.') { // Is path on the bottom
                    node *temp = neighborsField[yAxis][xAxis];
                    neighborsField[yAxis][xAxis] = CreateNode(yAxis + 1, xAxis);
                    neighborsField[yAxis][xAxis]->next = temp;
                }

                if (field[yAxis][xAxis - 1] == '.') { // Is path on the left
                    node *temp = neighborsField[yAxis][xAxis];
                    neighborsField[yAxis][xAxis] = CreateNode(yAxis, xAxis - 1);
                    neighborsField[yAxis][xAxis]->next = temp;
                }
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
            node *queueHead = CreateNode(yCordStart, xCordStart);
            node *queueTail = queueHead;

            int **visited = malloc(yLen * sizeof(int **)); // Creates a 2D array to check whether the vertex has been already processed
            path ***paths = malloc(yLen * sizeof(path ***)); // Creates a 2D array with paths

            for (int y = 0; y < yLen; y++) {
                visited[y] = malloc(xLen * sizeof(int *));
                paths[y] = malloc(xLen * sizeof(path **));
            }

            // Initializes default values to prevent access errors
            for (int y = 0; y < yLen; y++) {
                for (int x = 0; x < xLen; x++) {
                    visited[y][x] = 0;
                    paths[y][x] = NULL;
                }
            }

            paths[yCordStart][xCordStart] = CreatePath(yCordStart, xCordStart); // Creates starting tile path
            visited[yCordStart][xCordStart] = 1;

            while (queueHead) { // While there are items in queue
                node *currentNeighbor = neighborsField[queueHead->y][queueHead->x];

                while (currentNeighbor) { // While there are more neighbors
                    // Enqueue
                    if (visited[currentNeighbor->y][currentNeighbor->x] != 1) { // If the tile hasn't been visited yet
                        queueTail->next = CreateNode(currentNeighbor->y, currentNeighbor->x);
                        queueTail = queueTail->next;
                        visited[currentNeighbor->y][currentNeighbor->x] = 1; // To prevent double pathing

                        paths[currentNeighbor->y][currentNeighbor->x] = CreatePath(currentNeighbor->y, currentNeighbor->x);
                        paths[currentNeighbor->y][currentNeighbor->x]->from = paths[queueHead->y][queueHead->x];
                    }

                    currentNeighbor = currentNeighbor->next; // Passes to the next neighbor
                }
                
                // Dequeue
                node *temp = queueHead;
                queueHead = queueHead->next;
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