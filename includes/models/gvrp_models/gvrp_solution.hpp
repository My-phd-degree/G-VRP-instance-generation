#ifndef GVRP_SOLUTION_HPP_
#define GVRP_SOLUTION_HPP_

#include "models/vertex.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"

#include <list>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>

using namespace models;
using namespace models::gvrp_models;

using namespace std;

namespace models {
  namespace gvrp_models {
    class Gvrp_solution {
      public:
        explicit Gvrp_solution(list<list<Vertex> > routes, Gvrp_instance gvrp_instance);
        void write_in_file(const string& file_path) const;
        list<string> getInfeasibilities () const;
        friend ostream& operator<<(ostream& strm, const Gvrp_solution& gvrp_solution){
          //setup
          //maps of customers and afss
          int depot = gvrp_solution.gvrp_instance.depot.id;
          map<int, Vertex> afss, 
            customers;
          for (auto afs : gvrp_solution.gvrp_instance.afss)
            afss[afs.id] = afs;
          for (auto customer : gvrp_solution.gvrp_instance.customers)
            customers[customer.id] = customer;
          set<int> servedCustomers;
          //streams
          stringstream output, 
                       infeasibilities;
          int i = 1;
          double costs = 0;
          double beta = gvrp_solution.gvrp_instance.vehicleFuelCapacity;
          //for each route
          for (list<Vertex> route : gvrp_solution.routes){
            //setup route vars
            output<<"Route "<<i<<": ";
            auto it = route.begin();
            int curr = it->id,
                next;
            output<<curr;
            double currFuelCapacity = afss.count(curr) || curr == depot ? beta : 0,
                   cost = 0,
                   time = it->serviceTime;
            if (curr != depot)
              infeasibilities<<"Route "<<i<<": Does not start at depot"<<endl;
            else if (customers.count(curr)) {
              if (servedCustomers.count(curr))
                infeasibilities<<"Route "<<i<<": Repeats the customer "<<curr<<endl;
              else
                servedCustomers.insert(curr);
            }
            //for each vertex
            for (int j = 0; j < route.size() - 1; j++){
              it++;
              next = it->id;
              output<<" "<<next;
              double edgeCost = gvrp_solution.gvrp_instance.distances[curr][next],
              edgeFuel = edgeCost * gvrp_solution.gvrp_instance.vehicleFuelConsumptionRate;
              time += gvrp_solution.gvrp_instance.time(curr, next) + it->serviceTime;
              cost += edgeCost;
              currFuelCapacity -= edgeFuel;
              if (currFuelCapacity < 0)
                infeasibilities<<"Route "<<i<<": Edge ("<<curr<<","<<next<<") requires fuel capacity greater than the available "<<currFuelCapacity<<endl;
              if (edgeFuel > beta)
                infeasibilities<<"Route "<<i<<": Edge ("<<curr<<","<<next<<") requires fuel capacity greater than beta"<<endl;
              if (time > gvrp_solution.gvrp_instance.timeLimit)
                infeasibilities<<"Route "<<i<<": In "<<curr<<" time is more than the limit "<<gvrp_solution.gvrp_instance.timeLimit<<endl;
              if (afss.count(next) || next == depot)
                currFuelCapacity = min(max(currFuelCapacity, 0.0) + beta, beta);
              else if (customers.count(next))
                if (servedCustomers.count(next))
                  infeasibilities<<"Route "<<i<<": Repeats the customer "<<next<<endl;
                else
                  servedCustomers.insert(next);
              curr = next;            
            }
            if (next != depot)
              infeasibilities<<"Route "<<i<<": Does not end at depot"<<endl;
            if (time > gvrp_solution.gvrp_instance.timeLimit)
              infeasibilities<<"Route "<<i<<": Does not respect the time constraint"<<endl;
            i++;
            output<<" (cost: "<<cost<<")"<<" (time: "<<time<<")"<<endl;
            costs += cost;
          }
          if (servedCustomers.size() < gvrp_solution.gvrp_instance.customers.size())
            for (auto customer : gvrp_solution.gvrp_instance.customers)
              if (!servedCustomers.count(customer.id))
                infeasibilities<<"Customer "<<customer.id<<"is missing"<<endl;
          output<<"Total cost: "<<costs<<endl;
          if (infeasibilities.rdbuf()->in_avail())
            output<<"Infeasibilities found:"<<endl<<infeasibilities.str();
          return strm << output.str();
        };
        list<list<Vertex> > routes;
        Gvrp_instance gvrp_instance;
        double calculateCost() const;
    };
  }
}
#endif
