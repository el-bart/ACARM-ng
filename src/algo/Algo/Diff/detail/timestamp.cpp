/*
 * timestamp.cpp
 *
 */
#include <cmath>

#include "Algo/Diff/detail/timestamp.hpp"

namespace Algo
{
namespace Diff
{
namespace detail
{

Similarity compare(const Persistency::Timestamp e1, const Persistency::Timestamp e2)
{
  const double diff=std::abs( e1.get()-e2.get() );
  return 1/(1+diff/100);
} // compare()

} // namespace detail
} // namespace Diff
} // namespace Algo
