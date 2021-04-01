#ifndef UTIL_HPP_
#define UTIL_HPP_

#include "models/vertex.hpp"
#include "models/vrp_instance.hpp"
#include "models/gvrp_models/gvrp_instance.hpp"
#include "models/gvrp_models/gvrp_solution.hpp"
#include "models/gvrp_models/gvrp_afs_tree.hpp"

#include <string>
#include <list>
#include <map>
#include <vector>

using namespace models;
using namespace models::gvrp_models;
using namespace std;

namespace utils {
  Vrp_instance read_augerat_vrp_1995_setP_instance (const string& file_path);
  list<string> listFilesFromDir(string path);
} 
#endif
