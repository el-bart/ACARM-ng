/*
 * ReaderHelper.t.cpp
 *
 */
#include <tut.h>
#include <sstream>

#include "Persistency/IO/BackendFactory.hpp"
#include "Persistency/IO/Postgres/ReaderHelper.hpp"
#include "Persistency/IO/Postgres/TestConnection.t.hpp"
#include "Persistency/IO/Postgres/TestDBAccess.t.hpp"
#include "Persistency/IO/Postgres/TestHelpers.t.hpp"
#include "Persistency/IO/Postgres/TransactionAPI.hpp"
#include "Persistency/IO/Postgres/timestampFromString.hpp"
#include "Persistency/IO/Postgres/detail/Appender.hpp"

using Persistency::IO::Transaction;
using namespace Persistency;
using namespace Persistency::IO::Postgres;
using namespace Persistency::IO::Postgres::detail;
using namespace std;

namespace
{

template<typename T>
inline pqxx::result execSQL(Transaction &t, const T &sql)
{
  return t.getAPI<TransactionAPI>().exec(sql);
} // execSQL()

struct TestClass
{
  typedef Commons::LimitedNULLString<32> Name;

  TestClass(void):
    idCache_(new IDCache),
    dbh_(DBHandlePtrNN(new DBHandle(TestConnection::makeParams(), idCache_) ) ),
    conn_(makeConnection() ),
    t_( conn_->createNewTransaction("reader_helper_tests") ),
    name_("some name")
  {
  }

  void CreateTempTable()
  {
    execSQL(t_, "CREATE TEMP TABLE tmp"
                "("
                "  val1 int         NULL,"
                "  val2 real        NULL,"
                "  val3 timestamp   NULL,"
                "  val4 varchar(32) NULL,"
                "  val5 inet        NULL"
                ") ON COMMIT DROP;");
  }

  DataCleaner         dc_;
  IDCachePtrNN        idCache_;
  DBHandlePtrNN       dbh_;
  IO::ConnectionPtrNN conn_;
  Transaction         t_;
  Name                name_;
};


typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/IO/Postgres/ReaderHelper");
} // unnamned namespace

namespace tut
{

// trying read NULL values from data base
template<>
template<>
void testObj::test<1>(void)
{
  CreateTempTable();
  execSQL(t_, "INSERT INTO tmp(val1, val2, val3, val4) VALUES(NULL, NULL, NULL, NULL);");
  const pqxx::result r = execSQL(t_, "SELECT * FROM tmp;");
  ensure_equals("invalid size", r.size(), 1u);
  ensure("value isn't NULL", ReaderHelper< Base::NullValue<int> >::readAs(r[0]["val1"]).get() == NULL);
  ensure("value isn't NULL", ReaderHelper< Base::NullValue<double> >::readAs(r[0]["val2"]).get() == NULL);
  t_.commit();
}

// test specialization for int, double and LinitedNULLString
template<>
template<>
void testObj::test<2>(void)
{
  CreateTempTable();
  const int    val1 = 2;
  const double val2 = 1.4;
  stringstream ss;
  Appender     ap(t_);
  ss << "INSERT INTO tmp(val1, val2, val4) VALUES(" << val1 <<", " << val2 << ", ";
  ap.append(ss, name_.get() );
  ss << ");";
  execSQL(t_, ss);
  const pqxx::result r = execSQL(t_, "SELECT * FROM tmp;");
  ensure_equals("invalid size", r.size(), 1u);
  ensure_equals("invalid value", ReaderHelper<int>::readAsNotNull(r[0]["val1"]) , val1);
  ensure_equals("invalid value", ReaderHelper<double>::readAsNotNull(r[0]["val2"]), val2);
  ensure("invalid value", strcmp( ReaderHelper<Name>::readAs(r[0]["val4"]).get(), name_.get() ) == 0 );
}

// test specialization for Timestamp
template<>
template<>
void testObj::test<3>(void)
{
  CreateTempTable();
  string time("1970-01-15 07:56:07");
  const Timestamp ts=timestampFromString( time );
  stringstream ss;
  Appender     ap(t_);
  ss << "INSERT INTO tmp(val3) VALUES(";
  ap.append(ss, time);
  ss << ");";
  execSQL(t_, ss);
  const pqxx::result r = execSQL(t_, "SELECT * FROM tmp;");
  ensure_equals("invalid size", r.size(), 1u);
  ensure_equals("invalid time", ReaderHelper< Base::NullValue<Timestamp> >::readAs(r[0]["val3"]).get()->get() , ts.get());
}
// test specialization for IP
template<>
template<>
void testObj::test<4>(void)
{
  CreateTempTable();
  string adr("1.2.3.4");
  boost::asio::ip::address ip(boost::asio::ip::address::from_string ( adr ) );
  stringstream ss;
  Appender     ap(t_);
  ss << "INSERT INTO tmp(val5) VALUES(";
  ap.append(ss, ip.to_string());
  ss << ");";
  execSQL(t_, ss);
  const pqxx::result r = execSQL(t_, "SELECT * FROM tmp;");
  ensure_equals("invalid size", r.size(), 1u);
  ensure("NULL value has been read", r[0]["val5"].is_null()==false);
  ensure("invalid IP adress", ReaderHelper<boost::asio::ip::address>::readAsNotNull(r[0]["val5"]) == ip);
}

// test specialization for LimitedNULLString
template<>
template<>
void testObj::test<5>(void)
{
  CreateTempTable();
  // create LimitedNULLString
  Name s("some string");
  stringstream ss;
  Appender     ap(t_);
  ss << "INSERT INTO tmp(val4) VALUES(";
  ap.append(ss, s.get() );
  ss << ");";
  execSQL(t_, ss);
  const pqxx::result r = execSQL(t_, "SELECT * FROM tmp;");
  ensure_equals("invalid size", r.size(), 1u);
  ensure("invalid value", strcmp( ReaderHelper<Name>::readAs(r[0]["val4"]).get(), s.get() ) == 0 );
}

// test specialization for LimitedNULLString - NULL value
template<>
template<>
void testObj::test<6>(void)
{
  CreateTempTable();
  // create LimitedNULLString
  Name s(NULL);
  stringstream ss;
  Appender     ap(t_);
  ss << "INSERT INTO tmp(val4) VALUES(";
  ap.append(ss, s.get() );
  ss << ");";
  execSQL(t_, ss);
  const pqxx::result r = execSQL(t_, "SELECT * FROM tmp;");
  ensure_equals("invalid number of elements returned", r.size(), 1u);
  ensure("non-NULL value read", ReaderHelper<Name>::readAs(r[0]["val4"]).get()==NULL );
}

} // namespace tut
