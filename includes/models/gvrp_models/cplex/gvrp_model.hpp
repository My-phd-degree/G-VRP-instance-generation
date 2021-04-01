#ifndef _GVRP_MODEL_CPLEX_HPP_
#define _GVRP_MODEL_CPLEX_HPP_

#include "models/vertex.hpp" 
#include "models/cplex/mip_solution_info.hpp" 
#include "models/cplex/cplex_model.hpp" 
#include "models/gvrp_models/gvrp_instance.hpp" 
#include "models/gvrp_models/gvrp_afs_tree.hpp" 
#include "models/gvrp_models/gvrp_solution.hpp" 

#include <set> 
#include <map> 
#include <list> 
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

using namespace std;
using namespace models;
using namespace models::cplex;
using namespace models::gvrp_models;

typedef map<int, const Vertex *> IDVertex;

namespace models {
  namespace gvrp_models {
    namespace cplex {
      class Gvrp_model : public Cplex_model<Gvrp_instance, Gvrp_solution> {
        public:
          explicit Gvrp_model(const Gvrp_instance& gvrp_instance, unsigned int time_limit); 
          virtual ~Gvrp_model(); 
          virtual pair<Gvrp_solution, Mip_solution_info> run() = 0;
          set<int> customers;
          IDVertex all;
          Gvrp_afs_tree * gvrp_afs_tree;
        protected:
          virtual void createVariables() = 0;
          virtual void createObjectiveFunction() = 0;
          virtual void createModel() = 0;
          virtual void extraStepsAfterModelCreation() = 0;
          virtual void setCustomParameters() = 0;
          virtual void createGvrp_solution() = 0;
          virtual void endVars() = 0;
          virtual void endVals() = 0;
      };
    }
  }
}

#endif
