/*
 * Host.hpp
 *
 */
#ifndef INCLUDE_PERSISTENCY_IO_STUBS_HOST_HPP_FILE
#define INCLUDE_PERSISTENCY_IO_STUBS_HOST_HPP_FILE

#include "Persistency/IO/Host.hpp"

namespace Persistency
{
namespace IO
{
namespace Stubs
{

/** \brief Host's operations stub
 */
class Host: public IO::Host
{
public:
  /** \brief create hosts's persistency IO.
   *  \param host    host object to handle.
   *  \param t       active transaction.
   *  \param handler persistency handler.
   */
  Host(Persistency::HostPtr  host,
       const Transaction    &t,
       int                   handler);

  /** number of times setting name has been called. */
  size_t setNameCalls_;

private:
  virtual void setNameImpl(const Persistency::Host::Name &name);
}; // class Host

} // namespace Stubs
} // namespace IO
} // namespace Persistency

#endif
