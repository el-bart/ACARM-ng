/*
 * analyzer.t.cpp
 *
 */
#include <tut.h>

#include "System/Math/compareFP.hpp"
#include "Algo/Diff/detail/analyzer.hpp"

#include "TestHelpers/Persistency/TestHelpers.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"

using namespace Algo::Diff;
using namespace ::Persistency;
using namespace TestHelpers::Persistency;

namespace
{

typedef Algo::Diff::detail::Comparer<const Persistency::Analyzer> Cmp;

struct TestClass: private TestHelpers::Persistency::TestStubs
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Algo/Diff/analyzer");
} // unnamed namespace


namespace tut
{

// test comparing identical analyzers
template<>
template<>
void testObj::test<1>(void)
{
  const Similarity s=Cmp::cmp( *makeNewAnalyzer(), *makeNewAnalyzer() );
  ensure("value too small", System::Math::compareFP<double>(s.get(), 1, 3) );
}

// test comapring non-identical analyzers
template<>
template<>
void testObj::test<2>(void)
{
  const Similarity s=Cmp::cmp( *makeNewAnalyzer("abc"), *makeNewAnalyzer("def") );
  ensure("invalid value", System::Math::compareFP<double>(s.get(), 4/5.0, 3) );
}

} // namespace tut
