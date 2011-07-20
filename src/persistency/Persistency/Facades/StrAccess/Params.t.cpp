/*
 * Params.t.cpp
 *
 */
#include <tut.h>
#include <boost/type_traits/is_same.hpp>

#include "System/ignore.hpp"
#include "Persistency/Facades/StrAccess/Params.hpp"
#include "Persistency/Facades/StrAccess/TestCallback.t.hpp"

using namespace std;
using namespace Persistency::Facades::StrAccess;

namespace
{

struct TestClass
{
  TestClass(void):
    path_("one.two"),
    p_(path_, cb_)
  {
  }

  const Path   path_;
  TestCallback cb_;
  Params       p_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/Facades/StrAccess/Params");
} // unnamed namespace


namespace tut
{

// test for end element, when not on the end
template<>
template<>
void testObj::test<1>(void)
{
  ensure("starting with end", !p_.isEnd() );
  ++p_;
  ensure("end after first element", !p_.isEnd() );
}

// test for end element, when on the end
template<>
template<>
void testObj::test<2>(void)
{
  ++p_;
  ++p_;
  ensure("end not found", p_.isEnd() );
}

// test each element's value
template<>
template<>
void testObj::test<3>(void)
{
  ensure_equals("invalid element 1", p_.get(), "one");
  ++p_;
  ensure_equals("invalid element 2", p_.get(), "two");
}

// test for callbacks declaration
template<>
template<>
void testObj::test<4>(void)
{
  ResultCallback &tmp=p_.callback();
  System::ignore(tmp);
}

// test getting full path
template<>
template<>
void testObj::test<5>(void)
{
  ensure_equals("invalid path", p_.path().get(), path_.get());
}

} // namespace tut