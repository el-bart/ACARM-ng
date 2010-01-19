/*
 * Processor.t.cpp
 *
 */
#include <tut.h>

#include "Core/Types/Proc/Processor.hpp"
#include "Persistency/Stubs/TestHelpers.hpp"
#include "Core/Types/TestBase.t.hpp"

using namespace Core::Types::Proc;
using namespace Persistency;
using namespace Persistency::Stubs;

namespace
{

struct TestInterface: public Interface
{
  TestInterface(void):
    Interface("testinterface"),
    calls_(0),
    node_( makeNewLeaf() )
  {
  }

  virtual void process(Node node, ChangedNodes &changedNodes)
  {
    ++calls_;

    tut::ensure("invalid node", node.get()==node_.get() );
    tut::ensure_equals("invalid count of elements in changed list",
                       changedNodes.size(), 0);

    // add two nodes to chenged list
    changedNodes.push_back(node);
  }

  int  calls_;
  Node node_;
};


struct TestClass: private TestBase
{
  TestClass(void):
    interface_(new TestInterface)
  {
  }

  Core::Types::NodesFifo      mainQueue_;
  Processor::InterfaceAutoPtr interface_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Core/Types/Proc/Processor");
} // unnamed namespace


namespace tut
{

// test d-tor when nothing has been called (thread should be aborted)
template<>
template<>
void testObj::test<1>(void)
{
  Processor p(mainQueue_, interface_);
  usleep(30*1000);                      // wait a while to ensure thread is running
  // when exiting this should not block
}

// test c-tor throw when NULL interface passed
template<>
template<>
void testObj::test<2>(void)
{
  Processor::InterfaceAutoPtr tmp;
  ensure("pre-condition failed", tmp.get()==NULL );
  try
  {
    Processor p(mainQueue_, tmp);
    fail("processor's c-tor didn't throw on NULL interface");
  }
  catch(const ExceptionInvalidInterface&)
  {
    // this is expected
  }
}

// test whole call sequence
template<>
template<>
void testObj::test<3>(void)
{
  ensure_equals("pre-condition failed", mainQueue_.size(), 0);
  // process data
  TestInterface &ti=dynamic_cast<TestInterface&>(*interface_);
  Processor      p(mainQueue_, interface_);
  p.process(ti.node_);  // this call should add 1 element to 'changed'
                        // elements set, and processor should forward it
                        // to the main queue.
  // wait for the results
  for(int i=0; i<10; ++i)
    if( mainQueue_.size()==1 )
      break;
    else
      usleep(50*1000);
  // check results
  ensure_equals("invalid number of elements", mainQueue_.size(), 1);
  ensure("requested element not found", mainQueue_.pop().get()==ti.node_.get() );
}

// test if multiple calls to process() does not break anything
// (i.e. if 'changed' set is cleared before each call)
template<>
template<>
void testObj::test<4>(void)
{
  ensure_equals("pre-condition failed", mainQueue_.size(), 0);
  // process data
  TestInterface &ti=dynamic_cast<TestInterface&>(*interface_);
  Processor                   p(mainQueue_, interface_);
  p.process(ti.node_);          // this call should add 2 elements to 'changed'
  p.process(ti.node_);          // elements set, and processor should forward it
                                // to the main queue.
  // wait for the results
  for(int i=0; i<10; ++i)
    if( mainQueue_.size()==2 )
      break;
    else
      usleep(50*1000);
  // check results
  ensure_equals("invalid number of elements", mainQueue_.size(), 2);
}

} // namespace tut
