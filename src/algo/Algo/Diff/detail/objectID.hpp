/*
 * objectID.hpp
 *
 */
#ifndef INCLUDE_ALGO_DIFF_DETAIL_OBJECTID_HPP_FILE
#define INCLUDE_ALGO_DIFF_DETAIL_OBJECTID_HPP_FILE

/* public header */

#include <string>

#include "Base/ObjectID.hpp"
#include "Algo/Diff/Similarity.hpp"

namespace Algo
{
namespace Diff
{
namespace detail
{

/** \brief compares two elements.
 *  \param e1 first element to compare.
 *  \param e2 second element to compare.
 *  \return result of the comparison.
 */
template<typename T>
Similarity compare(const Base::ObjectID<T> e1, const Base::ObjectID<T> e2)
{
  return ( e1.get()==e2.get() )?1.0:0.0;
}

} // namespace detail
} // namespace Diff
} // namespace Algo

#endif
