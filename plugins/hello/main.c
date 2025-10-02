#include "../al.h"

int main() {
  // clear screen
  al_clear(0);

  // write text
  al_write(AL_W / 2, (AL_H - 16) / 2, 0, 16, 1, "Hello, World!", AL_WRITE_ALIGN_CENTER);

  // wait for an event
  al_yield(0, 0);

  return 0;
}
