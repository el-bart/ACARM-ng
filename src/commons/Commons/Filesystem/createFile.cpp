/*
 * createFile.cpp
 *
 */
#include "Base/Filesystem/BoostFS.hpp"
#include "Commons/Filesystem/createFile.hpp"
#include "Commons/Filesystem/isDirectorySane.hpp"

using namespace boost::filesystem;

namespace Commons
{
namespace Filesystem
{

SharedPtrNotNULL<std::fstream> createFile(const Base::Filesystem::Path &p)
{
  // file cannot exist yet
  if( exists(p) )
    throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, p, "exists", "file already exists");

  // directory path must be sane
  const path parent=Base::Filesystem::parentPath(p);
  if( parent.empty()==false )
  {
    if( isDirectorySane(parent)==false )
      throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, p, "isDirectorySane",
                                  "path looks suspicious - refusing to create file");
  }

  // create new file
  SharedPtrNotNULL<std::fstream> out( new std::fstream( p.string().c_str(),
                                                        std::fstream::in | std::fstream::out |
                                                        std::fstream::binary | std::fstream::trunc ) );
  assert( out.get()!=NULL );
  if( out->is_open()==false )
    throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, p, "fstream/open", "unable to create file");
  // return final object
  return out;
}

} // namespace Filesystem
} // namespace Commons
