#ifndef MIP_SOLUTION_INFO_CPLEX_HPP_
#define MIP_SOLUTION_INFO_CPLEX_HPP_

#include <ilcplex/ilocplex.h>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

namespace models {
  namespace cplex {
    class Mip_solution_info{
      public:
        double gap;
        IloAlgorithm::Status status;
        double elapsed_time;
        double cost;
        explicit Mip_solution_info ();
        explicit Mip_solution_info (double gap_, IloAlgorithm::Status status_, double elapsed_time, double cost);
        friend ostream& operator<<(ostream& strm, const Mip_solution_info& mip_solution_info){
          stringstream output;
          output<<"Gap:"<<mip_solution_info.gap<<", Status:"<<mip_solution_info.status<<", Elapsed time:"<<mip_solution_info.elapsed_time<<", Cost:"<<mip_solution_info.cost<<endl;
          return strm << output.str();
        };
    };
  }
}
#endif
