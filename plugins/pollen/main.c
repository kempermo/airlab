#include "../al.h"

int main() {
  // clear screen
  al_clear(0);

  // write text
  al_write(AL_W / 2, (AL_H - 16) / 2, 0, 16, 1, "Making Request...", AL_WRITE_ALIGN_CENTER);
  al_yield(1, 0);

  // prepare buffer
  char *res = calloc(1024, 1);

  // make request
  al_http_new();
  al_http_set(ENG_HTTP_URL, 0, "https://f.networkedartifacts.com/call/pollen?cty=pzh", NULL);
  al_http_set(ENG_HTTP_METHOD, 0, "GET", NULL);
  al_http_set(ENG_HTTP_TIMEOUT, 2000, NULL, NULL);
  al_http_run(NULL, 0, res, 1024);

  // get response info
  int status = al_http_get(ENG_HTTP_STATUS);
  int length = al_http_get(ENG_HTTP_LENGTH);
  int errno = al_http_get(ENG_HTTP_ERRNO);
  al_logf("HTTP Status: %d, Length: %d, Errno: %d", status, length, errno);

  // parse result
  float alder, ash, beech, birch, grasses, hazel, oak;
  sscanf(res, "%f,%f,%f,%f,%f,%f,%f", &alder, &ash, &beech, &birch, &grasses, &hazel, &oak);

  // format result
  char result[128];
  sprintf(result, "Alder: %.0f, Ash: %.0f, Beech: %.0f\nBirch: %.0f, Grasses: %.0f\nHazel: %.0f, Oak: %.0f", alder, ash, beech, birch, grasses, hazel, oak);

  // write result
  al_clear(0);
  al_write(AL_W / 2, AL_H / 2 - 18, 0, 16, 1, result, AL_WRITE_ALIGN_CENTER);

  // wait for an event
  al_yield(0, 0);

  return 0;
}
