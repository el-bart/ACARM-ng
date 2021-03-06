/*
 * compare.hpp
 *
 */
#ifndef INCLUDE_ALGO_DIFF_COMPARE_HPP_FILE
#define INCLUDE_ALGO_DIFF_COMPARE_HPP_FILE

/* public header */

#include "Algo/Diff/detail/all.hpp"

namespace Algo
{
namespace Diff
{

/** \brief compares two elements.
 *  \param e1 first element to compare.
 *  \param e2 second element to compare.
 *  \return result of the comparison.
 */
template<typename T>
Similarity compare(const T &e1, const T &e2)
{
  return detail::compare(e1, e2);
}

} // namespace Diff
} // namespace Algo

#endif
