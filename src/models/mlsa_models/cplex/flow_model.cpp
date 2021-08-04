#include "models/mlsa_models/cplex/flow_model.hpp"
#include "models/vrp_instance.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/cplex/mip_solution_info.hpp"
#include "models/cplex/cplex_model.hpp"
#include "models/gvrp_models/gvrp_feasible_solution_heuristic.hpp"

#include <set> 
#include <queue> 
#include <sstream> 
#include <float.h> 
#include <cstdlib>

using namespace models::mlsa_models::cplex;
using namespace models::gvrp_models;
using namespace std;

Flow_model::Flow_model (const Vrp_instance& vrp_instance, double timeLimit, double vehicleFuelCapacity_) : Cplex_model (vrp_instance, timeLimit), vehicleFuelCapacity (vehicleFuelCapacity_), n(vrp_instance.customers.size() + 1), nAFSLB(0) {
}

Flow_model::Flow_model (const Vrp_instance& vrp_instance, double timeLimit, double vehicleFuelCapacity_, int nAFSLB_) : Cplex_model (vrp_instance, timeLimit), vehicleFuelCapacity (vehicleFuelCapacity_), n(vrp_instance.customers.size() + 1), nAFSLB(nAFSLB_) {
}

pair<Gvrp_instance, Mip_solution_info> Flow_model::run () {
  //setup
  stringstream output_exception;
  Mip_solution_info mipSolInfo;
  try {
//    cout<<"Creating variables"<<endl;
    createVariables();
//    cout<<"Creating objective function"<<endl;
    createObjectiveFunction();
//    cout<<"Creating model"<<endl;
    createModel();
//    cout<<"Setting parameters"<<endl;
    setCustomParameters();
//    cout<<"Solving model"<<endl;
    if ( !cplex.solve() ) {
//      env.error() << "Failed to optimize LP." << endl;
      mipSolInfo = Mip_solution_info(-1, cplex.getStatus(), -1, -1);
      endVars();
//      env.end();
      throw mipSolInfo;
    }
//    cplex.exportModel("cplexcpp.lp");
//    env.out() << "Solution value = " << cplex.getObjValue() << endl;
//    cout<<"Getting values"<<endl;
    fillVals();
    //cout<<"Creating VRP instance"<<endl;
    createGvrp_instance();
    mipSolInfo = Mip_solution_info(cplex.getMIPRelativeGap(), cplex.getStatus(), cplex.getTime(), cplex.getObjValue());
    endVars();
//    env.end();
    return make_pair(*solution, mipSolInfo);
  } catch (IloException& e) {
    output_exception<<"Concert exception caught: " << e<<endl;
    throw output_exception.str();
  } catch (string s) {
    throw s;
  }
}

void Flow_model::createVariables(){
  try {
    stringstream nameStream;
    x = Matrix2DVar (env, n);
    double floatVarUB = 2 * (n - 1);
    //s var
    s = IloNumVar (env, 0, n - 1, IloNumVar::Int);
    nameStream<<"s";
    s.setName(nameStream.str().c_str());
    nameStream.clear();
    nameStream.str("");
    //x var
    for (int i = 0; i < n; i++){
      x[i] = IloNumVarArray(env, n - 1, 0, floatVarUB, IloNumVar::Float);
      //setting names
      for (int j = 0; j < n - 1; j++) {
        nameStream<<"x["<<i<<"]["<<j<<"]";
        x[i][j].setName(nameStream.str().c_str());
        nameStream.clear();
        nameStream.str("");
      }
      //terminal int var
      x[i].add(IloNumVar (env, 0, 1, IloNumVar::Int));
      nameStream<<"x["<<i<<"]["<<n - 1<<"]";
      x[i][n - 1].setName(nameStream.str().c_str());
      nameStream.clear();
      nameStream.str("");
    }
  }catch (IloException& e) {
    throw e;
  } catch(...){
    throw string("Error in creating variables");
  }
}

