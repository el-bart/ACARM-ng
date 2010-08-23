/*
 * Reader.t.cpp
 *
 */
#include <tut.h>

#include "Input/Prelude/Reader.hpp"

using namespace std;
using namespace Input::Prelude;

namespace
{

struct TestClass
{
  TestClass():
    r_("profile", "somename", "/etc/prelude/default/client.conf")
  {
  }

  Reader r_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Input/Prelude/Reader");
} // unnamed namespace


namespace tut
{

// test if type is valid
template<>
template<>
void testObj::test<1>(void)
{
  ensure_equals("invalid type", r_.getType(), "prelude");
}

} // namespace tut
