/*
 * NumericalBase.t.cpp
 *
 */
#include <tut.h>

#include "Preprocessor/Formatters/NumericalBase.hpp"
#include "Preprocessor/Formatters/Value.hpp"

using namespace std;
using namespace Preprocessor::Formatters;

namespace
{

struct TestImpl: public NumericalBase
{
  explicit TestImpl(ValuePtrNN v):
    NumericalBase( "testimpl", Formatters(1, v) )
  {
  }

  virtual double execConverted(const NumericalArguments &args) const
  {
    if( args.size()==0 )
      return 42;
    return args.at(0);
  }
}; // struct TestImpl

struct TestClass
{
  TestClass(void):
    v_(new Value),
    ti_(v_)
  {
  }

  ValuePtrNN v_;
  TestImpl   ti_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Preprocessor/Formatters/TestImplBase");
} // unnamed namespace


namespace tut
{

// test some numeric value
template<>
template<>
void testObj::test<1>(void)
{
  v_->set("123");
  ensure_equals("invalid value returned", ti_.exec(), "123");
}

// test exception propagation when non-number is spotted
template<>
template<>
void testObj::test<3>(void)
{
  v_->set("abc");
  try
  {
    ti_.exec();     // should throw
    fail("exec() didn't throw on non-numerica value");
  }
  catch(const ExceptionNotANumber&)
  {
    // this is expected
  }
}

} // namespace tut
