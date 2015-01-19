#ifndef TRACEDEBUG_INCLUDED
#define TRACEDEBUG_INCLUDED

// Sample :
// ========
// test.cpp:8:          [INFO   ][TOTO]      hello info
//
// get from the following code:
// -----------------------------
// #include "TraceDebug.h"
//
// TRACEDEBUG_REGISTER("[TOTO]");
//
// int main(int argc, char **argv)
// {
//   LOG_DEBUG(("hello debug"));
//   LOG_INFO (("hello info"));
// }
// 

// output descriptor and functions that use it
void log (char *format, ...);
void log_(char *str);

// log helper
void LOG_PREFIX(char * LOG_LEVEL, char * file, int line, const char * tracedebug_tag);

#define TRACEDEBUG_REGISTER(tag)                                        \
  static const int  *tracedebug_level = TraceDebug::inst()->getLogLevel(tag); \
  static const char *tracedebug_tag = tag;

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <libgen.h>

// special macro for SMC code
#define TRACE  log

// Log level value
// ================
#define AFF_LOG_LEVEL_DEBUG		(1<<4)
#define AFF_LOG_LEVEL_INFO		(1<<3)
#define AFF_LOG_LEVEL_WARNING	(1<<2)
#define AFF_LOG_LEVEL_ERROR		(1<<1)
#define AFF_LOG_LEVEL_FATAL		(1<<0)

// Tag for each log level
// ======================
#define LOG_LEVEL_DEBUG		"DEBUG  "
#define LOG_LEVEL_INFO		"INFO   " 
#define LOG_LEVEL_WARNING	"WARNING"
#define LOG_LEVEL_ERROR		"ERROR  "
#define LOG_LEVEL_FATAL		"FATAL  "
#define LOG_LEVEL_FORCE		"FORCE  "
#define LOG_LEVEL_CPL_SPY	"CPL_SPY"
#define	LOG_LEVEL_SM		  "SM     "


// User macro for condition
// =========================
#define TEST_LOG_DEBUG   if( tracedebug_level?(AFF_LOG_LEVEL_DEBUG <=   (*tracedebug_level /* & AFF_MASK */)):true)
#define TEST_LOG_INFO    if( tracedebug_level?(AFF_LOG_LEVEL_INFO <=    (*tracedebug_level /* & AFF_MASK */)):true)
#define TEST_LOG_WARNING if( tracedebug_level?(AFF_LOG_LEVEL_WARNING <= (*tracedebug_level /* & AFF_MASK */)):true)
#define TEST_LOG_ERROR   if( tracedebug_level?(AFF_LOG_LEVEL_ERROR <=   (*tracedebug_level /* & AFF_MASK */)):true)
#define TEST_LOG_FATAL   if( tracedebug_level?(AFF_LOG_LEVEL_FATAL <=   (*tracedebug_level /* & AFF_MASK */)):true) 
#define TEST_LOG_FORCE   if( true )

