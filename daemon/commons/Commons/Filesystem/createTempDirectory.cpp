/*
 * createTempDirectory.cpp
 *
 */
#include "Base/EditableCString.hpp"
#include "Commons/Filesystem/createTempDirectory.hpp"
#include "Commons/Filesystem/isDirectorySane.hpp"

using namespace boost::filesystem;

namespace Commons
{
namespace Filesystem
{

boost::filesystem::path createTempDirectory(const boost::filesystem::path &root)
{
  try
  {
    // sanity check
    if( root.empty() )
      throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, root, "sanity check", "path is not set");
    if( !isDirectorySane(root) )
      throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, root, "sanity check", "path is not sane");

    // preapre emplate
    const path            tmpl=root / ".fs_tmp_dir_XXXXXX";
    Base::EditableCString tmplStr( tmpl.string() );
    // create directory
    if( mkdtemp( tmplStr.get() )==NULL )
      throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, tmpl, "mkdtemp", "unable to create temporary directory");

    // return new directory
    return path( tmplStr.get() );
  }
  catch(const ExceptionFilesystemIO &ex)
  {
    throw ex;
  }
  catch(const std::exception &ex)
  {
    throw ExceptionFilesystemIO(SYSTEM_SAVE_LOCATION, root, "createTempDirectory",
                                "problem while creating temporary directory");
  }
}

} // namespace Filesystem
} // namespace Commons