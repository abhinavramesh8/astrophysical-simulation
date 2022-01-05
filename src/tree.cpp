#include <tuple>
#include <utility>
#include <cmath>
#include <vector>

#include "tree.h"

using namespace std;

extern double theta;

bool external(const vector<Node>& nodes, int node_idx) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      if (nodes[node_idx].node_idxs[i][j] != -1) {
        return false;
      }
    }
  }
  return true;
}

bool external(const Node* node) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      if (node->node_idxs[i][j] != -1) {
        return false;
      }
    }
  }
  return true;
}

int insert_new_node(vector<Node>& nodes, double xpos, double ypos, double mass) {
  Node new_node = {xpos, ypos, mass, {{-1, -1}, {-1, -1}}};
  nodes.push_back(new_node);
  return (int) nodes.size() - 1;
}

void insert_particle(vector<Node>& nodes, double* particle) {
  if (nodes.empty()) {
    insert_new_node(nodes, particle[0], particle[1], particle[2]);
    return;
  }
  double lower_xpos = 0, upper_xpos = 4;
  double lower_ypos = 0, upper_ypos = 4;
  int node_idx = 0;
  bool inserted = false;
  while (!inserted) {
    double mid_ypos = (lower_ypos + upper_ypos) / 2;
    double mid_xpos = (lower_xpos + upper_xpos) / 2;
    if (external(nodes, node_idx)) {
      int quad_row = (int) (nodes[node_idx].com_ypos < mid_ypos);
      int quad_col = (int) (nodes[node_idx].com_xpos >= mid_xpos);
      nodes[node_idx].node_idxs[quad_row][quad_col] = 
        insert_new_node(nodes, nodes[node_idx].com_xpos, 
                        nodes[node_idx].com_ypos, nodes[node_idx].mass);
    }
    int quad_row;
    if (particle[1] < mid_ypos) {
      quad_row = 1;
      upper_ypos = mid_ypos;
    } else {
      lower_ypos = mid_ypos;
      quad_row = 0;
    }
    int quad_col;
    if (particle[0] >= mid_xpos) {
      quad_col = 1;
      lower_xpos = mid_xpos;
    } else {
      quad_col = 0;
      upper_xpos = mid_xpos;
    }
    if (nodes[node_idx].node_idxs[quad_row][quad_col] == -1) {
      nodes[node_idx].node_idxs[quad_row][quad_col] = insert_new_node(nodes, particle[0], 
                                                                      particle[1], particle[2]);
      inserted = true;
    }
    auto old_mass = nodes[node_idx].mass; 
    nodes[node_idx].mass += particle[2];
    nodes[node_idx].com_xpos = (nodes[node_idx].com_xpos * old_mass + particle[0] * particle[2]) / 
                                nodes[node_idx].mass;
    nodes[node_idx].com_ypos = (nodes[node_idx].com_ypos * old_mass + particle[1] * particle[2]) / 
                                nodes[node_idx].mass;
    node_idx = nodes[node_idx].node_idxs[quad_row][quad_col];
  }
}

void insert_particles(vector<Node>& nodes, double* particles, int n_particles) {
  for (int i = 0; i < n_particles; i++) {
    double* particle = particles + 5 * i;
    if (particle[2] >= 0) {
      insert_particle(nodes, particle);
    }
  }
}

bool includes(pair<double, double> bl_pos, double width, 
              double* particle) {
  return (particle[0] >= bl_pos.first && particle[0] <= (bl_pos.first + width) &&
          particle[1] >= bl_pos.second && particle[1] <= (bl_pos.second + width));
}

tuple<double, double, double> distance(double* particle, const Node* node) {
  double dx = node->com_xpos - particle[0];
  double dy = node->com_ypos - particle[1];
  double dist = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
  dist = max(dist, 0.03);
  return make_tuple(dist, dx, dy);
}

pair<double, double> force(double mass1, double mass2, 
													 tuple<double, double, double> dist) {
  auto force_by_dist = 0.0001 * mass1 * mass2 / pow(get<0>(dist), 3.0);
  return make_pair(force_by_dist * get<1>(dist), force_by_dist * get<2>(dist));
}

pair<double, double> force_on(double* particle, const vector<Node>& nodes, 
                              const Node* node, pair<double, double> bl_pos, 
                              double width) {
  if (external(node)) {
    if (!includes(bl_pos, width, particle)) {
      return force(particle[2], node->mass, distance(particle, node));
    }
    return make_pair(0.0, 0.0);
  } else {
    auto dist = distance(particle, node);
    if ((width / get<0>(dist)) < theta) {
      return force(particle[2], node->mass, dist);
    } else {
      auto child_width = width / 2;
      pair<double, double> f(0.0, 0.0);
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          if (node->node_idxs[i][j] != -1) {
            pair<double, double> child_bl_pos(bl_pos.first + j * child_width, 
                                              bl_pos.second + (1 - i) * child_width);
            auto result = force_on(particle, nodes, &nodes[node->node_idxs[i][j]], 
                                   child_bl_pos, child_width);
            f.first += result.first;
            f.second += result.second;
          }
        }
      }
      return f;
    }
  }
}

pair<double, double> force_on(double* particle, const vector<Node>& nodes) {
  return force_on(particle, nodes, &nodes[0], make_pair(0.0, 0.0), 4);
}