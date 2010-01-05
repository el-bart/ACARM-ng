/*
 * forEachInTree.t.cpp
 *
 */
#include <tut.h>

#include "Filter/Algo/forEachInTree.hpp"
#include "Filter/TestHelpers.t.hpp"

using namespace Filter::Algo;
using namespace Persistency;

namespace
{

struct FuncObj
{
  FuncObj(void):
    cnt_(0)
  {
  }
  void operator()(GraphNodePtrNN)
  {
    ++cnt_;
  }
  int cnt_;
};

struct TestClass
{
  TestClass(void):
    root_( th_makeTree1() )
  {
  }

  GraphNodePtrNN root_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Filter/Algo/forEachInTree");
} // unnamed namespace


namespace tut
{

// check trawersing through all elements
template<>
template<>
void testObj::test<1>(void)
{
  FuncObj out=forEachInTree(root_, FuncObj() );
  ensure_equals("invalid number of elements", out.cnt_, 9);
}

// check leaf traversal
template<>
template<>
void testObj::test<2>(void)
{
  FuncObj out=forEachInTree( th_makeLeaf(), FuncObj() );
  ensure_equals("invalid traversal count for leaf", out.cnt_, 1);
}

} // namespace tut
