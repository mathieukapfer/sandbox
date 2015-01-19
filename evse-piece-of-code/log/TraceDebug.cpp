#include <signal.h>
#include <stdarg.h> /* pour va_list */
#include <unistd.h> /* pour write */
#include <time.h> 
#include "TraceDebug.h"
#include <string.h>

#ifdef ENABLE_LOG_SERVER
#include "HostConnection.h"
#endif

#define LOG_CNF_FILE    "log.cnf"


// String format
// ==============
#define USER_MSG_SIZE  100
#define PREFIX_SIZE     70
#define FILE_NAME_AND_LINE_SIZE 40


void LOG_PREFIX(char * LOG_LEVEL, char * file, int line, const char * tracedebug_tag)                       
{                                                     
	char prefix[PREFIX_SIZE];                             
	char file_name_and_line[FILE_NAME_AND_LINE_SIZE];	    

  timeval ts = TraceDebug::inst()->getTimestamp();

  // format prefix
  snprintf(prefix,PREFIX_SIZE,"%-28s%06u.%06us: [%-7s]%-20s",					
           (snprintf(file_name_and_line,FILE_NAME_AND_LINE_SIZE,"%s:%d:",basename(file),line), file_name_and_line), 
           (uint32_t)ts.tv_sec,            
           (uint32_t)ts.tv_usec,           
           (char *)LOG_LEVEL,
           tracedebug_tag);

  // log prefix
  log(prefix);

}


void log(char *format, ...)
{ 
  va_list ap;
  char str[255];
    
  va_start(ap,format);

  // format string 
  vsnprintf(str, 255, format, ap);
  // write it to log output
  log_(str);

	va_end(ap);
}


void log_(char * str)
{
#ifdef ENABLE_LOG_SERVER
  static HostConnection hostConnection;

  if(hostConnection.IsConnected()) 
    {
      // send to host
      hostConnection.WriteStr(str);
    }
  else
#endif
    {
      // write to standard output
      write(1, str, strlen(str));      
    } 
}



TraceDebug *TraceDebug::_globalObject = 0;

int TraceDebug::_globalLogLevel = TraceDebug::_defaultLogLevel;

TraceDebug::TraceDebug()
{  
  memset(_tableLevel,0, LOG_TABLE_NB_ENTRIES  * sizeof(tableLevelItemT));
  initialise();
};

TraceDebug::~TraceDebug()
{
  log("TraceDebug Descructor called\n");
}


int *TraceDebug::getLogLevel_(const char * source)
{
  int tableIndex = 0;
  int *ret = &_globalLogLevel;
  
  while(_tableLevel[tableIndex].tag[0]) {
    // printf("test :%s,%s\n",source,_tableLevel[tableIndex].tag);
    if (strcmp(_tableLevel[tableIndex].tag,source) == 0) {
      // printf("match:%s,%s\n",source,_tableLevel[tableIndex].tag);
      ret = &(_tableLevel[tableIndex].logLevel);
      break;
    } else {
      tableIndex++;       
    }
  }

  return ret;
}

int *TraceDebug::getLogLevel(const char * source)
{
  int *ret = &_globalLogLevel;
 
  // try full label
  ret = getLogLevel_(source);

  if (&_globalLogLevel == ret ) {
  
    // try level 1 label
   	char parent[LOG_TABLE_TAG_LENGTH];
    char * ptrLevel2;
  
    strcpy(parent,source);
    ptrLevel2 = strstr((&parent[1]), "[");

    if(ptrLevel2) {
      *ptrLevel2 = '\0';
      ret = getLogLevel_(parent);
      if (&_globalLogLevel != ret ) {
      printf("<!-- LOG : Unknow tag %s : take parent one %s -->\n", source, parent);
      } 
    }
  
  }
 
  if (&_globalLogLevel == ret ) {
    printf("<!-- LOG : Unknow tag %s : take global one -->\n",source);
  }

  return ret;
}


int *TraceDebug::getGlobalLogLevel(const char * source)
{ 
  return &_globalLogLevel;
}


bool TraceDebug::initialise()
{
    
#if 0
  // intialize start time
  ;  gettimeofday(&startTime, 0);
#else
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  startTime.tv_sec = ts.tv_sec;
  startTime.tv_usec = ts.tv_nsec / 1000;
#endif            
  // register signal handle
  signal (SIGUSR1, refreshLogLevelProxy) ;

  // parse cnf file
  parseFile();
  
  return true;
}

void TraceDebug::refreshLogLevel(int signum)
{
  printf("TRACEDEBUG INFO   : TraceDebug::Refresh called (%d)\n",signum);

  // reset table level
  memset(_tableLevel,0, LOG_TABLE_NB_ENTRIES * sizeof(tableLevelItemT));

  parseFile();

}

