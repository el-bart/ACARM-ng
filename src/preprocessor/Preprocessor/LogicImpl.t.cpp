/*
 * LogicImpl.t.cpp
 *
 */
#include <tut.h>

#include "Preprocessor/LogicImpl.hpp"
#include "TestHelpers/Persistency/TestHelpers.hpp"

using namespace ConfigIO::Preprocessor;
using namespace TestHelpers::Persistency;

namespace
{

struct TestClass
{
  TestClass(void):
    alert_( makeNewLeaf() )
  {
  }

  const Persistency::ConstGraphNodePtrNN alert_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Preprocessor/LogicImpl");
} // unnamed namespace


namespace tut
{

// test rejecting with single rule
template<>
template<>
void testObj::test<1>(void)
{
  const Rule                    r=Rule::makeTrue();
  const Expression              e=Expression::makeTerm(r);
  const Section                 s(Section::Type::REJECT, e);
  Config::Sections              cs;
  cs.push_back(s);
  const Config                  cfg(cs);
  const Preprocessor::LogicImpl ps(cfg);
  ensure("rejecting with single rule didn't worked", ps.checkAccept(alert_)==false );
}

// test accepting with single rule
template<>
template<>
void testObj::test<2>(void)
{
  const Rule                    r=Rule::makeTrue();
  const Expression              e=Expression::makeTerm(r);
  const Section                 s(Section::Type::ACCEPT, e);
  Config::Sections              cs;
  cs.push_back(s);
  const Config                  cfg(cs);
  const Preprocessor::LogicImpl ps(cfg);
  ensure("accepting with single rule didn't worked", ps.checkAccept(alert_)==true );
}

// test multiple rules - reject on second
template<>
template<>
void testObj::test<3>(void)
{
  // accept section
  const Rule                    r1=Rule::makeFalse();
  const Expression              e1=Expression::makeTerm(r1);
  const Section                 s1(Section::Type::ACCEPT, e1);

  // reject section
  const Rule                    r2=Rule::makeTrue();
  const Expression              e2=Expression::makeTerm(r2);
  const Section                 s2(Section::Type::REJECT, e2);

  Config::Sections              cs;
  cs.push_back(s1);
  cs.push_back(s2);
  const Config                  cfg(cs);
  const Preprocessor::LogicImpl ps(cfg);
  ensure("rejecting on second rule didn't worked", ps.checkAccept(alert_)==false );
}

// test multiple rules - accept (on second)
template<>
template<>
void testObj::test<4>(void)
{
  // reject section
  const Rule                    r1=Rule::makeFalse();
  const Expression              e1=Expression::makeTerm(r1);
  const Section                 s1(Section::Type::REJECT, e1);

  // accept section
  const Rule                    r2=Rule::makeTrue();
  const Expression              e2=Expression::makeTerm(r2);
  const Section                 s2(Section::Type::ACCEPT, e2);

  Config::Sections              cs;
  cs.push_back(s1);
  cs.push_back(s2);
  const Config                  cfg(cs);
  const Preprocessor::LogicImpl ps(cfg);
  ensure("accepting on second rule didn't worked", ps.checkAccept(alert_)==true );
}

// test empty rule set (accept)
template<>
template<>
void testObj::test<5>(void)
{
  const Config::Sections        cs;
  const Config                  cfg(cs);
  const Preprocessor::LogicImpl ps(cfg);
  ensure("accepting by default didn't worked", ps.checkAccept(alert_)==true );
}

} // namespace tut
