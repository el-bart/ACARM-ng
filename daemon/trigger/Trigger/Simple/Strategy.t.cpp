/*
 * Strategy.t.cpp
 *
 */
#include <tut.h>

#include "Trigger/Simple/Strategy.hpp"
#include "TestHelpers/Persistency/TestHelpers.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"

using namespace Persistency;
using namespace Trigger::Simple;
using namespace TestHelpers::Persistency;

namespace
{

struct TestTrigger: public Strategy
{
  TestTrigger(const char *severity, const char *count):
    Strategy("testtrigger", ThresholdConfig(severity, count) ),
    callsTrigger_(0)
  {
  }

  virtual void triggerImpl(const Persistency::GraphNodePtrNN &/*n*/)
  {
    ++callsTrigger_;
  }

  int callsTrigger_;
};


struct TestClass: private TestHelpers::Persistency::TestStubs
{
  void check(TestTrigger &tt, const Persistency::GraphNodePtrNN n, const size_t cnt)
  {
    tut::ensure_equals("pre-condition failed", tt.callsTrigger_, 0);
    TestTrigger::ChangedNodes cn;
    tt.process(n, cn);
    tut::ensure_equals("ChangedNodes changed in trigger", cn.size(), 0u);
    tut::ensure_equals("trigger called invalid number of times", tt.callsTrigger_, cnt);
  }
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Trigger/Simple/Strategy");
} // unnamed namespace


namespace tut
{

// check if name has been saved correctly
template<>
template<>
void testObj::test<1>(void)
{
  TestTrigger tt(NULL, NULL);
  ensure_equals("invalid name", tt.getTriggerName(), "testtrigger");
}

// test trigger when alerts count is above threshold
template<>
template<>
void testObj::test<2>(void)
{
  TestTrigger tt(NULL, "2");
  check(tt, makeNewTree1(), 1);
}

// test trigger when severity is above threshold
template<>
template<>
void testObj::test<3>(void)
{
  TestTrigger tt("1.0", NULL);
  check(tt, makeNewTree1(), 1);
}

// test trigger when severity equals threshold
template<>
template<>
void testObj::test<4>(void)
{
  TestTrigger tt("1.1", NULL);
  check(tt, makeNewNode(), 1);
}

// test trigger when alerts count equals threshold
template<>
template<>
void testObj::test<5>(void)
{
  TestTrigger tt(NULL, "2");
  check(tt, makeNewNode(), 1);
}

// test if trigger is NOT called, when both values are below threshold
template<>
template<>
void testObj::test<6>(void)
{
  TestTrigger tt("666", "42");
  check(tt, makeNewLeaf(), 0);
}

// test if trigger is NOT called, when both thresholds are not set
template<>
template<>
void testObj::test<7>(void)
{
  TestTrigger tt(NULL, NULL);
  check(tt, makeNewLeaf(), 0);
}

namespace
{
struct TestBufferTrigger: public Strategy
{
  explicit TestBufferTrigger(int skip):
    Strategy("testbuffertrigger", ThresholdConfig("0", "0") ),
    skip_(skip),
    callsTrigger_(0),
    counter_(0)
  {
  }

  virtual void triggerImpl(const Persistency::GraphNodePtrNN &/*n*/)
  {
    ++counter_;
    if(counter_<=skip_)
      throw std::runtime_error("error simulation - skip this run");
    // simulate success
    ++callsTrigger_;
  }

  int skip_;
  int callsTrigger_;
  int counter_;
};
} // unnamed namespace

// test if messages buffering works
template<>
template<>
void testObj::test<8>(void)
{
  TestBufferTrigger tt(1);

  // first run
  {
    TestBufferTrigger::ChangedNodes cn;
    try
    {
      tt.process( makeNewLeaf(), cn );
      // fail not needed here - it doesn't matter it this call throws or not...
    }
    catch(const std::runtime_error &)
    {
      // this is expected
    }
  }
  // test first call
  tut::ensure_equals("trigger not called at all", tt.counter_, 1);
  tut::ensure_equals("trigger called after exception", tt.callsTrigger_, 0);

  // second run
  {
    TestBufferTrigger::ChangedNodes cn;
    tt.process( makeNewLeaf(), cn );
  }
  // test second call
  tut::ensure_equals("trigger not called (2nd run)", tt.counter_, 1+2);
  tut::ensure_equals("not all messages have been sent", tt.callsTrigger_, 0+2);
}

// test if messages are removed from buffer when sent
template<>
template<>
void testObj::test<9>(void)
{
  TestBufferTrigger tt(1);

  // first run
  {
    TestBufferTrigger::ChangedNodes cn;
    try
    {
      tt.process( makeNewLeaf(), cn );
      // fail not needed here - it doesn't matter it this call throws or not...
    }
    catch(const std::runtime_error &)
    {
      // this is expected
    }
  }
  // test first call
  tut::ensure_equals("trigger not called at all", tt.counter_, 1);
  tut::ensure_equals("trigger called after exception", tt.callsTrigger_, 0);

  // second run
  {
    TestBufferTrigger::ChangedNodes cn;
    tt.process( makeNewLeaf(), cn );
  }
  // test second call
  tut::ensure_equals("trigger not called (2nd run)", tt.counter_, 1+2);
  tut::ensure_equals("not all messages have been sent", tt.callsTrigger_, 0+2);

  // third call
  {
    TestBufferTrigger::ChangedNodes cn;
    tt.process( makeNewLeaf(), cn );
  }
  // test third call
  tut::ensure_equals("trigger not called (3rd run)", tt.counter_, 1+2+1);
  tut::ensure_equals("messages are not removed?", tt.callsTrigger_, 0+2+1);
}

} // namespace tut