void Flow_model::createObjectiveFunction() {
//objective function
  try{
    IloExpr fo (env);
    for (int i = 1; i < n; i++) 
      fo +=  x[i][n - 1];
    model = IloModel (env);
    model.add(IloMaximize(env, fo));
  } catch (IloException& e) {
    throw e;
  } catch(...){
    throw string("Error in creating the objective function");
  }
}

void Flow_model::createModel() {
  try {
    //constraints
    IloExpr expr(env);    
    IloConstraint c;
    stringstream constraintName;
    //s
    for (int i = 1; i < n; i++) 
      expr += x[i][n - 1];
    c = IloConstraint (s == expr);
    c.setName("s definition");
    model.add(c);
    expr.end();
    expr = IloExpr(env);
    //s \leqslant |V| - 1 - nAFSLB
    c = IloConstraint (s <= int(n) - 1 - nAFSLB);
    c.setName("s definition");
    model.add(c);
    expr.end();
    expr = IloExpr(env);
    //x_{rt} = 0
    c = IloConstraint (x[0][n - 1] == 0);
    c.setName("root can not be a leaf");
    model.add(c);
    expr.end();
    expr = IloExpr(env);
    //\sum_{v_j \in V\backslash{v_t}} x_{rj} = n - 1 + s
    for (int i = 0; i < n - 1; i++) 
      expr += x[0][i];
    c = IloConstraint (expr == n - 1 + s);
    c.setName("amount of flow from the root");
    model.add(c);
    expr.end();
    expr = IloExpr(env);
    //\sum_{v_j \in V} x_{ji} - \sum_{v_k \in V \backslash \{v_r\}} x_{ik} = 1, \forall v_i \in V' \backslash \{v_r, v_t\}
    for (int i = 1; i < n; i++) {
      for (int j = 0; j < n; j++) 
        expr += x[j][i - 1] - x[i][j];
      constraintName<<"decrease the incoming amount of flow by 1 in "<<i;
      c = IloConstraint (expr == 1);
      c.setName(constraintName.str().c_str());
      model.add(c);
      expr.end();
      expr = IloExpr(env);
      constraintName.clear();
      constraintName.str("");
    }
    //\sum_{v_j \in V \backslash \{v_r, v_t\}} f_{ij} + 2 * (n - 2) * f_{it} \leqslant 2 * (n - 2), \forall v_i \in V' \backslash \{v_r, v_t\} 
    for (int i = 1; i < n; i++) {
      for (int j = 0; j < n - 1; j++)
        expr += x[i][j];
      expr += 2 * (int(n) - 2) * x[i][n - 1];
      constraintName<<"if "<<i<<" is a sink";
      c = IloConstraint (expr <= 2 * (int(n) - 2));
      c.setName(constraintName.str().c_str());
      model.add(c);
      expr.end();
      expr = IloExpr(env);
      constraintName.clear();
      constraintName.str("");
    }
    //x_{ij} = 0, \forall v_i \in V \backslash \{v_t\}, \forall v_j \in V \backslash \{v_t, v_r\}: e_{ij} > \beta
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n - 1; j++) {
        if (instance.distances[i][j + 1] > vehicleFuelCapacity) {
          constraintName<<"edge ("<<i<<", "<<j + 1<<") preprocessing";
          c = IloConstraint (x[i][j] == 0.0);
          c.setName(constraintName.str().c_str());
          model.add(c);
          constraintName.clear();
          constraintName.str("");
        }
      }
    }
    //\sum_{v_j \in V : e_{ij} \leqslant \beta/2 && e_{jr} \leqslant \beta} 1 - x_{jt} \geqslant x_{it}, \forall v_i \in V' \backslash \{v_r, v_t\} : e_{ir} > \beta/2
    for (int i = 1; i < n; ++i) {
      if (instance.distances[0][i] > vehicleFuelCapacity/2.0) {
        for (int j = 0; j < n - 1; ++j) 
          if (instance.distances[i][j + 1] <= vehicleFuelCapacity/2.0) 
         // if (instance.distances[i][j + 1] <= vehicleFuelCapacity/2.0 && instance.distances[j + 1][0] <= vehicleFuelCapacity) 
            expr += 1 - x[j + 1][n - 1];
        constraintName<<"if "<<i<<" is a sink then must exists a branch node respecting the EMH constraints";
        c = IloConstraint (expr >= x[i][n - 1]);
        c.setName(constraintName.str().c_str());
        model.add(c);
        expr.end();
        expr = IloExpr(env);
        constraintName.clear();
        constraintName.str("");
      }
    }
    //init
    cplex = IloCplex(model);
  } catch (IloException& e) {
    throw e;
  } catch (string s) {
    throw s;
  }
}

