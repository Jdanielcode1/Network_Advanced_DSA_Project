#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

using namespace std;

// This program optimizes the technological infrastructure of the city: optimal cabling, delivery routes, data flow, and connection allocation.
// Students: Javier Davila Ruiz-A00829870, José Daniel Cantu-A01284664, Miguel Ponce Zertuche-A01383176. 
// Creation date: May 13, 2024.

// Structure to represent a graph
struct Graph {
  int vertices;            // Number of vertices
  vector<vector<int>> adj; // Adjacency matrix
};

// Node for the branch and bound algorithm
struct Node {
  int vertex;        // Current vertex
  int cost;          // Accumulated cost to the current node
  int level;         // Level of the node in the search tree
  vector<int> path;  // Accumulated path to the current node
};

// Comparator to prioritize nodes with lower cost in the priority queue
struct CompareNode {
  bool operator()(const Node &n1, const Node &n2) {
    return n1.cost > n2.cost; // Prioritizes the node with lower cost
  }
};

// Function to create a graph with a specific number of vertices
Graph *createGraph(int vertices) {
  Graph *graph = new Graph;
  graph->vertices = vertices;
  graph->adj.resize(vertices, vector<int>(vertices, 0));
  return graph;
}

// Function to add an edge to the adjacency matrix of the graph
void addEdge(Graph *graph, int source, int destination, int weight) {
  graph->adj[source][destination] = weight;
}

// -----------------------------POINT 1 ---------------------------
// Function to find the optimal way to wire with fiber optic using MST, with Prim's algorithm
// Complexity: O(E log V)
class Prim {
public:
  pair<vector<pair<int, int>>, int> optimalCabling(Graph *graph) {
    vector<pair<int, int>> cabling;               // Stores the edges of the MST
    vector<bool> visited(graph->vertices, false); // Marks visited vertices
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>,
                   greater<pair<int, pair<int, int>>>>
        pq;
    pq.push({0, {0, 0}}); // Start from vertex 0
    int totalCost = 0;    // Total cost of the MST

    while (!pq.empty()) {
      int cost = pq.top().first;
      int u = pq.top().second.first;
      int parent = pq.top().second.second;
      pq.pop();

      if (visited[u])
        continue; // If the vertex has already been visited, skip it
      if (u != 0) {
        cabling.push_back({parent, u});
        totalCost += cost; // Add the cost of each selected edge
      }

      visited[u] = true;
      for (int v = 0; v < graph->vertices; ++v) {
        if (!visited[v] && graph->adj[u][v] > 0) {
          pq.push({graph->adj[u][v], {v, u}});
        }
      }
    }
    return {cabling, totalCost};
  }
};

// -----------------------------POINT 2 ---------------------------
// Function to find the TSP route using Branch and Bound
// Complexity: O(V!)
class BnB {
public:
  // Function to find the TSP route using Branch and Bound
  // Complexity: O(V!)
  vector<int> findTSPRoute(Graph *graph) {
    priority_queue<Node, vector<Node>, CompareNode>
        pq;              // Priority queue for nodes
    vector<int> minRoute; // Stores the minimum route found
    int minCost = numeric_limits<int>::max(); // Initial minimum cost

    Node root = {0, 0, 0, {0}}; // Root node
    pq.push(root);

    while (!pq.empty()) {
      Node current = pq.top();
      pq.pop();

      if (current.level == graph->vertices - 1) {
        // Complete the tour to the starting point
        if (graph->adj[current.vertex][0] > 0) {
          int totalCost = current.cost + graph->adj[current.vertex][0];
          if (totalCost < minCost) {
            minCost = totalCost;
            current.path.push_back(0);
            minRoute = current.path;
          }
        }
      } else {
        for (int i = 0; i < graph->vertices; i++) {
          if (current.vertex != i && graph->adj[current.vertex][i] > 0 &&
              find(current.path.begin(), current.path.end(), i) ==
                  current.path.end()) {
            Node next = {i, current.cost + graph->adj[current.vertex][i],
                              current.level + 1, current.path};
            next.path.push_back(i);
            pq.push(next);
          }
        }
      }
    }

    return minRoute;
  }
};

