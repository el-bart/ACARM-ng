/*
 * objectID.t.cpp
 *
 */
#include <tut.h>

#include "Algo/Diff/detail/objectID.hpp"

using namespace Algo::Diff;

namespace
{

struct SomeTestClass
{
};
typedef Base::ObjectID<SomeTestClass> ID;
typedef Algo::Diff::detail::Comparer<const ID> Cmp;

struct TestClass
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Algo/Diff/detail/objectID");
} // unnamed namespace


namespace tut
{

// test comparing identical IDs
template<>
template<>
void testObj::test<1>(void)
{
  ensure_equals("identical IDs have non-one similairty", Cmp::cmp( ID(2), ID(2) ).get(), 1);
}

// test comapring different IDs
template<>
template<>
void testObj::test<2>(void)
{
  ensure_equals("different IDs have non-zero similairty", Cmp::cmp( ID(1), ID(2) ).get(), 0);
}

} // namespace tut
