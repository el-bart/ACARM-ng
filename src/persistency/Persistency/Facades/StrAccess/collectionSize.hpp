/*
 * collectionSize.hpp
 *
 */
#ifndef INCLUDE_PERSISTENCY_FACADES_STRACCESS_COLLECTIONSIZE_HPP_FILE
#define INCLUDE_PERSISTENCY_FACADES_STRACCESS_COLLECTIONSIZE_HPP_FILE

/* public header */

namespace Persistency
{
namespace Facades
{
namespace StrAccess
{

/** \brief helper function counting elements in a given iterators range.
 *  \param begin iterator to start processing with.
 *  \param end   iterator indicating end of elements.
 *  \return count of elements in rage [begin;end).
 */
template<typename CIter>
inline size_t collectionSize(CIter begin, const CIter end)
{
  size_t size=0;
  while(begin!=end)
  {
    ++begin;
    ++size;
  }
  return size;
} // collectionSize()

} // namespace StrAccess
} // namespace Facades
} // namespace Persistency

#endif
