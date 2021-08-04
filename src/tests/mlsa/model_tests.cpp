#include "tests/mlsa/model_tests.hpp"
#include "models/vrp_instance.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_feasible_solution_heuristic.hpp"
#include "models/mlsa_models/cplex/flow_model.hpp"
#include "utils/util.hpp"
#include "SampleConfig.h"

#include <string>
#include <cmath>
#include <list>
#include <iostream>
#include <fstream>
#include <ilcplex/ilocplex.h>

using namespace std;
using namespace utils;
using namespace tests::mlsa_flow;
using namespace models;
using namespace models::gvrp_models;

Model_tests::Model_tests (bool VERBOSE_, unsigned int execution_time_, unsigned int nIntSol_): VERBOSE(VERBOSE_), execution_time(execution_time_), nIntSol(nIntSol_){}

void Model_tests::run() {
  //experiments
    //setup
  string solution_name = "mlsa_flow_";
  double afssRatio[] = {0.10, 0.20, 0.30};
  ofstream resultsFile;
  resultsFile.open (solution_name + string("results.csv"));
  resultsFile<<"Instance;Solution;C;F;Beta;T;C service time; F service time;GAP;Cost;Time;Status;Ratio"<<endl;
  list<Vrp_instance> vrp_instances;
  int i = 0;
//  list<string> instances = listFilesFromDir (PROJECT_INSTANCES_PATH + string("UchoaEtAl/"));
  list<string> instances = listFilesFromDir (PROJECT_INSTANCES_PATH + string("Augerat1995SetP/"));
    //instance list
  for (const string& instance : instances){
//    Vrp_instance vrp_instance = read_uchoa_vrp_instance(PROJECT_INSTANCES_PATH + string("UchoaEtAl/") + instance);
    Vrp_instance vrp_instance = read_augerat_vrp_1995_setP_instance(PROJECT_INSTANCES_PATH + string("Augerat1995SetP/") + instance);
    vrp_instances.push_back(vrp_instance);
  }
    //executions
  auto vrp_instance = vrp_instances.begin();
  for (const string& instance : instances) {
    cout<<instance<<endl;
    //run
    for (double ratio : afssRatio) {
      Mip_solution_info mip_solution_info;
      Gvrp_instance * gvrp_instance = nullptr;
      double vehicleFuelCapacity = 0.0;
      double elapsedTime = 0;
      //bs
      for (int l = 0, r = 1000000000, m = floor(r/2.0); l <= r; m = floor(l + (r - l)/2.0)) {
        int afssLB = vrp_instance->customers.size() * ratio;
        cout<<l<<" "<<m<<" "<<r<<endl;
        try {
          Flow_model flow_model (*vrp_instance, execution_time, m, afssLB);  
          flow_model.max_num_feasible_integer_sol = nIntSol;
          flow_model.VERBOSE = VERBOSE;
          pair<Gvrp_instance, Mip_solution_info > sol = flow_model.run();    
          gvrp_instance = new Gvrp_instance (sol.first);
          mip_solution_info = sol.second;
          elapsedTime += mip_solution_info.elapsed_time;
          vehicleFuelCapacity = m;
          if (sol.first.afss.size() == afssLB) 
            r = m - 1;
          else 
            l = m + 1;
        } catch (string s){
          cout<<"Error:"<<s;
        } catch (const Mip_solution_info exc) {
//          cout<<"Invalid solution "<<exc<<endl;
          l = m + 1;
        }
      }
      //write in file
      if (gvrp_instance == nullptr)
          resultsFile<<instance<<";-;-;-;-;-;-;-;"<<mip_solution_info.gap<<";"<<mip_solution_info.cost<<";"<<elapsedTime<<";"<<mip_solution_info.status<<";"<<ratio<<endl;
      else {
        string sol_file_name = solution_name + to_string(gvrp_instance->customers.size()) + string ("c") + to_string(gvrp_instance->afss.size()) + string ("f") + to_string(vehicleFuelCapacity) + string("B-") + instance;




//        Gvrp_feasible_solution_heuristic gvrp_feasible_solution_heuristic (*gvrp_instance);
 //       Gvrp_solution gvrp_solution = gvrp_feasible_solution_heuristic.run();
  //      gvrp_solution.write_in_file("./" + sol_file_name);






        gvrp_instance->write_in_csv(PROJECT_INSTANCES_PATH + string("new/") + sol_file_name);
        resultsFile<<instance<<";"<<sol_file_name<<";"<<gvrp_instance->customers.size()<<";"<<gvrp_instance->afss.size()<<";"<<gvrp_instance->vehicleFuelCapacity<<";"<<gvrp_instance->timeLimit<<";"<<gvrp_instance->customers.front().serviceTime<<";"<<gvrp_instance->afss.front().serviceTime<<";"<<mip_solution_info.gap<<";"<<int(mip_solution_info.cost)<<"."<<int(mip_solution_info.cost*100)%100<<";"<<elapsedTime<<";"<<mip_solution_info.status<<";"<<ratio<<endl;
        delete gvrp_instance;
      }
    }
    vrp_instance++;
    i++;
  }
  resultsFile.close();
  resultsFile.clear();
}
