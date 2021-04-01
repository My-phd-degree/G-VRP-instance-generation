#include "models/vertex.hpp"
#include "models/cplex/cplex_model.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_afs_tree.hpp"
#include "models/gvrp_models/cplex/gvrp_model.hpp"
#include "utils/util.hpp"

using namespace models;
using namespace models::cplex;
using namespace models::gvrp_models;
using namespace models::gvrp_models::cplex;
using namespace utils;

Gvrp_model::Gvrp_model(const Gvrp_instance& instance, unsigned int time_limit) : Cplex_model(instance, time_limit) {
  if (instance.distances_enum != METRIC)
    throw string("Error: The compact model requires a G-VRP instance with metric distances");
  //gvrp afs tree
  gvrp_afs_tree = new Gvrp_afs_tree(instance);
  //populating all map and customers set
  all[instance.depot.id] = &instance.depot;
  for (const Vertex& customer: instance.customers) {
    all[customer.id] = &customer;
    customers.insert(customer.id);
  }
  for (const Vertex& afs: instance.afss) 
    all[afs.id] = &afs;
} 

Gvrp_model::~Gvrp_model() {
  delete gvrp_afs_tree;
}
