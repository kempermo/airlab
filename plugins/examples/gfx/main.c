#include "../../al.h"

int main() {
  // clear screen
  al_clear(0);

  // write text
  al_write(20, 20, 0, 16, 1, "Left\nLeft 2", 0);
  al_write(AL_W / 2, 20, 0, 16, 1, "Center\nCenter 2", AL_WRITE_ALIGN_CENTER);
  al_write(AL_W - 20, 20, 0, 16, 1, "Right\nRight 2", AL_WRITE_ALIGN_RIGHT);

  // lines
  al_line(20, 64, AL_W - 20, 64, 1, 2);
  al_line(AL_W / 2, 64, AL_W / 2, AL_H - 20, 1, 4);

  // rectangles
  al_rect(20, 80, 50, 30, 1, 0);
  al_rect(AL_W - 20 - 50, 80, 50, 30, 1, 4);

  // wait for an event
  al_yield(0, 0);

  return 0;
}
