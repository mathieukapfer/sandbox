#include <stdlib.h>
#include "log.h"

int main(int argc, char *argv[]) {
  LOG_ENTRY();
  void *p;

  LOG_INFO("malloc:%p", malloc);

  p = malloc(10);
  p = malloc(10);
  p = malloc(10);
  p = malloc(10);

  LOG_EXIT();
  return 0;
}
