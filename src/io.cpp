#include <fstream>
#include <iostream>
#include <cstdlib>

#include "io.h"

void read_file(char* in_file, double** particles, int& n_particles) {
  std::ifstream in;
  in.open(in_file);
  
  in >> n_particles;
  *particles = (double*) malloc(5 * n_particles * sizeof(double));
  
  for (int i = 0; i < n_particles; i++) {
    int particle_idx;
    in >> particle_idx;
    
    double* particle = (*particles) + 5 * particle_idx;
    for (int j = 0; j < 5; j++) {
      in >> particle[j];
    }
  }
}

void write_to_file(char* out_file, double* particles, int n_particles) {
  std::ofstream out;
  out.open(out_file, std::ofstream::trunc);
  
  out << std::scientific;
  
  out << n_particles << std::endl;
  
  for (int i = 0; i < n_particles; i++) {
    out << i;
    double* particle = particles + 5 * i;
    for (int j = 0; j < 5; j++) {
      out << "  " << particle[j];
    }
    out << std::endl;
  }
  
  out.flush();
  out.close();
}