// User macro with prefixed log
// ===========================
#if 1
// NOTE : By remove LOG_DEBUG, you may save 200 kB on binary size (and slightly improve performance)
// #define LOG_DEBUG(LOG_STRING)   {}
#define LOG_DEBUG(LOG_STRING)   {TEST_LOG_DEBUG   {LOG_PREFIX(LOG_LEVEL_DEBUG,__FILE__, __LINE__, tracedebug_tag  );log LOG_STRING;}}
#define LOG_INFO(LOG_STRING)    {TEST_LOG_INFO    {LOG_PREFIX(LOG_LEVEL_INFO,__FILE__, __LINE__ , tracedebug_tag  );log LOG_STRING;}}
#define LOG_WARNING(LOG_STRING) {TEST_LOG_WARNING {LOG_PREFIX(LOG_LEVEL_WARNING,__FILE__, __LINE__, tracedebug_tag);log LOG_STRING;}}
#define LOG_ERROR(LOG_STRING)   {TEST_LOG_ERROR   {LOG_PREFIX(LOG_LEVEL_ERROR,__FILE__, __LINE__, tracedebug_tag  );log LOG_STRING;}}
#define LOG_FATAL(LOG_STRING)   {TEST_LOG_FATAL   {LOG_PREFIX(LOG_LEVEL_FATAL,__FILE__, __LINE__, tracedebug_tag  );log LOG_STRING;}}
// hask use for temporary force one specifique LOG (should not be used definitely)
#define LOG_FORCE(LOG_STRING)                     {LOG_PREFIX(LOG_LEVEL_FORCE,__FILE__, __LINE__, tracedebug_tag  );log LOG_STRING;}
#else
#define LOG_DEBUG(LOG_STRING)   {}
#define LOG_INFO(LOG_STRING)    {}
#define LOG_WARNING(LOG_STRING) {}
#define LOG_ERROR(LOG_STRING)   {}
#define LOG_FATAL(LOG_STRING)   {}
// hask use for temporary force one specifique LOG (should not be used definitely)
#define LOG_FORCE(LOG_STRING)   {}
#endif

// User macro without prefixed log
// ===============================
#define _LOG_DEBUG(LOG_STRING)   {TEST_LOG_DEBUG   log LOG_STRING;}
#define _LOG_INFO(LOG_STRING)    {TEST_LOG_INFO    log LOG_STRING;}
#define _LOG_WARNING(LOG_STRING) {TEST_LOG_WARNING log LOG_STRING;}
#define _LOG_ERROR(LOG_STRING)   {TEST_LOG_ERROR   log LOG_STRING;}
#define _LOG_FATAL(LOG_STRING)   {TEST_LOG_FATAL   log LOG_STRING;}

// User macro for enter and exit function 
// =======================================
#define LOG_ENTER             LOG_DEBUG(("%s called\n",__FUNCTION__));
#define LOG_ENTER_            LOG_INFO( ("%s called\n",__FUNCTION__));
#define LOG_PARAM(param)      LOG_DEBUG(("%s called with ",__FUNCTION__));   TEST_LOG_DEBUG { log param; log("\n");};
#define LOG_PARAM_(param)     LOG_INFO( ("%s called with ",__FUNCTION__));   TEST_LOG_INFO  { log param; log("\n");};
#define LOG_EXIT(param)       LOG_DEBUG(("%s exit with ",__FUNCTION__));    TEST_LOG_DEBUG { log param; log("\n");};
#define LOG_EXIT_(param)      LOG_INFO( ("%s exit with ",__FUNCTION__));    TEST_LOG_INFO  { log param; log("\n");};

// Internal methods
void refreshLogLevelProxy(int signum);

#define LOG_TABLE_NB_ENTRIES 100
#define LOG_TABLE_TAG_LENGTH 50

class TraceDebug {
  
 public:
  
  static TraceDebug *inst()
    {
      if(!_globalObject)
        _globalObject = new(TraceDebug);
      
      return _globalObject;
    }
  
  ~TraceDebug();  

  int *getLogLevel(const char * source);

  int *getGlobalLogLevel(const char * source);
  
  void refreshLogLevel(int signum);
  
  timeval getTimestamp();

 private:

  TraceDebug();  
  
  static TraceDebug *_globalObject;

  static const int _defaultLogLevel = AFF_LOG_LEVEL_INFO;
  
  static int _globalLogLevel;

  bool initialise();
  int *getLogLevel_(const char * source);
  
  typedef struct {
	int indent;
 	char tag[LOG_TABLE_TAG_LENGTH];
	int logLevel;
  } tableLevelItemT;

  timeval startTime;
	
  tableLevelItemT _tableLevel[LOG_TABLE_NB_ENTRIES];
  
  // cnf file related method
  FILE* fd;

  void parseFile();
  void parsePass1();
  void parsePass2();
  int  parseLogLevel(char *tampon);

  void save(char* str, int size);
  
};

#endif
