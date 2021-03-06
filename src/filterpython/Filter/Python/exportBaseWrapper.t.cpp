/*
 * exportBaseWrapper.t.cpp
 *
 */
#include <tut.h>
#include <boost/shared_ptr.hpp>

#include "PythonAPI/Python.hpp"
#include "PythonAPI/Environment.hpp"
#include "Filter/Python/BaseWrapper.hpp"

using namespace Filter::Python;


namespace
{
struct TestClass
{
  TestClass(void)
  {
    env_.importModule("filterapi");
  }

  PythonAPI::Environment env_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Filter/Python/exportBaseWrapper");
} // unnamed namespace

namespace tut
{

// smoke test
template<>
template<>
void testObj::test<1>(void)
{
  env_.run("from filterapi import BaseWrapper");
}

// test making an instance
template<>
template<>
void testObj::test<2>(void)
{
  env_.run("tmp=BaseWrapper()");
  BasePtr ptr=env_.var<BasePtr>("tmp");
  ensure("pointer is NULL", ptr.get()!=NULL);
}

} // namespace tut
