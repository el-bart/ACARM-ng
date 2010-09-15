/*
 * ToXML.t.cpp
 *
 */
#include <tut.h>
#include <string>
#include <sstream>
#include <cstring>
#include <cmath>

#include "RFCIO/ToXML.hpp"
#include "RFCIO/XML/Writer.hpp"
#include "Commons/Convert.hpp"
#include "Persistency/IO/create.hpp"
#include "Persistency/IO/Connection.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"
#include "TestHelpers/Persistency/TestHelpers.hpp"

using namespace std;
using namespace RFCIO;

namespace
{

struct TestClass: public TestHelpers::Persistency::TestStubs
{
  TestClass(void)
  {
    doc_.create_root_node("IDMEF-Message", "http://iana.org/idmef", "idmef");
    rootPtr_=doc_.get_root_node();
    assert(rootPtr_!=NULL);
  }

  void checkClassification(const Persistency::GraphNode &leaf, const char *expectedXML)
  {
    // add as a part of XML
    ToXML toXML(*rootPtr_);
    toXML.addClassification(leaf);
    writeAndCompare(expectedXML);
  }

  void checkAssessment(const Persistency::GraphNode &leaf, const double confidence, const char *severity)
  {
    // add as a part of XML
    ToXML toXML(*rootPtr_);
    toXML.addAssessment(leaf);
    const char *sec1="<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                     "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                       "<idmef:Assessment>"
                         "<idmef:Impact severity=\"";
    const char *sec2=    "\"/>"
                         "<idmef:Confidence rating=\"numeric\">";
    const char *sec3=    "</idmef:Confidence>"
                       "</idmef:Assessment>"
                     "</idmef:IDMEF-Message>\n";
    stringstream ss;
    ss << sec1 << severity << sec2 << confidence << sec3;
    const string expected=ss.str();
    const string actual  =write();
    // it may happen that these strings are identical
    if(expected==actual)
      return;
    // if they are not, it may mean that confidence is not precise (double...)
    if( strstr( actual.c_str(), sec1 )!=actual.c_str() )    // doesn't start with sec1?
      tut::ensure_equals("invalid XML/1", actual, expected);
    // check second part
    stringstream ss2;
    ss2 << sec1 << severity << sec2;
    const char *posSec2=strstr( actual.c_str(), ss2.str().c_str() );
    if(posSec2==NULL)                                       // no match for ending?
      tut::ensure_equals("invalid XML/2", actual, expected);
    // test double value by converting it back to double
    const char   *posSec3=strstr( actual.c_str(), sec3 );
    if(posSec3==NULL)
      tut::ensure_equals("invalid XML/3", actual, expected);
    // conver to double
    const char   *begin=actual.c_str()+ss2.str().length();
    const char   *end  =posSec3;
    const string  confStr(begin, end);
    const double  confVal=Commons::Convert::to<double>(confStr);
    if( fabs(confVal-confidence)>0.01 )
      tut::ensure_equals("invalid XML/4", actual, expected);
    // ok - no problem deteceted
  }

  void check(const Persistency::Analyzer &a, const char *expectedXML)
  {
    // add as a part of XML
    ToXML toXML(*rootPtr_);
    toXML.addAnalyzer(a);
    writeAndCompare(expectedXML);
  }

  void check(const ToXML::IP &ip, const char *expectedXML)
  {
    // add as a part of XML
    ToXML toXML(*rootPtr_);
    toXML.addAddress(ip);
    writeAndCompare(expectedXML);
  }

  Persistency::GraphNodePtrNN certaintyLeaf(const double delta) const
  {
    return certaintySeverityLeaf(delta, 0);
  }
  Persistency::GraphNodePtrNN severityLeaf(const double delta) const
  {
    return certaintySeverityLeaf(0, delta);
  }
  Persistency::GraphNodePtrNN certaintySeverityLeaf(const double certDif, const double sevDif) const
  {
    Persistency::IO::ConnectionPtrNN  conn( Persistency::IO::create().release() );
    Persistency::IO::Transaction      t( conn->createNewTransaction("change_cert_delta") );
    Persistency::GraphNodePtrNN       leaf=TestHelpers::Persistency::makeNewLeaf();
    Persistency::IO::MetaAlertAutoPtr maIO=conn->metaAlert( leaf->getMetaAlert(), t );
    assert( maIO.get()!=NULL );
    maIO->updateCertaintyDelta(certDif);
    maIO->updateSeverityDelta(sevDif);
    t.commit(); // do not log automatic rollback
    return leaf;
  }

  void writeAndCompare(const char *expectedXML)
  {
    assert(expectedXML!=NULL);
    // conver to string
    tut::ensure_equals("invalid XML", write(), expectedXML);
  }
  string write(void)
  {
    RFCIO::XML::Writer w(doc_);
    std::stringstream  ss;
    w.write(ss);
    return ss.str();
  }

