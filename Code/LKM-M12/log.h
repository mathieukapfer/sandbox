#define LOG(level, ...)                                                 \
  {                                                                     \
    char msg[200];                                                      \
    char prefix[100];                                                   \
    snprintf(prefix, sizeof(prefix), "%s:%d:%s:", " " /*__FILE__*/,  __LINE__, " "/*__FUNCTION__*/); \
    snprintf(msg, sizeof(msg), __VA_ARGS__);                            \
    trace_printk( /*level*/ "%s%s\n", prefix, msg);                     \
  }

#define LOG_ENTER  LOG(KERN_INFO, "called");
#define LOG_INFO(...)   LOG(KERN_INFO, __VA_ARGS__)
