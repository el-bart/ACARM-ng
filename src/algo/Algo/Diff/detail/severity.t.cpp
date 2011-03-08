/*
 * severity.t.cpp
 *
 */
#include <tut.h>

#include "Algo/Diff/detail/severity.hpp"

using namespace Persistency;
using namespace Algo::Diff;

namespace
{

typedef Algo::Diff::detail::Comparer<const Persistency::Severity>      Cmp;
typedef Algo::Diff::detail::Comparer<const Persistency::SeverityLevel> CmpLvl;

struct TestClass
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Algo/Diff/detail/severity");
} // unnamed namespace


namespace tut
{

// check if identical severities are the same
template<>
template<>
void testObj::test<1>(void)
{
  const Similarity s=Cmp::cmp( Severity( SeverityLevel(SeverityLevel::MEDIUM) ), Severity( SeverityLevel(SeverityLevel::MEDIUM) ) );
  ensure("identical elements differ", s.get()>0.99);
}

// test slightly different severitys
template<>
template<>
void testObj::test<2>(void)
{
  const Similarity s=Cmp::cmp( Severity( SeverityLevel(SeverityLevel::LOW) ), Severity( SeverityLevel(SeverityLevel::MEDIUM) ) );
  ensure("elements differ too little", s.get()>0.3);
  ensure("elements marked identical", s.get()<1);
}

// test totally different severitys
template<>
template<>
void testObj::test<3>(void)
{
  const Similarity s=Cmp::cmp( Severity( SeverityLevel(SeverityLevel::DEBUG) ), Severity( SeverityLevel(SeverityLevel::HIGH) ) );
  ensure("elements differ too much", s.get()<0.01);
}

// test comparing severity levels
template<>
template<>
void testObj::test<4>(void)
{
  const Similarity s=CmpLvl::cmp( SeverityLevel(SeverityLevel::LOW), SeverityLevel(SeverityLevel::MEDIUM) );
  ensure("elements differ too little", s.get()>0.3);
  ensure("elements marked identical", s.get()<1);
}

} // namespace tut
