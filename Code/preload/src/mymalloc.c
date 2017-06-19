#include <stdio.h>
#include <stdlib.h>

/* this define is mandatory to use RTLD_NEXT */
#define __USE_GNU
#include <dlfcn.h>

#include "log.h"

static void * (*real_malloc) (size_t size);

/* at init , keep the real malloc fct pointer */
static void __attribute__ (( constructor )) my_init() {
  LOG_ENTRY();
  real_malloc = dlsym(RTLD_NEXT, "malloc");
  LOG_EXIT("real_malloc:%p", real_malloc);

}

/* ok, that the new implementation */
void *malloc(size_t size) {
  void *ret;

  LOG_ENTRY();
  /* call the real malloc */
  ret = real_malloc(size);
  /* adding log */
  LOG_INFO("malloc:%p(%zd)", ret, size);
  /* return real malloc result */
  return ret;
}
