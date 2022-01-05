#include <mpi.h>
#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <vector>
#include <iostream>

#include "argparse.h"
#include "io.h"
#include "integrator.h"
#include "tree.h"

double theta;

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  
  double start_time = MPI_Wtime();
  
  int curr_id, n_processes;
  MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &curr_id);
  
  auto opts = get_opts(argc, argv);
  theta = opts.theta;
  
  double *particles = NULL;
  int n_particles;
  if (curr_id == 0) {
    read_file(opts.in_file, &particles, n_particles);
  }
  int n_eles = 5 * n_particles;
  
  Integrator integrator(opts.dt);
  
  if (n_processes == 1) {
    for (int step = 0; step < opts.steps; step++) {
      std::vector<Node> tree;
      insert_particles(tree, particles, n_particles);
      integrator.run_simulation(particles, n_eles, tree);
    }
  } else {
    MPI_Bcast(&n_particles, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int plen = 5 * (n_particles / n_processes);
    int all_len = plen * n_processes;
    int curr_len = (curr_id == 0) ? (plen + n_eles - all_len) : plen;
    double* curr_particles = (double*) malloc(curr_len * sizeof(double));
    MPI_Scatter(particles, plen, MPI_DOUBLE, curr_particles, 
                plen, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (curr_id == 0) {
      std::copy(particles + all_len, particles + n_eles, 
                curr_particles + plen);
    }
    
    
    const int n_members = 4;
    int lengths[n_members] = { 1, 1, 1, 4 };
    MPI_Aint offsets[n_members] = { offsetof(Node, com_xpos), 
                                    offsetof(Node, com_ypos), offsetof(Node, mass),
                                    offsetof(Node, node_idxs) };
    MPI_Datatype types[n_members] = { MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, 
                                      MPI_INT };
    MPI_Datatype node_t;
    MPI_Type_struct(n_members, lengths, offsets, types, &node_t);
    MPI_Type_commit(&node_t);
    
    for (int step = 0; step < opts.steps; step++) {
      std::vector<Node> tree;
      int n_nodes;
      if (curr_id == 0) {
        insert_particles(tree, particles, n_particles);
        n_nodes = (int) tree.size();
      }
      MPI_Bcast(&n_nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
      tree.resize(n_nodes);
      MPI_Bcast(tree.data(), n_nodes, node_t, 0, MPI_COMM_WORLD);
      integrator.run_simulation(curr_particles, curr_len, tree);
      MPI_Gather(curr_particles, plen, MPI_DOUBLE, particles, 
                 plen, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      if (curr_id == 0) {
        std::copy(curr_particles + plen, curr_particles + curr_len, 
                  particles + all_len);
      }
    }
    
    MPI_Type_free(&node_t);
    free(curr_particles);
  }

  if (curr_id == 0) {
    write_to_file(opts.out_file, particles, n_particles);
    double end_time = MPI_Wtime();
    std::cout << (end_time - start_time) << std::endl;
    free(particles);
  }
  MPI_Finalize();
  return 0;
}