void Flow_model::setCustomParameters(){
  try{
    setParameters();
  } catch (IloException& e) {
    throw e;
  } catch (...) {
    throw string("Error in setting parameters");
  }
}

void Flow_model::fillVals(){
  //getresult
  try{
    x_vals = Matrix2DVal (env, n);
    for (int i = 0; i < n; i++) {
      x_vals[i] = IloNumArray (env, n - 1, 0, 1, IloNumVar::Float);
      x_vals[i].add(IloInt ());
      cplex.getValues(x_vals[i], x[i]);
    }
  } catch (IloException& e) {
    throw e;
  } catch (...) {
    throw string("Error in getting solution");
  }
}

void Flow_model::createGvrp_instance(){
  try{
    //setup
    list<Vertex> customers,
                 afss;
    set<int> customersSet;
    double longestTime = 0, 
           afsServiceTime, 
           customerServiceTime,
           routeTime;
    int nCustomers,
        K = 10;
    //get nodes
    for (const Vertex& v : instance.customers)
      if (x_vals[v.id][n - 1] > INTEGRALITY_TOL) 
        customers.push_back(v);
      else
        afss.push_back(v);
    //create instance
    solution = new Gvrp_instance(afss, customers, instance.depot, vehicleFuelCapacity, instance.distances, instance.distances_enum, customers.size(), DBL_MAX, 1, 1);
    //set average speed
    int sall = customers.size() + afss.size() + 1;
    for (int i = 0; i < sall; ++i)
      for (int j = 0; j < sall; ++j)
        solution->vehicleAverageSpeed = max(solution->vehicleAverageSpeed, instance.distances[i][j]);
    //build set of customers
    for (Vertex& customer : solution->customers) {
      customersSet.insert(customer.id);
      customer.serviceTime = instance.distances[rand()%sall][rand()%sall]/solution->vehicleAverageSpeed;
    }
    //afss service time
    afsServiceTime = 1;
    for (Vertex& afs : solution->afss) 
      afs.serviceTime = afsServiceTime;
    //set time limit
    Gvrp_feasible_solution_heuristic gvrp_feasible_solution_heuristic (*solution);
    Gvrp_solution gvrp_solution = gvrp_feasible_solution_heuristic.run();
    //cout<<gvrp_solution<<endl;
    for (const list<Vertex>& route : gvrp_solution.routes) {
      routeTime = 0;
      nCustomers = 0;
      for (auto curr = route.begin(); curr != --route.end(); ++curr) {
        routeTime += solution->time(curr->id, next(curr)->id);
        if (customersSet.count(curr->id))
          ++nCustomers;
      }
      routeTime += nCustomers * customerServiceTime + (route.size() - nCustomers - 2) * afsServiceTime;
      longestTime = max(longestTime, routeTime);
    }
    solution->timeLimit = ceil(2 * longestTime); 
    //cout<<*solution<<endl;
  } catch (IloException& e) {
    throw e;
  } catch (...) {
    throw string("Error in getting routes");
  }
}

void Flow_model::endVars(){
  for (int i = 0; i < n; i++) {
    x[i].end();
  }
  x.end();
}

