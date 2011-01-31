/*
 * ObjectFactoryBuilder.t.cpp
 *
 */
#include <tut.h>
#include <cstring>
#include <memory>

#include "Persistency/IO/Postgres/ConnectionBuilder.hpp"

using namespace std;
using namespace Persistency::IO::Postgres;

namespace
{

struct TestClass
{
  ConnectionBuilder::FactoryPtr build(const char *host  ="localhost",
                                      const char *port  ="5432",
                                      const char *dbname="acarm_ng_test_v0_1_0",
                                      const char *user  ="acarm-ng-daemon",
                                      const char *pass  ="test.daemon") const
  {
    ConnectionBuilder::Options opts;
    if(host!=NULL)
      opts["host"]=host;
    if(port!=NULL)
      opts["port"]=port;
    if(dbname!=NULL)
      opts["dbname"]=dbname;
    if(user!=NULL)
      opts["user"]=user;
    if(pass!=NULL)
      opts["pass"]=pass;

    return ofb_.build(opts);
  }

  void ensureThrow(const char *host,
                   const char *port,
                   const char *dbname,
                   const char *user,
                   const char *pass) const
  {
    try
    {
      build(host, port, dbname, user, pass);
      tut::fail("build() didn't throw on missing paramter");
    }
    catch(const ConnectionBuilder::ExceptionNoSuchOption&)
    {
      // this is expected
    }
  }

  ConnectionBuilder ofb_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/IO/Postgres/ConnectionBuilder");
} // unnamed namespace


namespace tut
{

// tets getting name
template<>
template<>
void testObj::test<1>(void)
{
  ensure_equals("invalid name", ofb_.getTypeName(), "postgres");
}

// test creating object factory
template<>
template<>
void testObj::test<2>(void)
{
  ConnectionBuilder::FactoryPtr ptr=build();
  ensure("NULL pointere returned", ptr.get()!=NULL );
}

// test throw on missing host name
template<>
template<>
void testObj::test<3>(void)
{
  ensureThrow(NULL, "p", "b", "c", "d");
}

// test throw on missing data base name
template<>
template<>
void testObj::test<4>(void)
{
  ensureThrow("a", "p", NULL, "c", "d");
}

// test throw on missing user name
template<>
template<>
void testObj::test<5>(void)
{
  ensureThrow("a", "p", "b", NULL, "d");
}

// test throw on missing password
template<>
template<>
void testObj::test<6>(void)
{
  ensureThrow("a", "p", "b", "c", NULL);
}

// test throw on invalid options
template<>
template<>
void testObj::test<7>(void)
{
    try
    {
      build("localhost", "5432", "acarm_ng_test_v0_1_0", "acarm-ng-daemon", "BAD_PASSWORD");
      tut::fail("build() didn't throw on invalid options");
    }
    catch(const std::runtime_error&)
    {
      // this is expected
    }
}

// test throw on missing port
template<>
template<>
void testObj::test<8>(void)
{
  ensureThrow("a", NULL, "b", "c", "d");
}

} // namespace tut
