/*
 * BoostFSCompat_new.cpp
 *
 * implementation of BoostFSCompat.hpp using new boost versions.
 */
#include "Commons/Filesystem/detail/BoostFSCompat.hpp"

namespace Commons
{
namespace Filesystem
{
namespace detail
{

bool isRegularFile(const boost::filesystem::path &p)
{
  return boost::filesystem::is_regular_file(p);
}

boost::filesystem::path parentPath(const boost::filesystem::path &p)
{
  return p.parent_path();
}

} // namespace detail
} // namespace Filesystem
} // namespace Commons
