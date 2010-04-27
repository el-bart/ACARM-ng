/*
 * Equals.t.cpp
 *
 */
#include <tut.h>

#include "Preprocessor/Checkers/Equals.hpp"

using namespace Preprocessor::Checkers;

namespace
{

struct TestClass
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Preprocessor/Checkers/Equals");
} // unnamed namespace


namespace tut
{

// test c-tor/d-tor
template<>
template<>
void testObj::test<1>(void)
{
  Equals ce("abc");
}

// test positive comparison
template<>
template<>
void testObj::test<2>(void)
{
  const Equals ce("abc");
  ensure("comparison failed", ce.check("abc") );
}

// test negative comparison
template<>
template<>
void testObj::test<3>(void)
{
  const Equals ce("abc");
  ensure("comparison didn't failed", ce.check("abcXYZ")==false );
}

} // namespace tut
