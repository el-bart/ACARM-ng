/*
 * LocalAnalyzersCache.cpp
 *
 */
#include <sstream>
#include <sha.h>
#include <cstring>

#include "Persistency/Facades/detail/LocalAnalyzersCache.hpp"

namespace Persistency
{
namespace Facades
{
namespace detail
{
namespace
{
const char *cstrHash(const char *in)
{
  if(in==NULL)
    return "<NULL>";
  return in;
} // cstrHash()
} // unnamed namespace

LocalAnalyzersCache::Hash LocalAnalyzersCache::makeHash(const Analyzer::Name            &name,
                                                        const Analyzer::Version         &version,
                                                        const Analyzer::OperatingSystem &os,
                                                        const Analyzer::IP              *ip) const
{
  std::stringstream ss;
  ss << "Anlzr:HASH#";
  ss << cstrHash( name.get() );
  ss << "/";
  ss << cstrHash( version.get() );
  ss << "/";
  ss << cstrHash( os.get() );
  ss << "/";
  if(ip==NULL)
    ss << cstrHash(NULL);
  else
    ss << ip->to_string();

  SHA1 sha;
  byte digest[ shad.DigestSize() +1 ];
  memset(digest, 0, sizeof(digest) );
  sha.CalculateDigest( digest, ss.str().c_str(), ss.str().length() );

  assert( strlen(digest)<sizeof(digest) );
  return digest;
}

} // namespace detail
} // namespace Facades
} // namespace Persistency
