#include <stdio.h>
#include <stdarg.h>

// Comment this line to disable log
#define ENABLE_LOG

#ifdef ENABLE_LOG
#define LOG(...)                                                        \
  {                                                                     \
    char msg[200];                                                      \
    char prefix[100];                                                   \
    snprintf(prefix, sizeof(prefix), "%s:%d:%s:", __FILE__,  __LINE__, __FUNCTION__); \
    snprintf(msg, sizeof(msg), __VA_ARGS__);                            \
    printf("%s%s\n", prefix, msg);                                      \
  }
#define LOG_INFO(...)  LOG("INFO: " __VA_ARGS__)
#define LOG_ENTRY(...)  LOG("ENTRY: " __VA_ARGS__)
#define LOG_EXIT(...)  LOG("EXIT: " __VA_ARGS__)
#else
#define LOG(...)
#define LOG_ENTRY(...)
#define LOG_EXIT(...)
#endif