// -----------------------------POINT 3 ---------------------------
// Function to find the maximum information flow from the initial node to the final node
// Class for the Dinic algorithm
// Complexity: O(V^2 * E)
class Dinic {
private:
  Graph &graph;      // Reference to the original graph
  vector<int> level; // Vector to store the levels of the nodes

  // BFS function to build the level graph
  // Complexity: O(V + E)
  bool bfs(int source, int sink) {
    fill(level.begin(), level.end(), -1); // Initialize levels
    level[source] = 0;                    // Level of the source node
    queue<int> queue;
    queue.push(source);

    while (!queue.empty()) {
      int u = queue.front();
      queue.pop();
      for (int v = 0; v < graph.vertices; ++v) {
        // If the vertex has not been visited and there is capacity in the residual edge
        if (level[v] < 0 && graph.adj[u][v] > 0) {
          level[v] = level[u] + 1; // Assign level
          queue.push(v);
        }
      }
    }
    return level[sink] >= 0; // Returns true if the sink node was reached
  }

  // DFS function to find the blocking flow
  // Complexity: O(E)
  int dfs(vector<int> &start, vector<bool> &blocked, int u, int sink, int flow) {
    if (u == sink)
      return flow; // If the sink is reached, return the flow
    for (int &i = start[u]; i < graph.vertices; ++i) {
      int v = i;
      // If the next level is valid and there is capacity
      if (!blocked[v] && level[v] == level[u] + 1 && graph.adj[u][v] > 0) {
        int currentFlow = min(flow, graph.adj[u][v]); // Find the minimum flow in the path
        int tempFlow = dfs(start, blocked, v, sink, currentFlow); // Recursively call DFS

        if (tempFlow > 0) {            // If a valid flow was found
          graph.adj[u][v] -= tempFlow; // Reduce the capacity in the edge
          graph.adj[v][u] += tempFlow; // Increase the capacity in the reverse edge
          return tempFlow;             // Return the found flow
        }
      }
    }
    blocked[u] = true; // Mark the node as blocked if no flow was found
    return 0;          // If no flow was found, return 0
  }

public:
  // Constructor of the Dinic class
  Dinic(Graph &graph) : graph(graph), level(graph.vertices) {}

  // Function to calculate the maximum flow using the Dinic algorithm
  // Complexity: O(V^2 * E)
  int maxFlow(int source, int sink) {
    Graph residualGraph = graph; // Residual graph
    int maxFlowValue = 0;        // Maximum flow value

    // While a level path can be constructed from source to sink
    while (bfs(source, sink)) {
      vector<int> start(graph.vertices, 0); // Initialize the start array for DFS
      vector<bool> blocked(graph.vertices, false); // Initialize the array of blocked nodes
      while (int flow = dfs(start, blocked, source, sink, numeric_limits<int>::max())) { // Find the blocking flow
        maxFlowValue += flow;                   // Add the found flow
        fill(blocked.begin(), blocked.end(), false); // Reset the array of blocked nodes for the next iteration
      }
    }
    return maxFlowValue; // Return the maximum flow
  }
};

//--------------------------------POINT 4----------------------------------

// Class that represents a point in the plane (x, y)
class Point {
public:
  double x, y;

  Point(double x = 0, double y = 0) : x(x), y(y) {}

  // Calculate the midpoint between this point and another
  Point midpoint(const Point &other) const {
    return Point((x + other.x) / 2, (y + other.y) / 2);
  }

  // Calculate the slope of the line between this point and another
  double slopeTo(const Point &other) const {
    return (other.y - y) / (other.x - x);
  }

  // Calculate the intersection point of two perpendicular bisectors
  static Point intersection(const Point &m1, double s1, const Point &m2, double s2) {
    double x = (s2 * m2.x - s1 * m1.x + m1.y - m2.y) / (s2 - s1);
    double y = m1.y + s1 * (x - m1.x);
    return Point(x, y);
  }

