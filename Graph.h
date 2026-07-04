#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>
#include <utility>

struct PathResult {
    bool reachable = false;
    int distance = 0;
    std::vector<char> path;
};

using CityGraph = std::unordered_map<char, std::vector<std::pair<char, int>>>;

extern const std::vector<char> WAREHOUSE_CITIES;
extern const std::unordered_map<char, std::string> CITY_LABELS;

CityGraph buildCityGraph();

PathResult dijkstra(char start, char target, const CityGraph &graph);

PathResult shortestRouteFromNearestWarehouse(char destination);

std::string formatRouteDisplay(const std::vector<char> &path, int distance);

#endif
