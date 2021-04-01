#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_afs_tree.hpp"

#include <string> 
#include <iostream> 
#include <float.h>
#include <queue>

using namespace models;
using namespace models::gvrp_models;
using namespace std;

Gvrp_afs_tree::Gvrp_afs_tree(const Gvrp_instance& gvrp_instance) {
  if (gvrp_instance.distances_enum != METRIC)
    throw string("The algorithm 'Gvrp AFS Tree' only works for METRIC instances.");
  //create F0 Set
  int size = gvrp_instance.afss.size() + 1;
  f0.reserve(size);
  f0.push_back(&gvrp_instance.depot);
  for (const Vertex& afs : gvrp_instance.afss)
    f0.push_back(&afs);
  //reserve spaces
  pairTimes = vector<vector<double>> (size, vector<double> (size, DBL_MAX));
  pairCosts = vector<vector<double>> (size, vector<double> (size, DBL_MAX));
  pairPreds = vector<vector<int>> (size, vector<int> (size));
  //dijkstra
  //setup
  int curr,
      f,
      r;
  double cost, 
         time;
  queue<int> q;
  for (f = 0; f < size; ++f) {
    for (r = 0; r < size; ++r) 
      pairPreds[f][r] = r;
    pairTimes[f][f] = f0[f]->serviceTime;
    pairCosts[f][f] = 0.0;
    q.push(f);
    while (!q.empty()) {
      curr = q.front();
      q.pop();
      for (r = 0; r < size; r++) {
        cost = gvrp_instance.distances[f0[curr]->id][f0[r]->id];
        time = (cost / gvrp_instance.vehicleAverageSpeed) + f0[r]->serviceTime; 
        if (cost * gvrp_instance.vehicleFuelConsumptionRate <= gvrp_instance.vehicleFuelCapacity && pairCosts[f][curr] + cost < pairCosts[f][r] && pairTimes[f][curr] + time < pairTimes[f][r]) {
          pairTimes[f][r] = pairTimes[f][curr] + time;
          pairCosts[f][r] = pairCosts[f][curr] + cost;
          pairPreds[f][r] = curr;
          q.push(r);
        }
      }
    }
  }
  fuels = pairCosts[0];
  for (f = 0; f < size; ++f)
    fuels[f] *= gvrp_instance.vehicleFuelConsumptionRate;
  times = pairTimes[0];
  pred = pairPreds[0];
}
