# labyrinth

To enable / disable UI solution, change value of variable `showSolution` on line 95

---

## Undirected Graph

In an undirected graph, if there is a path from vertex A to vertex B, there is also a path from vertex B to vertex A. (The direction of the edges is not considered)

## Directed Graph

In a directed graph, a path from vertex A to vertex B does not imply a path from B to A. (There can be one-way paths)

## Weighted Graph

A weighted graph assigns a value (weight) to each edge. These weights can represent various things, such as distance, time, or costs associated with traveling between two vertices.

## Spanning Tree

A spanning tree is a subset of edges from the original graph that connects all the vertices together. It is part of the minimum spanning tree, ensuring the graph remains connected without any cycles.

## Tree

* A tree is a connected graph that contains no cycles.
* It connects all the vertices of the original graph.
* There is exactly one path between any two vertices.

## Real-World Uses

* Navigation systems (finding the shortest route between locations)
* Routing in networks (finding the fastest data path)
* Mapping and coloring problems (optimizing resource allocation)

---

# Shortest Path Problem

## What is the shortest path?

* The shortest path between two vertices is the one with the smallest sum of edge weights connecting them.
* In an unweighted graph, the shortest path is the one with the least number of edges.

## Negative Edges and Their Impact

* A negative edge is an edge with a negative weight.

  * Example in the real world: discounts on travel, bonuses for speed, etc.
* If a graph contains negative edges, the shortest path could involve these negative edges, potentially leading to an infinite negative cycle.

---

# Algorithm Overview

## Dijkstra's Algorithm

* The fastest known algorithm for finding the shortest paths from a given node to all other nodes in a graph with non-negative edge weights.
* It explores the graph "outward", expanding waves based on the distance (weight) from the starting node rather than the number of edges.
* It uses a priority queue (min-heap) to efficiently choose the next closest node to explore.
* **Time complexity**: O((V + E) \* log V), where V is the number of vertices and E is the number of edges.

## Bellman-Ford Algorithm

* Used to find the shortest paths from a source node to all other nodes in a graph, including those with negative edge weights.
* The algorithm can detect negative weight cycles in the graph.
* It works by iteratively "relaxing" edges, gradually improving the estimated shortest path.

  * Relaxation means checking if the path through an edge gives a shorter path than the current known path.
* **Time complexity**: O(V \* E), where V is the number of vertices and E is the number of edges.

## Floyd-Warshall Algorithm

* Designed to find the shortest paths between all pairs of vertices in a graph.

  * Ideal for applications where you need to compute all shortest paths at once.
* A key advantage is its simplicity, using a 2D matrix to store distances between all pairs of vertices.
* **Time complexity**: O(V^3), where V is the number of vertices.

## A\* (A-star) Algorithm

* A heuristic search algorithm for finding the shortest path in a graph, which combines the efficiency of Dijkstra with a heuristic.
* The heuristic function estimates the cost to reach the destination node from the current node.
* Widely used in pathfinding for games and navigation systems, where speed is crucial.
* **Time complexity**: O(E), where E is the number of edges, but it depends on the quality of the heuristic and the graph structure.

## Johnson’s Algorithm

* Finds the shortest paths between all pairs of vertices in a graph with weighted edges, including negative weights, but no negative cycles.
* It combines the Bellman-Ford algorithm with Dijkstra’s algorithm.
* **Time complexity**: O(V \* E + V^2 \* log V), where V is the number of vertices and E is the number of edges.

---

# Summary / Overview

* If a graph contains negative weight cycles (cycles with a total negative weight), the shortest path may theoretically be infinitely negative because traversing such a cycle reduces the path length indefinitely.
* Algorithms like Dijkstra cannot be used with negative edge weights, as they assume all weights are non-negative.
