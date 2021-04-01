#ifndef _FLOW_MODEL_MLSA_HPP_
#define _FLOW_MODEL_MLSA_HPP_

#include "models/cplex/mip_solution_info.hpp"
#include "models/cplex/cplex_model.hpp"
#include "models/vrp_instance.hpp" 
#include "models/gvrp_models/gvrp_instance.hpp" 
#include "models/vrp_instance.hpp" 

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> Matrix2DVar;
typedef IloArray<IloNumArray> Matrix2DVal;

using namespace models::cplex;
using namespace models::gvrp_models;

namespace models {
  namespace mlsa_models {
    namespace cplex {
      class Flow_model : public Cplex_model<Vrp_instance, Gvrp_instance> {
        public:
          explicit Flow_model (const Vrp_instance& vrp_instance, double timeLimit, double vehicleFuelCapacity);
          explicit Flow_model (const Vrp_instance& vrp_instance, double timeLimit, double vehicleFuelCapacity, int nAFSLB);
          double vehicleFuelCapacity;
          pair<Gvrp_instance, Mip_solution_info> run();
          IloNumVar s;
          Matrix2DVar x;
          Matrix2DVal x_vals;
          int n;
          int nAFSLB;
        protected:
          void createVariables();
          void createObjectiveFunction();
          void createModel();
          void setCustomParameters();
          void fillVals();
          void createGvrp_instance();
          void endVars();
      };
    }
  }
}

#endif
