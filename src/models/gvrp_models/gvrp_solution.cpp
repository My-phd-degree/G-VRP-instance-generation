#include "models/gvrp_models/gvrp_solution.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/vertex.hpp"

#include <list>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>

using namespace models;
using namespace models::gvrp_models;
using namespace std;

Gvrp_solution::Gvrp_solution(list<list<Vertex> > _routes, Gvrp_instance _gvrp_instance) :
  routes(_routes), gvrp_instance(_gvrp_instance) {
}

list<string> Gvrp_solution::getInfeasibilities () const {
  //setup
  //maps of customers and afss
  int depot = gvrp_instance.depot.id;
  map<int, Vertex> afss, 
    customers;
  for (auto afs : gvrp_instance.afss)
    afss[afs.id] = afs;
  for (auto customer : gvrp_instance.customers)
    customers[customer.id] = customer;
  set<int> servedCustomers;
  //streams
  stringstream output, 
               infeasibility;
  list<string> infeasibilities;
  int i = 1;
  double beta = gvrp_instance.vehicleFuelCapacity;
  //for each route
  for (list<Vertex> route : routes){
    //setup route vars
    auto it = route.begin();
    int curr = it->id,
        next;
    double currFuelCapacity = afss.count(curr) || curr == depot ? beta : 0,
           time = it->serviceTime;
    if (curr != depot) {
      infeasibility<<"Route "<<i<<": Does not start at depot"<<endl;
      infeasibilities.push_back(infeasibility.str());
      infeasibility.clear();
      infeasibility.str("");
    } else if (customers.count(curr)) {
      if (servedCustomers.count(curr)) {
        infeasibility<<"Route "<<i<<": Repeats the customer "<<curr<<endl;
        infeasibilities.push_back(infeasibility.str());
        infeasibility.clear();
        infeasibility.str("");
      } else
        servedCustomers.insert(curr);
    }
    //for each vertex
    for (int j = 0; j < route.size() - 1; j++){
      it++;
      next = it->id;
      double edgeCost = gvrp_instance.distances[curr][next],
      edgeFuel = edgeCost * gvrp_instance.vehicleFuelConsumptionRate;
      time += gvrp_instance.time(curr, next) + it->serviceTime;
      currFuelCapacity -= edgeFuel;
      if (currFuelCapacity < 0) {
        infeasibility<<"Route "<<i<<": Edge ("<<curr<<","<<next<<") requires fuel capacity greater than the available "<<currFuelCapacity<<endl;
        infeasibilities.push_back(infeasibility.str());
        infeasibility.clear();
        infeasibility.str("");
      }
      if (edgeFuel > beta) {
        infeasibility<<"Route "<<i<<": Edge ("<<curr<<","<<next<<") requires fuel capacity greater than beta"<<endl;
        infeasibilities.push_back(infeasibility.str());
        infeasibility.clear();
        infeasibility.str("");
      }
      if (time > gvrp_instance.timeLimit) {
        infeasibility<<"Route "<<i<<": In "<<curr<<" time is more than the limit "<<gvrp_instance.timeLimit<<endl;
        infeasibilities.push_back(infeasibility.str());
        infeasibility.clear();
        infeasibility.str("");
      }
      if (afss.count(next) || next == depot)
        currFuelCapacity = min(max(currFuelCapacity, 0.0) + beta, beta);
      else if (customers.count(next)) {
        if (servedCustomers.count(next)) {
          infeasibility<<"Route "<<i<<": Repeats the customer "<<next<<endl;
          infeasibilities.push_back(infeasibility.str());
          infeasibility.clear();
          infeasibility.str("");
        } else
          servedCustomers.insert(next);
      }
      curr = next;            
    }
    if (next != depot) {
      infeasibility<<"Route "<<i<<": Does not end at depot"<<endl;
      infeasibilities.push_back(infeasibility.str());
      infeasibility.clear();
      infeasibility.str("");
    }
    if (time > gvrp_instance.timeLimit) {
      infeasibility<<"Route "<<i<<": Does not respect the time constraint"<<endl;
      infeasibilities.push_back(infeasibility.str());
      infeasibility.clear();
      infeasibility.str("");
    }
    i++;
  }
  if (servedCustomers.size() < gvrp_instance.customers.size())
    for (auto customer : gvrp_instance.customers)
      if (!servedCustomers.count(customer.id)) {
        infeasibility<<"Customer "<<customer.id<<"is missing"<<endl;
        infeasibilities.push_back(infeasibility.str());
        infeasibility.clear();
        infeasibility.str("");
      }
  return infeasibilities;
}

void Gvrp_solution::write_in_file(const string& file_path) const {
  ofstream solutionFile;
  solutionFile.open (file_path);
  solutionFile <<*this;
  solutionFile.close(); 
}

double Gvrp_solution::calculateCost () const {
  double cost = 0;
  for (const list<Vertex>& route : routes) 
    for (list<Vertex>::const_iterator c = next(route.begin()); c != route.end(); ++c)
      cost += gvrp_instance.distances[prev(c)->id][c->id];
  return cost;
}
