#ifndef GVRP_FEASIBLE_SOLUTION_HEURISTIC_HPP_
#define GVRP_FEASIBLE_SOLUTION_HEURISTIC_HPP_

#include "models/gvrp_models/gvrp_heuristic.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_solution.hpp"
#include "models/gvrp_models/gvrp_afs_tree.hpp"

namespace models {
  namespace gvrp_models {
    class Gvrp_feasible_solution_heuristic : public Gvrp_heuristic {
      public:
        explicit Gvrp_feasible_solution_heuristic (const Gvrp_instance& gvrp_instance);
        Gvrp_solution run();
        Gvrp_afs_tree * gvrp_afs_tree;
    };
  }
}

#endif
