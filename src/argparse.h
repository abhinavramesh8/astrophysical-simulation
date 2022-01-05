#ifndef _ARGPARSE_H
#define _ARGPARSE_H

struct Options {
  char* in_file;
  char* out_file;
  int steps;
  double theta;
  double dt;
};

Options get_opts(int, char**);

#endif