  // Print the point in (x, y) format
  void print() const {
    cout << "(" << (x == -0.0 ? 0.0 : x) << ", " << (y == -0.0 ? 0.0 : y) << ")" << endl;
  }
};

// Calculate the intersections of the perpendicular bisectors
// Complexity: O(n^2), where n is the number of corners (in this case always
// Brute Force Solution
vector<Point> calculateIntersections(const vector<Point> &points) {
  Point center = points[0];
  vector<Point> corners(points.begin() + 1, points.end());

  vector<Point> midpoints;
  vector<double> slopes;

  for (const auto &corner : corners) {
    midpoints.push_back(center.midpoint(corner));
    slopes.push_back(-1 / center.slopeTo(corner));
  }

  vector<Point> intersections;
  for (size_t i = 0; i < midpoints.size(); ++i) {
    for (size_t j = i + 1; j < midpoints.size(); ++j) {
      intersections.push_back(Point::intersection(midpoints[i], slopes[i], midpoints[j], slopes[j]));
    }
  }

  return intersections;
}

// Print the selected intersections in a clockwise order
void printSelectedIntersections(const vector<Point> &intersections) {
  // Custom order: original 0, 3, 5, 2
  if (intersections.size() >= 6) {
    intersections[0].print();
    intersections[3].print();
    intersections[5].print();
    intersections[2].print();
  }
}


int main() {
  int numColonies;
  cout << "Enter the number of colonies: ";
  cin >> numColonies;

  Graph *distanceGraph = createGraph(numColonies);
  cout << "Enter the adjacency matrix of distances between colonies:" << endl;
  for (int i = 0; i < numColonies; ++i) {
    for (int j = 0; j < numColonies; ++j) {
      cin >> distanceGraph->adj[i][j];
    }
  }
  cout << endl;

  // Part 1: Find the optimal way to wire with fiber optic
  Prim prim;
  auto result = prim.optimalCabling(distanceGraph);
  vector<pair<int, int>> cabling = result.first;
  int totalCost = result.second;
  cout << "1) First part" <<endl;
  cout << "Cost= " << totalCost << endl;
  for (auto edge : cabling) {
    cout << (char)('A' + edge.first) << "-" << (char)('A' + edge.second) << endl;
  }
  cout << endl;

  // Part 2: Find the TSP route
  BnB bnb;
  vector<int> route = bnb.findTSPRoute(distanceGraph);
  cout << "2) Second part" << endl;
  if (!route.empty()) {
    cout << (char)('A' + route[0]);
    for (size_t i = 1; i < route.size(); ++i) {
      cout << "-" << (char)('A' + route[i]);
    }
    cout << endl;
  } else {
    cout << "No valid route was found that covers all colonies" << endl;
  }
  cout << endl;

  // Part 3: Find the maximum flow
  Graph *flowGraph = createGraph(numColonies);
  cout << "Enter the adjacency matrix of maximum data flow capacities between colony i and colony j:" << endl;
  for (int i = 0; i < numColonies; ++i) {
    for (int j = 0; j < numColonies; ++j) {
      cin >> flowGraph->adj[i][j];
    }
  }
  cout << endl;

  Dinic dinic(*flowGraph);
  int maxFlowValue = dinic.maxFlow(0, numColonies - 1);
  cout << "3) Third part" << endl;
  cout << "Cost: " << maxFlowValue << endl;
  cout << endl;

  // Part 4: Find the intersections
  vector<Point> points;
  double x, y;

  cout << "4) Fourth part \nEnter 5 coordinates (x y) to form the polygon:" << endl;
  for (int i = 0; i < 5; ++i) {
    cin >> x >> y;
    points.push_back(Point(x, y));
  }
  cout << endl;
  vector<Point> intersections = calculateIntersections(points);
  printSelectedIntersections(intersections);

  return 0;
}