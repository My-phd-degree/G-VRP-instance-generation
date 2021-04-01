#include "models/gvrp_models/gvrp_heuristic.hpp" 
#include "models/gvrp_models/gvrp_instance.hpp" 
#include "models/gvrp_models/gvrp_solution.hpp" 

using namespace models::gvrp_models;

Gvrp_heuristic::Gvrp_heuristic (const Gvrp_instance& gvrp_instance_) : gvrp_instance (gvrp_instance_) {}
