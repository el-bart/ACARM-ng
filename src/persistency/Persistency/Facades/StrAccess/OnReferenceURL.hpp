/*
 * OnReferenceURL.hpp
 *
 */
#ifndef INCLUDE_PERSISTENCY_FACADES_STRACCESS_ONREFERENCEURL_HPP_FILE
#define INCLUDE_PERSISTENCY_FACADES_STRACCESS_ONREFERENCEURL_HPP_FILE

/* public header */

#include "System/NoInstance.hpp"
#include "Persistency/ReferenceURL.hpp"
#include "Persistency/Facades/StrAccess/MainDispatcher.hpp"

namespace Persistency
{
namespace Facades
{
namespace StrAccess
{

struct OnReferenceURL: private System::NoInstance
{
  template<typename TParams>
  static bool process(const ReferenceURL &e, TParams &p)
  {
    typedef typename TParams::template GetHandle<ErrorHandle>::type ErrH;

    ErrH::throwIfLast(SYSTEM_SAVE_LOCATION, p);
    ++p;

    if(p.get()=="name")
      return MainDispatcher::process(e.getName().get(), p);
    if(p.get()=="url")
      return MainDispatcher::process(e.getURL().get(), p);

    ErrH::throwOnInvalidPath(SYSTEM_SAVE_LOCATION, p);
    return false;
  }
}; // struct OnReferenceURL

} // namespace StrAccess
} // namespace Facades
} // namespace Persistency

#endif
