/*
 * IDMEFParserCommons.t.cpp
 *
 */
#include <tut.h>
#include <cstring>
#include <memory>
#include <string>

#include <prelude-client.h>
#include <boost/noncopyable.hpp>
#include "Input/Exception.hpp"
#include "Input/Prelude/IDMEFParserCommons.hpp"
#include "Persistency/IPTypes.hpp"
#include "Input/Prelude/TestData.t.hpp"
#include "TestHelpers/Input/TestBase.hpp"

using namespace std;
using namespace Input::Prelude;
using namespace boost::posix_time;
using namespace Persistency;

namespace
{

struct TestClass: public TestHelpers::Input::TestBase
{
  TestData td_;

  TestClass()
  {
    td_.makeAlert();
    td_.addClassificationToAlert("some classification");
    td_.addAnalyzerToAlert();
    source_=td_.addSourceToAlert();
  }

  void addAddressv4(const char * addr)
  {
    assert(source_!=NULL);
    td_.addAddressToSource(source_,addr,false);
  }

  void addAddressv6(const char * addr)
  {
    assert(source_!=NULL);
    td_.addAddressToSource(source_,addr,true);
  }

  idmef_node_t * getNode()
  {
    assert(source_!=NULL);
    return idmef_source_get_node(source_);
  }

protected:
  idmef_source_t * source_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Input/Prelude/IDMEFParserCommons");
}

namespace tut
{

// Check address (IPv4)
template<>
template<>
void testObj::test<1>(void)
{
  addAddressv4("1.2.3.4");
  const boost::asio::ip::address *ip=IDMEFParserCommons::getIPfromIdmefNode(getNode()).get();
  assert(ip!=NULL);
  ensure_equals("Address IPv4",*ip, Analyzer::IP(boost::asio::ip::address_v4::from_string("1.2.3.4")));
}

// Check address (IPv6)
template<>
template<>
void testObj::test<2>(void)
{
  char addrv6[]="2001:0db8:0000:0000:0000:0000:1428:57ab";
  addAddressv6(addrv6);
  const boost::asio::ip::address *ip=IDMEFParserCommons::getIPfromIdmefNode(getNode()).get();
  assert(ip!=NULL);
  ensure_equals("Address IPv6",*ip, Analyzer::IP(boost::asio::ip::address_v6::from_string(addrv6)));
}

} // namespace tut