  xmlpp::Document  doc_;
  xmlpp::Element  *rootPtr_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("RFCIO/ToXML");
} // unnamed namespace


namespace tut
{

// test basic analyzer's convertion to XML
template<>
template<>
void testObj::test<1>(void)
{
  const Persistency::Analyzer::IP ip=Persistency::Analyzer::IP::from_string("1.2.3.4");
  const Persistency::Analyzer     analyzer(123u, "some name", "v1.2.3", "Linux", &ip);
  check(analyzer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                     "<idmef:Analyzer analyzerid=\"123\" ostype=\"Linux\" version=\"v1.2.3\">"
                       "<idmef:Node category=\"host\">"
                         "<idmef:name>some name</idmef:name>"
                         "<idmef:Address category=\"ipv4-addr\">"
                           "<idmef:address>1.2.3.4</idmef:address>"
                         "</idmef:Address>"
                       "</idmef:Node>"
                     "</idmef:Analyzer>"
                  "</idmef:IDMEF-Message>\n");
}

// test NULL IP in analyzer
template<>
template<>
void testObj::test<2>(void)
{
  const Persistency::Analyzer analyzer(123u, "some name", "v1.2.3", "Linux", NULL);
  check(analyzer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                     "<idmef:Analyzer analyzerid=\"123\" ostype=\"Linux\" version=\"v1.2.3\">"
                       "<idmef:Node category=\"host\">"
                         "<idmef:name>some name</idmef:name>"
                       "</idmef:Node>"
                     "</idmef:Analyzer>"
                  "</idmef:IDMEF-Message>\n");
}

// test NULL Version in analyzer
template<>
template<>
void testObj::test<3>(void)
{
  const Persistency::Analyzer::IP ip=Persistency::Analyzer::IP::from_string("1.2.3.4");
  const Persistency::Analyzer     analyzer(123u, "some name", NULL, "Linux", &ip);
  check(analyzer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                     "<idmef:Analyzer analyzerid=\"123\" ostype=\"Linux\">"
                       "<idmef:Node category=\"host\">"
                         "<idmef:name>some name</idmef:name>"
                         "<idmef:Address category=\"ipv4-addr\">"
                           "<idmef:address>1.2.3.4</idmef:address>"
                         "</idmef:Address>"
                       "</idmef:Node>"
                     "</idmef:Analyzer>"
                  "</idmef:IDMEF-Message>\n");
}

// test NULL OS in analyzer
template<>
template<>
void testObj::test<4>(void)
{
  const Persistency::Analyzer::IP ip=Persistency::Analyzer::IP::from_string("1.2.3.4");
  const Persistency::Analyzer     analyzer(123u, "some name", "v1.2.3", NULL, &ip);
  check(analyzer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                     "<idmef:Analyzer analyzerid=\"123\" version=\"v1.2.3\">"
                       "<idmef:Node category=\"host\">"
                         "<idmef:name>some name</idmef:name>"
                         "<idmef:Address category=\"ipv4-addr\">"
                           "<idmef:address>1.2.3.4</idmef:address>"
                         "</idmef:Address>"
                       "</idmef:Node>"
                     "</idmef:Analyzer>"
                  "</idmef:IDMEF-Message>\n");
}

// test analyzer with IPv6
template<>
template<>
void testObj::test<5>(void)
{
  const Persistency::Analyzer::IP ip=Persistency::Analyzer::IP::from_string("::1");
  const Persistency::Analyzer     analyzer(123u, "some name", "v1.2.3", "Linux", &ip);
  check(analyzer, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                     "<idmef:Analyzer analyzerid=\"123\" ostype=\"Linux\" version=\"v1.2.3\">"
                       "<idmef:Node category=\"host\">"
                         "<idmef:name>some name</idmef:name>"
                         "<idmef:Address category=\"ipv6-addr\">"
                           "<idmef:address>::1</idmef:address>"
                         "</idmef:Address>"
                       "</idmef:Node>"
                     "</idmef:Analyzer>"
                  "</idmef:IDMEF-Message>\n");
}

// test adding creation time
template<>
template<>
void testObj::test<6>(void)
{
  // Fri Feb 13 23:31:30 UTC 2009
  const Persistency::Timestamp ts(1234567890u);
  ToXML toXML(*rootPtr_);
  toXML.addCreateTime(ts);
  writeAndCompare("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                    "<idmef:CreateTime ntpstamp=\"0xCD408152.0x00000000\">2009-02-13T23:31:30Z</idmef:CreateTime>"
                  "</idmef:IDMEF-Message>\n");
}

// test adding IPv4
template<>
template<>
void testObj::test<7>(void)
{
  const ToXML::IP ip=ToXML::IP::from_string("1.2.3.4");
  check(ip, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
              "<idmef:Address category=\"ipv4-addr\">"
                "<idmef:address>1.2.3.4</idmef:address>"
              "</idmef:Address>"
            "</idmef:IDMEF-Message>\n");
}

// test adding IPv6
template<>
template<>
void testObj::test<8>(void)
{
  const ToXML::IP ip=ToXML::IP::from_string("::1");
  check(ip, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
              "<idmef:Address category=\"ipv6-addr\">"
                "<idmef:address>::1</idmef:address>"
              "</idmef:Address>"
            "</idmef:IDMEF-Message>\n");
}

// test adding detection tine
template<>
template<>
void testObj::test<9>(void)
{
  // Fri Feb 13 23:31:30 UTC 2009
  const Persistency::Timestamp ts(1234567890u);
  ToXML toXML(*rootPtr_);
  toXML.addDetectTime(ts);
  writeAndCompare("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                    "<idmef:DetectTime ntpstamp=\"0xCD408152.0x00000000\">2009-02-13T23:31:30Z</idmef:DetectTime>"
                  "</idmef:IDMEF-Message>\n");
}

// test certainty addition
template<>
template<>
void testObj::test<10>(void)
{
  const Persistency::GraphNodePtrNN leaf=certaintyLeaf(0.2);
  checkAssessment(*leaf, 0.42+0.2, "low");
}

// test certainty<0
template<>
template<>
void testObj::test<11>(void)
{
  const Persistency::GraphNodePtrNN leaf=certaintyLeaf(-0.9);
  checkAssessment(*leaf, 0, "low");
}

// test certainty>1
template<>
template<>
void testObj::test<12>(void)
{
  const Persistency::GraphNodePtrNN leaf=certaintyLeaf(0.9);
  checkAssessment(*leaf, 1, "low");
}

// test some random severity
template<>
template<>
void testObj::test<13>(void)
{
  const Persistency::GraphNodePtrNN leaf=severityLeaf(2);
  checkAssessment(*leaf, 0.42, "medium");
}

// test severity<0
template<>
template<>
void testObj::test<14>(void)
{
  const Persistency::GraphNodePtrNN leaf=severityLeaf(-9);
  checkAssessment(*leaf, 0.42, "info");
}

// test deverity>1
template<>
template<>
void testObj::test<15>(void)
{
  const Persistency::GraphNodePtrNN leaf=severityLeaf(9);
  checkAssessment(*leaf, 0.42, "high");
}

// test adding reference URL
template<>
template<>
void testObj::test<16>(void)
{
  const Persistency::ReferenceURLPtr ref=TestHelpers::Persistency::makeNewReferenceURL();
  assert( ref.get()!=NULL );
  ToXML toXML(*rootPtr_);
  toXML.addReference(*ref);
  writeAndCompare("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                  "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                    "<idmef:Reference origin=\"unknown\">"
                      "<idmef:name>some name</idmef:name>"
                      "<idmef:url>http://gnu.org</idmef:url>"
                    "</idmef:Reference>"
                  "</idmef:IDMEF-Message>\n");
}

// test leaf with reference URL set
template<>
template<>
void testObj::test<17>(void)
{
  const Persistency::NodeChildrenVector ncv( severityLeaf(0), severityLeaf(1) );
  const Persistency::MetaAlertPtrNN     ma=TestHelpers::Persistency::makeNewMetaAlert();
  Persistency::IO::ConnectionPtrNN      conn( Persistency::IO::create() );
  Persistency::IO::Transaction          t( conn->createNewTransaction("leaf_with_ref_url") );
  const Persistency::GraphNodePtrNN     node( new Persistency::GraphNode(ma, conn, t, ncv) );
  t.commit();
  checkClassification(*node, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                             "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                               "<idmef:Classification text=\"some meta-alert\">"
                                 "<idmef:Reference origin=\"unknown\">"
                                    "<idmef:name>some name</idmef:name>"
                                    "<idmef:url>http://gnu.org</idmef:url>"
                                 "</idmef:Reference>"
                               "</idmef:Classification>"
                             "</idmef:IDMEF-Message>\n");
}

// test leaf without reference URL
template<>
template<>
void testObj::test<18>(void)
{
  const Persistency::GraphNodePtrNN leaf=TestHelpers::Persistency::makeNewLeaf();
  checkClassification(*leaf, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                             "<idmef:IDMEF-Message xmlns:idmef=\"http://iana.org/idmef\">"
                               "<idmef:Classification text=\"some alert\"/>"
                             "</idmef:IDMEF-Message>\n");
}

// TODO
template<>
template<>
void testObj::test<19>(void)
{
}

// TODO
template<>
template<>
void testObj::test<20>(void)
{
}

// TODO
template<>
template<>
void testObj::test<21>(void)
{
}

// TODO
template<>
template<>
void testObj::test<22>(void)
{
}

// TODO
template<>
template<>
void testObj::test<23>(void)
{
}

// TODO
template<>
template<>
void testObj::test<24>(void)
{
}

// TODO
template<>
template<>
void testObj::test<25>(void)
{
}

// TODO
template<>
template<>
void testObj::test<26>(void)
{
}

// TODO
template<>
template<>
void testObj::test<27>(void)
{
}

} // namespace tut
