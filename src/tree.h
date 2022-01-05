#ifndef _TREE_H
#define _TREE_H

#include <tuple>
#include <utility>
#include <vector>

#include "particle.h"

struct Node {
  double com_xpos;
  double com_ypos;
  double mass;
  int node_idxs[2][2];
};

bool external(const Node*);
bool external(const std::vector<Node>&, int);
int insert_new_node(std::vector<Node>&, double, double, double);
bool includes(std::pair<double, double>, double, double*);
std::tuple<double, double, double> distance(double*, const Node*);
std::pair<double, double> force(double, double, 
															  std::tuple<double, double, double>);
std::pair<double, double> force_on(double*, const std::vector<Node>&, const Node*, 
                                   std::pair<double, double>, double);
void insert_particle(std::vector<Node>&, double*);
std::pair<double, double> force_on(double*, const std::vector<Node>&);
void insert_particles(std::vector<Node>&, double*, int);

#endif