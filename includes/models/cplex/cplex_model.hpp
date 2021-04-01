#ifndef CPLEX_MODEL_HPP_
#define CPLEX_MODEL_HPP_

#include "models/cplex/mip_solution_info.hpp" 

#include <ilcplex/ilocplex.h>
#include <iostream>

ILOSTLBEGIN

namespace models {
  namespace cplex {
    template <class I, class S> class Cplex_model {
      public:
        explicit Cplex_model(const I& instance_, unsigned int time_limit_) : instance(instance_), solution(nullptr), time_limit(time_limit_), max_num_feasible_integer_sol(2100000000), VERBOSE(true) {} 
        virtual ~Cplex_model() {
          if (solution != nullptr) 
            delete solution; 
          env.end();
        } 
        virtual pair<S, Mip_solution_info> run() = 0;
        const I& instance;
        static constexpr double INTEGRALITY_TOL = 1e-5;
        S* solution;
        unsigned int time_limit;//seconds
        unsigned int max_num_feasible_integer_sol;//0 to 2100000000
        bool VERBOSE;
        IloEnv env;
        IloModel model;
      protected:
        IloCplex cplex;
        void setParameters() {
          try{
            if (!VERBOSE)
              cplex.setOut(env.getNullStream());
            cplex.setParam(IloCplex::Param::TimeLimit, time_limit);
            cplex.setParam(IloCplex::Param::ClockType, 2);
            cplex.setParam(IloCplex::Param::MIP::Limits::Solutions, max_num_feasible_integer_sol);
          } catch (IloException& e) {
            throw e;
          } catch (...) {
            throw string("Error in setting parameters");
          }
        }
    };
  }
}

#endif