void refreshLogLevelProxy(int signum)
{
  printf("TRACEDEBUG INFO   : Refresh called (%d)\n",signum);
  
  TraceDebug::inst()->refreshLogLevel(signum);
}


// Parse file related fct
// (get from previous project Linky-ERDF)


void TraceDebug::parseFile()
{
  
  int res;

  // open file
  fd = fopen(LOG_CNF_FILE,"r");
  if( fd == NULL)
    {
      printf("TRACEDEBUG ERROR   : Erreur ouverture fichier de conf %s\n", LOG_CNF_FILE);
      return;
    }
  
  // pass 1 : read each line
  parsePass1();
  // pass 2 : compute level (inherate upper log level from parent)
  parsePass2();

  // close file
  res = fclose(fd);    
  if( res < 0)
    {
      printf("TRACEDEBUG ERROR : Erreur fermeture fichier de conf %d\n",res);
    }

}


// TraceDebug::parseLines()
// ==========================
// File : log.cnf =
// -------
// [GLOBAL]  LOG_LEVEL_ERROR
// [MODULE1]
//     [SUBMODULE1] LOG_LEVEL_INFO
//     [SUBMODULE2] LOG_LEVEL_INFO
//
// provide table: _tableLevel =
// --------------
// {{indent = 0, tag = "[GLOBAL", '\000' <repeats 92 times>, logLevel = 2}
;//  {indent = 0, tag = "[MODULE1", '\000' <repeats 91 times>,    logLevel = -1}, 
//  {indent = 1, tag = "[SUBMODULE1", '\000' <repeats 88 times>, logLevel = 8}, 
//  {indent = 0, tag = "[MODULE2", '\000' <repeats 91 times>, logLevel = -1}, 
//  {indent = 1, tag = "[SUBMODULE2", '\000' <repeats 88 times>, logLevel = 8}, 
//  { indent = 0, tag = '\000' <repeats 99 times>, logLevel = 0} <repeats 90 times>}

void TraceDebug::parsePass1()
{
  char buf[200];
  char *str = buf;
  int tabIndex = 0;
  int inputFileLine = 0;
  
  while(fgets (str, sizeof(buf), fd) != NULL)
    if (tabIndex < LOG_TABLE_NB_ENTRIES) 
      {    
        // get nb tabs
        int pos = 0;
        while( (str[pos] == '\t' )&& (str[pos] != '\0') ) pos++;            
        str += pos; // go forward  

        // file pointer (for debug purpose)
        inputFileLine++;

        // get tags
        if (str[0] == '[')
          {
            char * token;
            token = strtok(str, "]");
        
            if(token && token[0] == '[' )
              {
                _tableLevel[tabIndex].indent = pos;
                strncpy(_tableLevel[tabIndex].tag,token,LOG_TABLE_TAG_LENGTH);
            
                token = strtok(NULL, "]"); // go forward
              }
        
            // get level
            _tableLevel[tabIndex].logLevel = parseLogLevel(token);          
            
            tabIndex++;
          
          }

      }
    else 
      {
        printf("<!--Too many entries in file %s, ligne %d ignored:'%s')--> \n", LOG_CNF_FILE, __LINE__, str);       
      }
  
  
}

int TraceDebug::parseLogLevel(char *str)    //Recherche du niveau de log a attribuer
{
  int log_lvl=0;
  int ret = 0;
  
  //LOG_LEVEL_INFO,LOG_LEVEL_DEBUG,LOG_LEVEL_WARNING,LOG_LEVEL_ERROR,LOG_LEVEL_FATAL,LOG_LEVEL_CPL_SPY,LOG_LEVEL_SM,NO_LEVEL,LOG_TRACES_APPLI
  if (!str) {
    ret = -1;
  } else if(strstr (str, "NO_LEVEL")) {
    ret = 0;
  } else {
    
    if(strstr (str, "LOG_LEVEL_INFO"))       log_lvl += AFF_LOG_LEVEL_INFO;
    if(strstr (str, "LOG_LEVEL_DEBUG"))      log_lvl += AFF_LOG_LEVEL_DEBUG;
    if(strstr (str, "LOG_LEVEL_WARNING"))    log_lvl += AFF_LOG_LEVEL_WARNING;
    if(strstr (str, "LOG_LEVEL_ERROR"))      log_lvl += AFF_LOG_LEVEL_ERROR;
    if(strstr (str, "LOG_LEVEL_FATAL"))      log_lvl += AFF_LOG_LEVEL_FATAL;
    
    if(log_lvl==0) {   
      ret = -1;
    } else {
      ret = log_lvl;
    }
      
  }
  //printf("\ncheck_lvl check: %d\n",log_lvl);

  return ret;
}

