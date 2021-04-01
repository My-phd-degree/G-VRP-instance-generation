#include "models/distances_enum.hpp"
#include "models/vertex.hpp"
#include "models/vrp_instance.hpp"

#include <list> 
#include <vector> 

using namespace models;

Vrp_instance::Vrp_instance (list<Vertex> customers_, Vertex depot_, vector<vector<double> > distances_, Distances_enum distances_enum_, int maxRoutes_) : customers (customers_), maxRoutes(maxRoutes_), depot (depot_), distances (distances_), distances_enum(distances_enum_) {
}
