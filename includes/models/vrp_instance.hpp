#ifndef VRP_INSTANCE_HPP_
#define VRP_INSTANCE_HPP_

#include "models/vertex.hpp" 
#include "models/distances_enum.hpp" 

#include <list>
#include <vector>

using namespace std;

namespace models {
  class Vrp_instance {
    public:
      explicit Vrp_instance(list<Vertex> customers, Vertex depot, vector<vector<double> > distances, Distances_enum distances_enum, int maxRoutes);
      friend ostream& operator<<(ostream& strm, const Vrp_instance& vrp_instance){
        stringstream output;
        output<<"Depot:"<<endl<<"\t"<<vrp_instance.depot<<endl;
        output<<"Number of vehicles:"<<endl<<"\t"<<vrp_instance.maxRoutes<<endl;
        output<<endl<<"Customers:";
        for (auto customer: vrp_instance.customers)
          output<<endl<<"\t"<<customer;
        return strm << output.str();
      };
      list<Vertex> customers;
      int maxRoutes;
      int mimaxRoutes;
      Vertex depot;
      vector<vector<double> > distances;
      Distances_enum distances_enum;
  };
}

#endif
