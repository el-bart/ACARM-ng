/*
 * Alert.t.cpp
 *
 */
#include <tut.h>
#include <cstring>
#include <memory>

#include "Persistency/Alert.hpp"
#include "Persistency/TestHelpers.t.hpp"
#include "TestHelpers/checkEquality.hpp"

using namespace std;
using namespace Persistency;

namespace
{

struct TestClass: private TestBase
{
  TestClass(void):
    name_("some name"),
    analyzer_( new Analyzer("analyzer name", HostPtr() ) ),
    detected_(),
    created_(),
    severity_(SeverityLevel::INFO),
    certanity_(0.42),
    description_("alert's description"),
    sourceHosts_( generateReportedHosts(2) ),
    targetHosts_( generateReportedHosts(5) ),
    custom_(name_, analyzer_, &detected_, created_, severity_, certanity_,
            description_, sourceHosts_, targetHosts_)
  {
  }

  Alert::ReportedHosts generateReportedHosts(unsigned int size) const
  {
    Alert::ReportedHosts out;
    for(unsigned int i=0; i<size; ++i)
      out.push_back( makeNewHost() );
    return out;
  }

  const Alert::Name          name_;
  const AnalyzerPtr          analyzer_;
  const Timestamp            detected_;
  const Timestamp            created_;
  const Severity             severity_;
  const Certainty            certanity_;
  const string               description_;
  const Alert::ReportedHosts sourceHosts_;
  const Alert::ReportedHosts targetHosts_;
  const Alert                custom_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/Alert");
} // unnamed namespace


namespace tut
{

// test c-tor with all paramters set
template<>
template<>
void testObj::test<1>(void)
{
  const Alert a(name_, analyzer_, &detected_, created_, severity_, certanity_,
                description_, sourceHosts_, targetHosts_);
  // check getters
  ensure_equals("invalid name", a.getName().get(), string( name_.get() ) );
  ensure("invalid analyzer", &a.getAnalyzer()==analyzer_.get() );
  ensure("NULL detect time", a.getDetectionTime()!=NULL);
  ensure_equals("invalid detect time", *a.getDetectionTime(), detected_);
  ensure_equals("invalid created time", a.getCreationTime(), created_);
  ensure_equals("invalid severity",
                a.getSeverity().getLevel().toInt(), severity_.getLevel().toInt() );
  ensure_equals("invalid certanity", a.getCertainty().get(), certanity_.get() );
  ensure_equals("invalid description", a.getDescription(), description_);
  ensure_equals("invalid source hosts",
                a.getReportedSourceHosts().size(), sourceHosts_.size() );
  ensure_equals("invalid destination hosts",
                a.getReportedTargetHosts().size(), targetHosts_.size() );
}

// test c-tor with NULL detection time
template<>
template<>
void testObj::test<2>(void)
{
  const Alert a(name_, analyzer_, NULL, created_, severity_, certanity_,
                description_, sourceHosts_, targetHosts_);
  ensure("detection time is not NULL", a.getDetectionTime()==NULL);
}

// test c-tor for NULL analyzer
template<>
template<>
void testObj::test<3>(void)
{
  AnalyzerPtr tmp;
  assert( tmp.get()==NULL );
  try
  {
    Alert a(name_, tmp, &detected_, created_, severity_, certanity_,
            description_, sourceHosts_, targetHosts_);
    fail("NULL analyzer has been accepted");
  }
  catch(const Commons::ExceptionUnexpectedNULL&)
  {
    // this is expected
  }
}

// test equality with different names
template<>
template<>
void testObj::test<4>(void)
{
  const Alert a("different", analyzer_, &detected_, created_, severity_,
                certanity_, description_, sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different analyzers
template<>
template<>
void testObj::test<5>(void)
{
  AnalyzerPtrNN tmp( new Analyzer("different", HostPtr() ) );
  const Alert   a(name_, tmp, &detected_, created_, severity_,
                  certanity_, description_, sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different detection time
template<>
template<>
void testObj::test<6>(void)
{
  const Alert a(name_, analyzer_, NULL, created_, severity_,
                certanity_, description_, sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different creation time
template<>
template<>
void testObj::test<7>(void)
{
  const Timestamp tmp=boost::posix_time::time_from_string("2010-11-12");
  const Alert     a(name_, analyzer_, &detected_, tmp, severity_,
                    certanity_, description_, sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different severities
template<>
template<>
void testObj::test<8>(void)
{
  const Severity tmp(SeverityLevel::ERROR);
  const Alert    a(name_, analyzer_, &detected_, created_, tmp,
                   certanity_, description_, sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different certainties
template<>
template<>
void testObj::test<9>(void)
{
  const Alert a(name_, analyzer_, &detected_, created_, severity_,
                0.1234, description_, sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different descriptions
template<>
template<>
void testObj::test<10>(void)
{
  const Alert a(name_, analyzer_, &detected_, created_, severity_,
                certanity_, "blah", sourceHosts_, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different target hosts
template<>
template<>
void testObj::test<11>(void)
{
  Alert::ReportedHosts hosts;
  hosts.push_back( makeNewHost() );
  const Alert a(name_, analyzer_, &detected_, created_, severity_,
                certanity_, description_, sourceHosts_, hosts);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different source hosts
template<>
template<>
void testObj::test<12>(void)
{
  Alert::ReportedHosts hosts;
  hosts.push_back( makeNewHost() );
  const Alert a(name_, analyzer_, &detected_, created_, severity_,
                certanity_, description_, hosts, targetHosts_);
  TestHelpers::checkEquality(custom_, a);
}

// test equality with different object representing the same values
template<>
template<>
void testObj::test<13>(void)
{
  const AnalyzerPtr          analyzer1( new Analyzer("analyzer name", HostPtr() ) );
  const Alert::ReportedHosts hosts1( generateReportedHosts(1) );
  const Alert a1(name_, analyzer1, &detected_, created_, severity_,
                 certanity_, description_, hosts1, targetHosts_);

  const AnalyzerPtr          analyzer2( new Analyzer("analyzer name", HostPtr() ) );
  const Alert::ReportedHosts hosts2( generateReportedHosts(1) );
  const Alert a2(name_, analyzer2, &detected_, created_, severity_,
                 certanity_, description_, hosts2, targetHosts_);

  TestHelpers::checkEquality(a1, a2, custom_);
}

} // namespace tut
