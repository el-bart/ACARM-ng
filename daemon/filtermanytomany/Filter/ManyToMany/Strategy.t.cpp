/*
 * Strategy.t.cpp
 *
 */
#include <tut.h>
#include <string>
#include <cassert>

#include "Filter/ManyToMany/Strategy.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"
#include "TestHelpers/Persistency/TestHelpers.hpp"

using namespace std;
using namespace Persistency;
using namespace Filter::ManyToMany;
using namespace TestHelpers::Persistency;

namespace
{

struct TestClass: public TestStubs
{
  TestClass(void):
    sampleLeaf_( makeNewLeaf( makeNewAlertWithHosts("1.2.3.4", "2.3.4.5",
                                                    "1.2.3.4", "9.8.7.6") ) ),
    s_(997)
  {
  }

  GraphNodePtrNN         sampleLeaf_;
  Strategy::ChangedNodes changed_;
  Strategy               s_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Filter/ManyToMany/Strategy");
} // unnamed namespace


namespace tut
{

// test if entry is not interesting when no target hosts are set
template<>
template<>
void testObj::test<1>(void)
{
  GraphNodePtrNN sourceOnlyLeaf(
        makeNewLeaf( makeNewAlertWithHosts("1.2.3.4", NULL, NULL, NULL) ) );
  s_.process(sourceOnlyLeaf, changed_);
  ensure_equals("some nodes have been changed / 1", changed_.size(), 0u);

  s_.process(sampleLeaf_, changed_);
  ensure_equals("some nodes have been changed / 2", changed_.size(), 0u);
}

// test if entry is not interesting when no source hosts are set
template<>
template<>
void testObj::test<2>(void)
{
  GraphNodePtrNN targetOnlyLeaf(
      makeNewLeaf( makeNewAlertWithHosts(NULL, NULL, "1.2.3.4", NULL) ) );
  s_.process(targetOnlyLeaf, changed_);
  ensure_equals("some nodes have been changed / 1", changed_.size(), 0u);

  s_.process(sampleLeaf_, changed_);
  ensure_equals("some nodes have been changed / 2", changed_.size(), 0u);
}

// test canCorrelate method
template<>
template<>
void testObj::test<3>(void)
{
  GraphNodePtrNN tmp( makeNewLeaf( makeNewAlertWithHosts("6.6.3.4", "2.3.4.5",
                                                         "6.6.3.4", "9.8.7.6") ) );
  s_.process(tmp, changed_);
  ensure_equals("some nodes have been changed", changed_.size(), 0u);

  s_.process(sampleLeaf_, changed_);
  ensure_equals("no nodes changed", changed_.size(), 1u);
}

// test if can correltate will return false for non-overlaping entries
template<>
template<>
void testObj::test<4>(void)
{
  GraphNodePtrNN tmp( makeNewLeaf( makeNewAlertWithHosts("6.6.3.4", "6.3.4.5",
                                                         "6.6.3.4", "6.8.7.6") ) );
  s_.process(tmp, changed_);
  ensure_equals("some nodes have been changed / 1", changed_.size(), 0u);

  s_.process(sampleLeaf_, changed_);
  ensure_equals("some nodes have been changed / 2", changed_.size(), 0u);
}

// test getting name of meta alert
template<>
template<>
void testObj::test<5>(void)
{
  GraphNodePtrNN tmp( makeNewLeaf( makeNewAlertWithHosts("6.6.3.4", "2.3.4.5",
                                                         "6.6.3.4", "9.8.7.6") ) );
  s_.process(tmp, changed_);
  ensure_equals("some nodes have been changed", changed_.size(), 0u);

  s_.process(sampleLeaf_, changed_);
  ensure_equals("no nodes changed", changed_.size(), 1u);

  const string resp("[many2many] atacks from multiple hosts on "
                    "multiple hosts detected");
  ensure_equals("invalid name",
                changed_[0]->getMetaAlert()->getName().get(), resp);
}

} // namespace tut