// pass 2 : 
// - compute log level (inherate upper log level from parent)
// - concat tags
#define MAX_INDENT_VALUE 10

void TraceDebug::parsePass2()
{
  int tableIndex = 0; 
  int logLevelTable[MAX_INDENT_VALUE];
  char* tags[MAX_INDENT_VALUE];
  
  memset(logLevelTable,0,sizeof(int)*MAX_INDENT_VALUE );
  memset(tags,0,sizeof(char *)*MAX_INDENT_VALUE);

  // first : get global log level
  while((tableIndex < LOG_TABLE_NB_ENTRIES) && _tableLevel[tableIndex].tag[0])
    if( (_tableLevel[tableIndex].indent == 0) && 
        (strcmp(_tableLevel[tableIndex].tag,"[GLOBAL") == 0)
        )      
      {
        _globalLogLevel = _tableLevel[tableIndex].logLevel;
        break;
      }
    else
      tableIndex++;        

  tableIndex = 0; 
 
  // second : inherite log level
  while((tableIndex < LOG_TABLE_NB_ENTRIES) &&_tableLevel[tableIndex].tag[0]) // tag name in not null
    {
      int indent = _tableLevel[tableIndex].indent      ;
      
      // build logLevel table
      if ( indent < MAX_INDENT_VALUE)
        {
          // save log level
          logLevelTable[indent] = _tableLevel[tableIndex].logLevel;
          
          // close the missing braket of tag
          strncat(_tableLevel[tableIndex].tag,"]",LOG_TABLE_TAG_LENGTH);
          // save tag addr
          tags[indent]= _tableLevel[tableIndex].tag;
        }
      
      // inherite the upper logLevel
      while(indent>0)
        {
          // inherite log level from parent
          // if(logLevelTable[indent-1] >= _tableLevel[tableIndex].logLevel)
          if(_tableLevel[tableIndex].logLevel ==  -1)
            _tableLevel[tableIndex].logLevel = logLevelTable[indent-1];
          
          // concat parent tag in reverse order
          {
            // - save it
            char tmp[LOG_TABLE_TAG_LENGTH];
            tmp[0] ='\0';
            strncpy(tmp,_tableLevel[tableIndex].tag,LOG_TABLE_TAG_LENGTH);
            // - erase it
            _tableLevel[tableIndex].tag[0] = '\0';
            // - concat it
            strncat(_tableLevel[tableIndex].tag,tags[indent-1],LOG_TABLE_TAG_LENGTH);
            strncat(_tableLevel[tableIndex].tag,tmp  ,LOG_TABLE_TAG_LENGTH);
          }

          // next 
          indent--;
        }    
 
      // inherite global loglevel
      //if(_globalLogLevel >= _tableLevel[tableIndex].logLevel)
      if(_tableLevel[tableIndex].logLevel == -1)
        _tableLevel[tableIndex].logLevel = _globalLogLevel;
      
      
      tableIndex++;
    }
}

// extract from DelayServer/core_time.h
// inline timeval &normalizedTimeval(timeval &t)
// {
//     while (t.tv_usec > 1000000l) {
//         ++t.tv_sec;
//         t.tv_usec -= 1000000l;
//     }
//     while (t.tv_usec < 0l) {
//         --t.tv_sec;
//         t.tv_usec += 1000000l;
//     }
//     return t;
// }

// inline timeval operator-(const timeval &t1, const timeval &t2)
// {
//     timeval tmp;
//     tmp.tv_sec = t1.tv_sec - (t2.tv_sec - 1);
//     tmp.tv_usec = t1.tv_usec - (t2.tv_usec + 1000000);
//     return normalizedTimeval(tmp);
// }

timeval TraceDebug::getTimestamp()
{
    timeval actualTime;
    timeval res;

#if 0
  gettimeofday(&actualTime, 0);
#else
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  actualTime.tv_sec = ts.tv_sec;
  actualTime.tv_usec = ts.tv_nsec / 1000;
#endif
  
    // return (actualTime - startTime);
    timersub(&actualTime,&startTime,&res);
    return res;
}

// {
//   int tabIndex = 0;
//   char tagConcated[LOG_TABLE_TAG_LENGTH];
  
//   while(_tableLevel[tableIndex].tag)
//     {
//       if (_tableLevel[tableIndex].indent == 0)
//         strncpy(tagConcated,_tableLevel[tableIndex].tag,LOG_TABLE_TAG_LENGTH);
//       else
//         {
          
//         }
//     }
// }

#if 0

#include <sys/stat.h>
#include <fcntl.h>

void TraceDebug::save()
{
  static int fd = 0;
  
  if (!fd)
    {
      open("log.txt", O_RDWR | O_APPEND | O_CREAT);
    }

  
}
#endif

