/*
 * Alert.t.cpp
 *
 */

#include <tut.h>

#include "Persistency/IO/Postgres/TestConnection.t.hpp"
#include "Persistency/IO/Postgres/TestDBAccess.t.hpp"
#include "Persistency/IO/Postgres/TestHelpers.t.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Persistency/IO/Postgres/Alert.hpp"

// TODO: tests

// TODO: THIS IS COMMENTED OUT SINCE IMPLEMENTATION OF ANALYZER CHANGED - UPDATE THIS CODE ASAP
using Persistency::IO::Transaction;
using namespace Persistency;
using namespace Persistency::IO::Postgres;
using namespace std;
using boost::posix_time::from_iso_string;
using boost::posix_time::time_from_string;

namespace
{

struct TestClass
{
  TestClass(void):
    name_("some name"),
    //analyzer_( new Analyzer("analyzer name", makeNewHost() ) ),
    //analyzers_( analyzer_ ),
    nullanalyzer_( new Analyzer("analyzer name", NULL, NULL, NULL )),
    nullanalyzers_( nullanalyzer_ ),
    detected_(from_iso_string("2001109T231100")),
    created_(from_iso_string("20011010T231100")),
    severity_(SeverityLevel::INFO),
    certainty_(0.42),
    description_("alert's description"),
    sourceHosts_( generateReportedHosts(2) ),
    targetHosts_( generateReportedHosts(5) ),
    idCache_(new IDCache),
    dbh_(DBHandlerPtrNN(new DBHandler(TestConnection::makeParams(), idCache_) ) ),
    conn_(makeConnection() ),
    t_( conn_->createNewTransaction("save_alert_tests") )
  {
    tdba_.removeAllData();
  }

  IO::ConnectionPtrNN makeConnection(void) const
  {
    IO::BackendFactory::Options opts;
    opts["host"]  ="localhost";
    opts["port"]  ="5432";
    opts["dbname"]="acarm_ng_test";
    opts["user"]  ="acarm-ng-daemon";
    opts["pass"]  ="test.daemon";
    return Persistency::IO::BackendFactory::create("postgres", opts);
  }

  Persistency::Alert::ReportedHosts generateReportedHosts(unsigned int size) const
  {
    Persistency::Alert::ReportedHosts out;
    for(unsigned int i=0; i<size; ++i)
      out.push_back( makeNewHost() );
    return out;
  }

  const Persistency::Alert::Name          name_;
  //const AnalyzerPtrNN                     analyzer_;
  //Persistency::Alert::SourceAnalyzers     analyzers_;
  const AnalyzerPtrNN                     nullanalyzer_;
  Persistency::Alert::SourceAnalyzers     nullanalyzers_;
  const Timestamp                         detected_;
  const Timestamp                         created_;
  const Severity                          severity_;
  const Certainty                         certainty_;
  const std::string                       description_;
  const Persistency::Alert::ReportedHosts sourceHosts_;
  const Persistency::Alert::ReportedHosts targetHosts_;

  TestDBAccess            tdba_;
  IDCachePtrNN            idCache_;
  DBHandlerPtrNN          dbh_;
  IO::ConnectionPtrNN     conn_;
  Transaction             t_;

};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/IO/Postgres/Alert");
} //unnamned namespace

namespace tut
{

// trying save Alert
template<>
template<>
void testObj::test<1>(void)
{
  /*
  Persistency::AlertPtr alertPtr_(new Persistency::Alert(name_, analyzers_, &detected_, created_, severity_,                                                                   certainty_, description_, sourceHosts_, targetHosts_));
  Persistency::IO::Postgres::Alert alert(alertPtr_, t_, dbh_);
  alert.save();
  */
}

//trying save Alert with NULL Host in Analyzer
template<>
template<>
void testObj::test<2>(void)
{
  Persistency::AlertPtr alertPtr_(new Persistency::Alert(name_, nullanalyzers_, &detected_, created_, severity_,                                                                   certainty_, description_, sourceHosts_, targetHosts_));
  Persistency::IO::Postgres::Alert alert(alertPtr_, t_, dbh_);
  alert.save();
}

// trying save Alert with NULL detected time
template<>
template<>
void testObj::test<3>(void)
{
  Persistency::AlertPtr alertPtr_(new Persistency::Alert(name_, nullanalyzers_, NULL, created_, severity_,                                                                   certainty_, description_, sourceHosts_, targetHosts_));
  Persistency::IO::Postgres::Alert alert(alertPtr_, t_, dbh_);
  alert.save();
}

} // namespace tut

