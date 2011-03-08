/*
 * viaPointerImpl.hpp
 *
 */
#ifndef INCLUDE_ALGO_DIFF_DETAIL_VIAPOINTERIMPL_HPP_FILE
#define INCLUDE_ALGO_DIFF_DETAIL_VIAPOINTERIMPL_HPP_FILE

/* public header */

#include "Algo/Diff/compare.hpp"
#include "Algo/Diff/detail/viaPointer.hpp"

namespace Algo
{
namespace Diff
{
namespace detail
{

namespace detail
{
/** \brief compares two strings - implementation detail.
 *  \param p1 first element to compare.
 *  \param p2 second element to compare.
 *  \return result of the comparison.
 *  \note pointers are assumed to be non-NULLs.
 */
template<typename T>
inline Similarity comparePtrValues(const T *p1, const T *p2)
{
  assert(p1!=NULL);
  assert(p2!=NULL);
  return compare(*p1, *p2);
} // comparePtrValues()

/** \brief compares two strings - implementation detail.
 *  \param p1 first element to compare.
 *  \param p2 second element to compare.
 *  \return result of the comparison.
 *  \note pointers are assumed to be non-NULLs.
 */
inline Similarity comparePtrValues(const char *p1, const char *p2)
{
  assert(p1!=NULL);
  assert(p2!=NULL);
  return compare( std::string(p1), std::string(p2) );
} // comparePtrValues()
} // namesace detail


template<typename T>
inline Similarity Comparer<const T*>::cmp(const T *p1, const T *p2)
{
  if(p1==p2)
    return 1;
  if(p1==NULL || p2==NULL)
    return 0;

  return detail::comparePtrValues(p1, p2);
}


template<typename T>
inline Similarity Comparer<const boost::shared_ptr<T> >::cmp(const boost::shared_ptr<T> &p1, const boost::shared_ptr<T> &p2)
{
  return compare( p1.get(), p2.get() );
}


template<typename T>
inline Similarity Comparer<const Commons::SharedPtrNotNULL<T> >::cmp(const Commons::SharedPtrNotNULL<T> &p1,
                                                                     const Commons::SharedPtrNotNULL<T> &p2)
{
  return compare( p1.get(), p2.get() );
}

} // namespace detail
} // namespace Diff
} // namespace Algo

#endif
