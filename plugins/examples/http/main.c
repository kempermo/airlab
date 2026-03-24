#include "../../al.h"

int main() {
  // clear screen
  al_clear(0);

  // write text
  al_write(AL_W / 2, (AL_H - 16) / 2, 0, 16, 1, "Making Request...", AL_WRITE_ALIGN_CENTER);
  al_yield(1, 0);

  // prepare buffer
  char res[128] = {0};

  // make request
  al_http_new();
  al_http_set(ENG_HTTP_URL, 0, "http://api.ipify.org", NULL);  // TODO: http://wttr.in/?format=%C+%t.
  al_http_set(ENG_HTTP_METHOD, 0, "GET", NULL);
  al_http_set(ENG_HTTP_TIMEOUT, 1000, NULL, NULL);
  al_http_run(NULL, 0, res, sizeof(res) - 1);

  // write result
  al_clear(0);
  al_write(AL_W / 2, 56, 0, 16, 1, res, AL_WRITE_ALIGN_CENTER);

  // wait for an event
  al_yield(0, 0);

  return 0;
}
