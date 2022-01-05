#ifndef _INTEGRATOR_H
#define _INTEGRATOR_H

#include <vector>

#include "tree.h"

class Integrator {
public:
  Integrator(double);
  void run_simulation(double*, int, const std::vector<Node>&) const;
private:
  double dt, dt_sq;
};

#endif