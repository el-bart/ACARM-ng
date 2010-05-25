/*
 * LoggerWrapper.cpp
 *
 */
#include <cstring>
#include <boost/checked_delete.hpp>
#include <libetpan/mailstream.h>
#include <cassert>

#include "System/Threads/SafeInitLocking.hpp"
#include "Logger/Logger.hpp"
#include "Trigger/Mail/LoggerWrapper.hpp"


namespace
{
SYSTEM_MAKE_STATIC_SAFEINIT_MUTEX(g_mutex);
size_t        g_count=0;
Logger::Node *g_log  =NULL;
} // unnamed namespace

extern "C"
{
static void loggerCIgnore(const int /*direction*/, const char * /*str*/, const size_t /*size*/)
{
  // this call does nothing
} // loggerCIgnore()

// direction is 1 for send, 0 for receive, -1 when it does not apply
static void loggerCCall(const int direction, const char *str, const size_t size)
{
  assert( strlen(str)<=size );
  assert(g_log!=NULL && "logger function not unregistred after deinitialization");

  switch(direction)
  {
    case 0:     // server
      LOGMSG_DEBUG_S(*g_log)<<"SERVER: "<<str;
      break;

    case 1:     // client
      LOGMSG_DEBUG_S(*g_log)<<"CLIENT: "<<str;
      break;

    case -1:    // does not apply
    default:    // other, whatever...
      LOGMSG_DEBUG(*g_log, str);
      break;
  } // switch(direction)
} // loggerCCall()
} // extern "C"


namespace Trigger
{
namespace Mail
{

LoggerWrapper::LoggerWrapper(void)
{
  System::Threads::SafeInitLock lock(g_mutex);

  // no object allocated so far?
  if(g_count==0)
  {
    assert(g_log==NULL);
    // initialize new instance
    g_log=new Logger::Node("trigger.mail.loggerwrapper");
    // assign proper variables
    mailstream_debug =1;
    mailstream_logger=loggerCCall;
  }

  // mark new user
  ++g_count;

  // anyway - now pointer cannot be NULL
  assert(g_count>0u);
  assert(g_log!=NULL);
}

LoggerWrapper::~LoggerWrapper(void)
{
  System::Threads::SafeInitLock lock(g_mutex);

  // sanity check
  assert(g_count>0);
  assert(g_log!=NULL);

  // mark user's disappearance
  --g_count;

  // should we deallocate?
  if(g_count==0)
  {
    mailstream_debug =0;
    mailstream_logger=loggerCIgnore;
    boost::checked_delete(g_log);
    g_log=NULL;
  }
}

} // namespace Mail
} // namespace Trigger