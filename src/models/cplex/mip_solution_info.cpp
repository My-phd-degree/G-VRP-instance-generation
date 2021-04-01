#include "models/cplex/mip_solution_info.hpp"

#include <ilcplex/ilocplex.h>

using namespace models::cplex;

Mip_solution_info::Mip_solution_info(): gap(), status(), elapsed_time(), cost() {
  
}

Mip_solution_info::Mip_solution_info(double gap_, IloAlgorithm::Status status_, double elapsed_time_, double cost_) : gap(gap_), status(status_), elapsed_time(elapsed_time_), cost(cost_) {
}
