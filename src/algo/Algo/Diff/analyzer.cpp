/*
 * analyzer.cpp
 *
 */
#include "Algo/Diff/analyzer.hpp"
#include "Algo/Diff/all.hpp"

namespace Algo
{
namespace Diff
{

Similarity compare(const Persistency::Analyzer &e1, const Persistency::Analyzer &e2)
{
  const Similarity name=compare( e1.getName(), e2.getName() );
  const Similarity ver =compare( e1.getVersion(), e2.getVersion() );
  const Similarity os  =compare( e1.getOperatingSystem(), e2.getOperatingSystem() );
  const Similarity ip  =compare( e1.getIP(), e2.getIP() );
  const Similarity id  =compare( e1.getID(), e2.getID() );

  return ( name.get()+ver.get()+os.get()+ip.get()+id.get() )/5;
}

} // namespace Diff
} // namespace Algo
