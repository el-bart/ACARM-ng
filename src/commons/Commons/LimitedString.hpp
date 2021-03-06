/*
 * LimitedString.hpp
 *
 */
#ifndef INCLUDE_COMMONS_LIMITEDSTRING_HPP_FILE
#define INCLUDE_COMMONS_LIMITEDSTRING_HPP_FILE

/* public header */

#include <string>
#include <cstring>
#include <stdint.h>
#include <cassert>
#include <boost/static_assert.hpp>
#include <boost/operators.hpp>

#include "Commons/Exception.hpp"
#include "Commons/ExceptionUnexpectedNULL.hpp"


namespace Commons
{

/** \brief thrown when input string is too long.
 */
class ExceptionStringTooLong: public Exception
{
public:
  /** \brief create execption with given message.
   *  \param where place wxception has been thrown.
   *  \param str   string that has been found too long.
   */
  ExceptionStringTooLong(const Location &where, const char *str):
    Exception(where, std::string("too long string: ") + ensureString(str) )
  {
  }
}; // class ExceptionStringTooLong


/** \brief class holding string of a given maximum length.
 */
template<uint16_t N>
class LimitedString: public boost::equality_comparable< LimitedString<N> >
{
public:
  /** \brief export size to be later referencable. */
  static const uint16_t size=N;

  /** \brief creates empty string.
   */
  LimitedString(void)
  {
    createFromNonNULL("");
  }
  /** \brief creates object from a given string.
   *  \param str string to create object from.
   *  \note this c-tor is not explicit to allow easier argument passing.
   */
  LimitedString(const char *str)
  {
    // sanity check
    if(str==NULL)
      throw ExceptionUnexpectedNULL(SYSTEM_SAVE_LOCATION);
    createFromNonNULL(str);
  }
  /** \brief creates object from a given string.
   *  \param str string to create object from.
   *  \note this c-tor is not explicit to allow easier argument passing.
   */
  LimitedString(const std::string &str)
  {
    createFromNonNULL( str.c_str() );
  }

  /** \brief gets pointer to data.
   *  \return string held inside.
   */
  const char *get(void) const
  {
    assert( strlen(str_)<=N );
    return str_;
  }

  /** \brief gets char from a given position.
   *  \param p position to get char from.
   *  \return char.
   *  \note call does NOT check ranges.
   */
  char operator[](const uint16_t p) const
  {
    assert(p<=N && "index out of bound");
    return str_[p];
  }

  /** \brief check if classes are equal.
   *  \param other element to compare with.
   *  \return true if elements are equal, false otherwise.
   */
  bool operator==(const LimitedString<N> &other) const
  {
    assert(str_!=NULL);
    assert(other.str_!=NULL);
    if(str_==other.str_)    // small optimization
      return true;
    return strcmp(str_, other.str_)==0;
  }

private:
  BOOST_STATIC_ASSERT(N<=512);// string is unreasonably large

  void createFromNonNULL(const char *str)
  {
    assert(str!=NULL);
    // check length
    const size_t len=strlen(str);
    if(len>N)
      throw ExceptionStringTooLong(SYSTEM_SAVE_LOCATION, str);

    // save data
    assert(len+1<=sizeof(str_));
    memcpy(str_, str, len+1);
  }

  char str_[N+1];
}; // class LimitedString

} // namespace Commons

#endif
