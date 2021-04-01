#include "models/distances_enum.hpp"
#include "models/vertex.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"

#include <vector>
#include <list>
#include <fstream>

using namespace models;
using namespace models::gvrp_models;
using namespace std;

Gvrp_instance::Gvrp_instance(list<Vertex> afss_, list<Vertex> customers_, Vertex depot_, double vehicleFuelCapacity_, vector<vector<double> > distances_, Distances_enum distances_enum_, int maxRoutes) : Vrp_instance (customers_, depot_, distances_, distances_enum_, maxRoutes), afss(afss_), vehicleFuelCapacity(vehicleFuelCapacity_) {
  timeLimit = 1000000000;
  vehicleFuelConsumptionRate = 1.0;
  vehicleAverageSpeed = 1.0;
}

double Gvrp_instance::time(int i, int j) const {
  //return distances[i][j] * 60 / vehicleAverageSpeed;
  return distances[i][j] / vehicleAverageSpeed;
}

double Gvrp_instance::fuel(int i, int j) const {
  return distances[i][j] * vehicleFuelConsumptionRate;
}

Gvrp_instance::Gvrp_instance(list<Vertex> _afss, list<Vertex> _customers, Vertex _depot, double _vehicleFuelCapacity, vector<vector<double> > _distances, Distances_enum _distances_enum, int maxRoutes, double _timeLimit, double _vehicleFuelConsumptionRate, double _vehicleAverageSpeed) : Gvrp_instance(_afss, _customers, _depot, _vehicleFuelCapacity, _distances, _distances_enum, maxRoutes) {
  timeLimit = _timeLimit;
  vehicleFuelConsumptionRate = _vehicleFuelConsumptionRate;
  vehicleAverageSpeed = _vehicleAverageSpeed;
}

void Gvrp_instance::write_in_csv(const string& file_path){
  ofstream instanceFile;
  instanceFile.open (file_path);
  //params
  instanceFile<<"Vehicle:"<<endl<<"Average speed:;"<<this->vehicleAverageSpeed<<endl<<"Time limit:;"<<this->timeLimit<<endl<<"Fuel comsumption rate:;"<<this->vehicleFuelConsumptionRate<<endl<<"Fuel Capacity:;"<<this->vehicleFuelCapacity<<endl;
  //depot
  instanceFile <<"Depot:"<<endl;
  instanceFile <<"ID;X;Y;Service Time"<<endl;
  instanceFile<<depot.id<<";"<<depot.x<<";"<<depot.y<<";"<<depot.serviceTime<<endl;
  //customers
  instanceFile <<"Customers:"<<endl;
  instanceFile <<"ID;X;Y;Service Time"<<endl;
  for (const Vertex& customer : this->customers)
    instanceFile<<customer.id<<";"<<customer.x<<";"<<customer.y<<";"<<customer.serviceTime<<endl;
  //afss
  instanceFile <<"AFSs:"<<endl;
  instanceFile <<"ID;X;Y;Service Time"<<endl;
  for (const Vertex& afs: this->afss)
    instanceFile<<afs.id<<";"<<afs.x<<";"<<afs.y<<";"<<afs.serviceTime<<endl;
  instanceFile.close(); 
}
