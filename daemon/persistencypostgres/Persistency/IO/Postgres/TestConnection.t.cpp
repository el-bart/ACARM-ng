/*
 * TestConnection.t.cpp
 *
 */
#include <tut.h>

#include "Persistency/IO/Postgres/TestConnection.t.hpp"

using namespace Persistency::IO::Postgres;

namespace
{

struct TestClass
{
  TestConnection tc_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/IO/Postgres/TestConnection.t");
} // unnamed namespace


namespace tut
{

// test if connection is opened
template<>
template<>
void testObj::test<1>(void)
{
  ensure("not connected", tc_.dbHandler_.getConnection().get().is_open() );
}

// ensure cache is not NULL
template<>
template<>
void testObj::test<2>(void)
{
  ensure("NULL ID cache", tc_.dbHandler_.getIDCache().get()!=NULL );
}

} // namespace tut
