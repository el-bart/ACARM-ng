/*
 * Interface.hpp
 *
 */
#ifndef INCLUDE_INPUT_INTERFACE_HPP_FILE
#define INCLUDE_INPUT_INTERFACE_HPP_FILE

/* public header */

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include "Base/Threads/ThreadJoiner.hpp"
#include "Logger/Node.hpp"
#include "Commons/SharedPtrNotNULL.hpp"
#include "Core/Types/AlertsFifo.hpp"
#include "Input/Reader.hpp"

namespace Input
{

/** \brief generic input interface class.
 */
class Interface: private boost::noncopyable
{
public:
  /** \brief create thread running reader in background.
   *  \param reader reader to run in background.
   *  \param output queue to output data to.
   */
  Interface(ReaderPtrNN reader, Core::Types::AlertsFifo &output);
  /** \brief deactivate thread and do the cleanup.
   */
  ~Interface(void);
  /** \brief request stop on thread.
   *
   *  this call exits fast, but thread usually need some time to exit.
   */
  void stop(void);

private:
  Logger::Node                log_;
  Base::Threads::ThreadJoiner thread_;
}; // class Interface


/** \brief interface pointer. */
typedef Commons::SharedPtrNotNULL<Interface> InterfacePtrNN;

} // namespace Input

#endif
