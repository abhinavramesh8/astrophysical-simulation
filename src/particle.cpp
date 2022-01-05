#include "particle.h"

bool lost(double* particle) {
  return !(0 <= particle[0] && particle[0] <= 4 && 
           0 <= particle[1] && particle[1] <= 4);
}