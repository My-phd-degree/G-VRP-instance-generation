#ifndef GVRP_INSTANCE_HPP_
#define GVRP_INSTANCE_HPP_

#include "models/vertex.hpp"
#include "models/vrp_instance.hpp"
#include "models/distances_enum.hpp"

#include <list>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

namespace models{
  namespace gvrp_models {
    class Gvrp_instance : public Vrp_instance {
      public:
        explicit Gvrp_instance(list<Vertex> afss, list<Vertex> customers, Vertex depot, double vehicleFuelCapacity, vector<vector<double> > distances, Distances_enum distances_enum, int maxRoutes);
        explicit Gvrp_instance(list<Vertex> afss, list<Vertex> customers, Vertex depot, double vehicleFuelCapacity, vector<vector<double> > distances, Distances_enum distances_enum, int maxRoutes, double timeLimit, double vehicleFuelConsumptionRate, double vehicleAverageSpeed);
        friend ostream& operator<<(ostream& strm, const Gvrp_instance& gvrp_instance){
          Vrp_instance vrp_instance = gvrp_instance;
          stringstream output;
          output<<vrp_instance<<endl<<"AFSs: ";
          for (auto afs: gvrp_instance.afss)
            output<<endl<<"\t"<<afs;
          output<<endl<<"Vehicle:"<<endl<<"\tAverage speed:"<<gvrp_instance.vehicleAverageSpeed<<endl<<"\tTime limit:"<<gvrp_instance.timeLimit<<endl<<"\tFuel comsumption rate:"<<gvrp_instance.vehicleFuelConsumptionRate<<endl<<"\tFuel Capacity:"<<gvrp_instance.vehicleFuelCapacity<<endl;
          return strm << output.str();
        };
        double time(int i, int j) const;
        double fuel(int i, int j) const;
        void write_in_csv(const string& file_path);
        list<Vertex> afss;
        double vehicleFuelCapacity;
        double timeLimit;
        double vehicleFuelConsumptionRate;
        double vehicleAverageSpeed;
        double lambda;
        double phi;
        double psi;
        double alpha;
    };
  }

}
#endif
