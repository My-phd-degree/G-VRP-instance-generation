#ifndef GVRP_HEURISTIC_HPP_
#define GVRP_HEURISTIC_HPP_

#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_solution.hpp"

namespace models {
  namespace gvrp_models {
    class Gvrp_heuristic {
      public:
        const Gvrp_instance& gvrp_instance;
        explicit Gvrp_heuristic (const Gvrp_instance& gvrp_instance);
        virtual Gvrp_solution run() = 0;
    };
  }
}

#endif
