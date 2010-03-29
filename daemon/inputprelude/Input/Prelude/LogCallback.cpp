/*
 * LogCallback.cpp
 *
 */
#include <prelude.h>
#include <cassert>

#include "System/Threads/SafeInitLocking.hpp"
#include "Base/Threads/Lock.hpp"
#include "Input/Prelude/LogCallback.hpp"

using namespace System::Threads;
using namespace Base::Threads;

// unnamed namespace for implementaiton details
namespace
{
SYSTEM_MAKE_STATIC_SAFEINIT_MUTEX(g_mutex);
const Logger::Node *g_node=NULL;
} // unnamed namespace

extern "C"
{

static void wrp_Input_Prelude_ignore(prelude_log_t /*level*/, const char* /*log*/)
{
  assert( g_node ==NULL && "unpredicted condition found");
  // nothing's done here.
} // wrp_Input_Prelude_ignore()

static void wrp_Input_Prelude_loggerCallback(prelude_log_t level, const char *log)
{
  assert( g_node !=NULL && "looks like callback has not been registered");

  if(log==NULL)     // trust no one!
    log="<NULL>";

  /*
   * this definition is taken from libprelude
   *
   * typedef enum {
   *    PRELUDE_LOG_CRIT  = -1,
   *    PRELUDE_LOG_ERR   =  0,
   *    PRELUDE_LOG_WARN  =  1,
   *    PRELUDE_LOG_INFO  =  2,
   *    PRELUDE_LOG_DEBUG  = 3
   * } prelude_log_t;
   */
  assert( PRELUDE_LOG_DEBUG ==  3 );
  assert( PRELUDE_LOG_INFO  ==  2 );
  assert( PRELUDE_LOG_WARN  ==  1 );
  assert( PRELUDE_LOG_ERR   ==  0 );
  assert( PRELUDE_LOG_CRIT  == -1 );

  SafeInitLock lock(g_mutex);
  switch(level)
  {
    case  3: LOGMSG_DEBUG(*g_node, log); break;
    case  2: LOGMSG_INFO (*g_node, log); break;
    case  1: LOGMSG_WARN (*g_node, log); break;
    case  0: LOGMSG_ERROR(*g_node, log); break;
    case -1: LOGMSG_FATAL(*g_node, log); break;
    default: LOGMSG_ERROR_S(*g_node)<<"unknown log-level "<<level<<"; message is: "<<log; break;
  } // switch(level)
} // wrp_Input_Prelude_loggerCallback()

} // extern "C"

namespace Input
{
namespace Prelude
{

LogCallback::LogCallback(const char *node):
  log_(node)
{
  LOGMSG_INFO(log_, "registering callback to use common logger in system");

  assert( g_node ==NULL && "LogCallback - more instances have been created!");

  g_node =&log_;
  // register callback to prelude
  prelude_log_set_callback( wrp_Input_Prelude_loggerCallback );
  // set other options
  //::Prelude::PreludeLog::SetFlags(0);   // disable loffing to syslog and quiet mode
}

LogCallback::~LogCallback(void)
{
  LOGMSG_WARN(log_, "disconnecting from Prelude++ logger - all logs from "
                    "Prelude++ will be ignored from now on");

  SafeInitLock lock(g_mutex);
  g_node =NULL;

  // after d-tor is called we may no loger provide loggin environment, so
  // we switch logging off to prevent illegal accesses.
  prelude_log_set_callback( wrp_Input_Prelude_ignore );
}

} // namespace Prelude
} // namespace Input
