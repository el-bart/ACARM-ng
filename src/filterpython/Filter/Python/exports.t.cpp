/*
 * exports.t.cpp
 *
 */
#include <tut.h>

#include "PythonAPI/Environment.hpp"

namespace
{
struct TestClass
{
  PythonAPI::Environment env_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Filter/Python/exports");
} // unnamed namespace

namespace tut
{

// tets if namespace is present
template<>
template<>
void testObj::test<1>(void)
{
  env_.importModule("filterapi");
}

} // namespace tut
