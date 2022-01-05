#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "argparse.h"

using std::cout;
using std::cerr;
using std::endl;

Options get_opts(int argc, char** argv) {
  Options opts;
  
  if (argc == 1) {
    cout << "Usage:" << endl;
    cout << "\t-i <file_path>" << endl;
    cout << "\t-o <file_path>" << endl;
    cout << "\t-s <num_steps>" << endl;
    cout << "\t-t <theta>"     << endl;
    cout << "\t-d <timestep>"  << endl;
    exit(0);
  }
  
  int ch;
  extern char* optarg;
  extern int optopt;
  while ((ch = getopt(argc, argv, "i:o:s:t:d:")) != -1) {
    switch (ch) {
      case 0:
        break;
      case 'i':
        opts.in_file = optarg;
        break;
      case 'o':
        opts.out_file = optarg;
        break;
      case 's':
        opts.steps = atoi(optarg);
        break;
      case 't':
        opts.theta = strtod(optarg, NULL);
        break;
      case 'd':
        opts.dt = strtod(optarg, NULL);
        break;
      case ':':
        cerr << argv[0] << ": option -" << (char)optopt << "requires an argument." << endl;
        exit(1);
    }
  }
  
  return opts;
}