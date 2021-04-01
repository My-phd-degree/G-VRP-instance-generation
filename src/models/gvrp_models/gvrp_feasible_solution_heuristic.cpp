#include "models/vertex.hpp"
#include "models/distances_enum.hpp"
#include "models/gvrp_models/gvrp_feasible_solution_heuristic.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_solution.hpp"
#include "utils/util.hpp"

#include <float.h>
#include <iostream>
#include <list>
#include <vector>
#include <queue>

using namespace models;
using namespace models::gvrp_models;
using namespace utils;

Gvrp_feasible_solution_heuristic::Gvrp_feasible_solution_heuristic (const Gvrp_instance& gvrp_instance) : Gvrp_heuristic (gvrp_instance) {
  if (gvrp_instance.distances_enum != METRIC)
    throw string("The heuristic 'Feasible Solution' only works for METRIC instances.");
  gvrp_afs_tree = new Gvrp_afs_tree(gvrp_instance);
}

Gvrp_solution Gvrp_feasible_solution_heuristic::run () {
  //create induced graph
  vector<const Vertex *>& f0 = gvrp_afs_tree->f0;
  vector<int>& pred = gvrp_afs_tree->pred;
  vector<double>& fuels = gvrp_afs_tree->fuels;
  vector<double>& times = gvrp_afs_tree->times;
  int sf0 = f0.size(),
         r,
         f;
  //build spanning tree
  double routeFuel,
         fuel,
         cost,
          bestAfssFuel;
  list<list<Vertex> > routes;
  list<Vertex> route;  
  for (const Vertex& customer : gvrp_instance.customers) {
    //arg\ min_{f0[f], f0[r] \in F_0 : c_{fi} + c_{ir} \leqslant \beta} {\pi_f + c_{fi} + \pi_r + c_{ir}}
    pair<int, int> bestAfss = make_pair(-1, -1);
    bestAfssFuel = DBL_MAX;
    for (f = 0; f < sf0; f++) 
      if (f0[f]->id == gvrp_instance.depot.id || pred[f] != f)
        for (r = 0; r < sf0; r++) 
          //if the afss f and r are connected 
          if (f0[r]->id == gvrp_instance.depot.id || pred[r] != r) {
            cost = gvrp_instance.distances[f0[f]->id][customer.id] + gvrp_instance.distances[customer.id][f0[r]->id];
            fuel = cost * gvrp_instance.vehicleFuelConsumptionRate;
            if (fuel <= gvrp_instance.vehicleFuelCapacity) {
              routeFuel = fuels[f] + fuel + fuels[r];
              if (times[f] + (cost/gvrp_instance.vehicleAverageSpeed) + customer.serviceTime + times[r] <= gvrp_instance.timeLimit && routeFuel < bestAfssFuel) {
                bestAfssFuel = routeFuel; 
                bestAfss = make_pair(f, r);
              } 
            }
          }
    //get route
    if (bestAfss.first >= 0) {
      //way
      for (f = bestAfss.first, route.push_back(*f0[f]); f != 0; f = pred[f], route.insert(route.begin(), *f0[f]));
      route.push_back(customer);
      //way back
      for (r = bestAfss.second, route.push_back(*f0[r]); r != 0; r = pred[r], route.push_back(*f0[r]));
      routes.push_back(route);
      route = list<Vertex> ();
    }
  }
  return Gvrp_solution(routes, gvrp_instance);
}
