/*
 * exports.t.cpp
 *
 * NOTE: this is NOT test suit but import scheduler for classes used in tests.
 *
 */
#include <tut.h>

#include "PythonAPI/Environment.hpp"
#include "Trigger/Python/exports.hpp"

namespace
{
// it needs to be imported before initializing environment
PythonAPI::Environment::StaticImporter g_import("trigger", inittrigger);
//Trigger::Environment::StaticImporter g_import("trigger", PyInit_trigger);

struct TestClass
{
  PythonAPI::Environment env_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Trigger/Python/exports");
} // unnamed namespace

namespace tut
{

// tets if namespace is present
template<>
template<>
void testObj::test<1>(void)
{
  env_.importModule("trigger");
  //env_.run("from trigger import *");
}

} // namespace tut
