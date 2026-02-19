#include "Graph.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <sstream>

const std::vector<char> WAREHOUSE_CITIES = {'B', 'I'};

const std::unordered_map<char, std::string> CITY_LABELS = {
    {'A', "Bimsy"},
    {'B', "Garous"},
    {'C', "Debrecen"},
    {'D', "Lae"},
    {'E', "Merribell"},
    {'F', "Wimbie"},
    {'G', "Aguascalientes"},
    {'H', "Froli"},
    {'I', "Tainan"},
    {'J', "Honara"},
    {'K', "Twinklehollow"},
    {'L', "Trinkleby"}
};

CityGraph buildCityGraph() {
    CityGraph graph;

    auto addEdge = [&](char u, char v, int w) {
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    };

    addEdge('A', 'B', 2);
    addEdge('A', 'C', 5);
    addEdge('B', 'F', 5);
    addEdge('B', 'A', 2);
    addEdge('C', 'A', 5);
    addEdge('C', 'E', 4);
    addEdge('C', 'K', 4);
    addEdge('C', 'L', 3);
    addEdge('C', 'J', 6);
    addEdge('D', 'E', 1);
    addEdge('E', 'F', 2);
    addEdge('E', 'G', 3);
    addEdge('E', 'C', 4);
    addEdge('E', 'D', 1);
    addEdge('F', 'H', 3);
    addEdge('F', 'G', 5);
    addEdge('F', 'B', 5);
    addEdge('F', 'E', 2);
    addEdge('G', 'E', 3);
    addEdge('G', 'H', 4);
    addEdge('G', 'F', 5);
    addEdge('G', 'I', 2);
    addEdge('G', 'J', 4);
    addEdge('G', 'K', 10);
    addEdge('H', 'F', 3);
    addEdge('H', 'G', 4);
    addEdge('I', 'G', 2);
    addEdge('J', 'C', 6);
    addEdge('J', 'G', 4);
    addEdge('J', 'K', 8);
    addEdge('K', 'C', 4);
    addEdge('K', 'J', 8);
    addEdge('L', 'C', 3);

    return graph;
}

PathResult dijkstra(char start, char target, const CityGraph &graph) {
    std::unordered_map<char, int> dist;
    std::unordered_map<char, char> parent;

    for (const auto &entry : graph) {
        dist[entry.first] = std::numeric_limits<int>::max();
    }
    dist[start] = 0;

    using PQItem = std::pair<int, char>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currentDist, node] = pq.top();
        pq.pop();

        if (currentDist != dist[node]) continue;
        if (node == target) break;

        const auto &neighbors = graph.at(node);
        for (const auto &edge : neighbors) {
            char neighbor = edge.first;
            int weight = edge.second;

            if (dist[node] + weight < dist[neighbor]) {
                dist[neighbor] = dist[node] + weight;
                parent[neighbor] = node;
                pq.push({dist[neighbor], neighbor});
            }
        }
    }

    PathResult result;
    if (dist.find(target) == dist.end() || dist[target] == std::numeric_limits<int>::max()) {
        return result;
    }

    result.reachable = true;
    result.distance = dist[target];

    std::vector<char> reversed;
    char cur = target;
    reversed.push_back(cur);
    while (cur != start) {
        cur = parent[cur];
        reversed.push_back(cur);
    }
    std::reverse(reversed.begin(), reversed.end());
    result.path = reversed;

    return result;
}

PathResult shortestRouteFromNearestWarehouse(char destination) {
    CityGraph graph = buildCityGraph();

    PathResult best;
    for (char warehouse : WAREHOUSE_CITIES) {
        PathResult candidate = dijkstra(warehouse, destination, graph);
        if (!candidate.reachable) continue;

        if (!best.reachable || candidate.distance < best.distance) {
            best = candidate;
        }
    }

    return best;
}

std::string formatRouteDisplay(const std::vector<char> &path, int distance) {
    if (path.empty()) return "Route unavailable";

    std::stringstream ss;
    for (size_t i = 0; i < path.size(); ++i) {
        ss << path[i];
        if (i + 1 < path.size()) ss << " -> ";
    }
    ss << " | Distance: " << distance;

    return ss.str();
}
