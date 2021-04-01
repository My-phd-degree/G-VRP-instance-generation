#include "models/vertex.hpp"

using namespace models;

Vertex::Vertex():
  id(), x(), y() {}
Vertex::Vertex(int _id, double _x, double _y) :
  id(_id), x(_x), y(_y), serviceTime(0.0) {}
Vertex::Vertex(int _id, double _x, double _y, double _serviceTime) :
  Vertex(_id, _x, _y) {
  serviceTime = _serviceTime;
    }

