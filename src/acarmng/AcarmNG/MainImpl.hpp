/*
 * MainImpl.hpp
 *
 */
#ifndef INCLUDE_ACARMNG_MAINIMPL_HPP_FILE
#define INCLUDE_ACARMNG_MAINIMPL_HPP_FILE

#include <string>
#include <pwd.h>
#include <grp.h>
#include <boost/noncopyable.hpp>

#include "Logger/Node.hpp"
#include "Commons/CmdLineArgs.hpp"
#include "AcarmNG/Exception.hpp"


namespace AcarmNG
{

/** \brief implementation of main system's processing mechanism.
 *
 *  this code should be content of main(). it may throw exceptions however,
 *  when some critical error is spotted, thus it must be wrapper in try-catch
 *  for any exceptions.
 */
class MainImpl: private boost::noncopyable
{
public:
  /** \brief exception thrown when daemonizing process fails.
   */
  struct ExceptionCannotDaemonize: public Exception
  {
    /** \brief create execption message.
     *  \param where location where exception has been risen.
     */
    explicit ExceptionCannotDaemonize(const Location &where);
  };

  /** \brief exception thrown when privileges dropping fails.
   */
  struct ExceptionCannotDropPrivileges: public Exception
  {
    /** \brief create execption message.
     *  \param where location where exception has been risen.
     *  \param type  user/group.
     *  \param from  ID that is currently set.
     *  \param to    ID to which change was supposed to happen.
     */
    ExceptionCannotDropPrivileges(const Location &where, const char *type, const int from, const int to);
  };

  /** \brief create data structures, basing on command line arguments.
   *  \param argc number of arguments passed via command line.
   *  \param argv arguments table.
   */
  MainImpl(int argc, char const * const * argv);

  /** \brief creates all internal structures and runs system.
   *  \return code to be returned from main().
   */
  int run(void);

private:
  void runImpl(void);
  void dropPrivileges(uid_t uid, gid_t gid);
  void runAsDaemon(void);
  void runApp(void);

  const Logger::Node    log_;
  Commons::CmdLineArgs  cla_;
  const char           *appName_;
}; // struct MainImpl

} // namespace AcarmNG

#endif
