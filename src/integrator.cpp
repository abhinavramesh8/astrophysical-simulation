#include <vector>

#include "integrator.h"
#include "particle.h"
#include "tree.h"

Integrator::Integrator(double dt) {
  this->dt = dt;
  dt_sq = dt * dt;
}

void Integrator::run_simulation(double* particles, int n_eles, 
                                const std::vector<Node>& tree) const {
  for (int i = 0; i < n_eles; i += 5) {
    double* particle = particles + i;
    if (particle[2] < 0) {
      continue;
    }
    double fx, fy;
    std::tie(fx, fy) = force_on(particle, tree);
    double ax = fx / particle[2], ay = fy / particle[2];
    particle[0] += (particle[3] * dt + 0.5 * ax * dt_sq);
    particle[1] += (particle[4] * dt + 0.5 * ay * dt_sq);
    particle[3] += ax * dt;
    particle[4] += ay * dt;
    if (lost(particle)) {
      particle[2] = -1;
    }
  }
}