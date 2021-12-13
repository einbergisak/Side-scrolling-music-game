#include "waves.h"

unsigned char tone_at_count(unsigned int count, char tone_index) {
  int i;
  switch (tone_index) {
  case 0:
    i = count % G4length;
    return G4[i];
  case 1:
    i = count % A4length;
    return A4[i];
  case 2:
    i = count % B4length;
    return B4[i];
  case 3:
    i = count % C5length;
    return C5[i];
  case 4:
    i = count % D5length;
    return D5[i];
  case 5:
    i = count % E5length;
    return E5[i];
  default:
    return 0;
  }
}
