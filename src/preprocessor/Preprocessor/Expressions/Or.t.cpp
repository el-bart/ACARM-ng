/*
 * Or.t.cpp
 *
 */
#include <tut.h>

#include "Preprocessor/Expressions/Or.hpp"
#include "Preprocessor/Expressions/TestsCommon.t.hpp"

using namespace Preprocessor::Expressions;

namespace
{
struct TestClass
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Preprocessor/Expressions/Or");
} // unnamed namespace


namespace tut
{

// test positive case
template<>
template<>
void testObj::test<1>(void)
{
  TestExpression *te1=new TestExpression(false);
  BasePtrNN       b1(te1);
  TestExpression *te2=new TestExpression(true);
  BasePtrNN       b2(te2);

  Or::Expressions e;
  e.push_back(b1);
  e.push_back(b2);
  Or o(e);

  ensure_equals("compute()/1 called in c-tor", te1->calls_, 0u);
  ensure_equals("compute()/2 called in c-tor", te1->calls_, 0u);
  ensure("invalid value", o.compute( makeAlert() )==true );
  ensure_equals("compute()/1 not called", te1->calls_, 1u);
  ensure_equals("compute()/2 not called", te2->calls_, 1u);
}

// test negative result
template<>
template<>
void testObj::test<2>(void)
{
  TestExpression *te1=new TestExpression(false);
  BasePtrNN       b1(te1);
  TestExpression *te2=new TestExpression(false);
  BasePtrNN       b2(te2);

  Or::Expressions e;
  e.push_back(b1);
  e.push_back(b2);
  Or o(e);

  ensure_equals("compute()/1 called in c-tor", te1->calls_, 0u);
  ensure_equals("compute()/2 called in c-tor", te1->calls_, 0u);
  ensure("invalid negated", o.compute( makeAlert() )==false );
  ensure_equals("compute()/1 not called", te1->calls_, 1u);
  ensure_equals("compute()/2 not called", te2->calls_, 1u);
}

// test throw on not enought elements
template<>
template<>
void testObj::test<3>(void)
{
  TestExpression  *te=new TestExpression(true);
  BasePtrNN        b(te);
  Or::Expressions  e;
  e.push_back(b);
  try
  {
    Or o(e);    // should throw
    tut::fail("c-tor didn't thorow on not enought elements");
  }
  catch(const Preprocessor::ExceptionInvalidExpression&)
  {
    // this is expected
  }
}

} // namespace tut
