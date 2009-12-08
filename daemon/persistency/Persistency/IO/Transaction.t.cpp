/*
 * Transaction.t.cpp
 *
 */
#include <tut.h>
#include <cstring>
#include <memory>

#include "Persistency/IO/Transaction.hpp"

using namespace std;
using namespace Persistency::IO;

namespace
{

struct TestTransactionAPI: public Persistency::IO::TransactionAPI
{
  TestTransactionAPI(Base::Threads::Mutex &mutex, int *commits, int *rollbacks):
    Persistency::IO::TransactionAPI(mutex, "test_transaction"),
    commited_(commits),
    rollbacked_(rollbacks)
  {
  }
  virtual void commitImpl(void)
  {
    ++*commited_;
  }
  virtual void rollbackImpl(void)
  {
    ++*rollbacked_;
  }

  int *commited_;
  int *rollbacked_;
};

struct TestClass
{
  TestClass(void):
    commits_(0),
    rollbacks_(0),
    tapi_( new TestTransactionAPI(mutex_, &commits_, &rollbacks_) ),
    t_( new Transaction(tapi_) )
  {
    tut::ensure("API ownership not taken", tapi_.get()==NULL);
  }

  int                        commits_;
  int                        rollbacks_;
  Base::Threads::Mutex       mutex_;
  TransactionAPIAutoPtr      tapi_;
  std::auto_ptr<Transaction> t_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/IO/Transaction");
} // unnamed namespace


namespace tut
{

// ensure no commit/rollback has been performed in c-tor
template<>
template<>
void testObj::test<1>(void)
{
  ensure_equals("commit called in c-tor",   commits_,   0);
  ensure_equals("rollback called in c-tor", rollbacks_, 0);
}

// check default rollback
template<>
template<>
void testObj::test<2>(void)
{
  t_.reset(NULL);   // force d-tor call
  ensure_equals("commit called in d-tor",       commits_,   0);
  ensure_equals("rollback not called in d-tor", rollbacks_, 1);
}

// check if no double-rollback is not called in d-tor
template<>
template<>
void testObj::test<3>(void)
{
  t_->rollback();
  ensure_equals("commit called in rollback",       commits_,   0);
  ensure_equals("rollback not called in rollback", rollbacks_, 1);

  t_.reset(NULL);   // force d-tor call
  ensure_equals("commit called in d-tor",                  commits_,   0);
  ensure_equals("rollback called after rollback in d-tor", rollbacks_, 1);
}

// check if double rollbacks are secure
template<>
template<>
void testObj::test<4>(void)
{
  t_->rollback();
  t_->rollback();
  ensure_equals("commit called in rollback", commits_,   0);
  ensure_equals("multiple rollbacks called", rollbacks_, 1);
}

// test if double commits do not fail
template<>
template<>
void testObj::test<5>(void)
{
  t_->commit();
  t_->commit();
  ensure_equals("multiple commits called",   commits_,   1);
  ensure_equals("rollback called in commit", rollbacks_, 0);
}

// test d-tor after commit
template<>
template<>
void testObj::test<6>(void)
{
  t_->commit();     // commit
  t_.reset(NULL);   // force d-tor
  ensure_equals("commit called too many times", commits_,   1);
  ensure_equals("rollback called after commit", rollbacks_, 0);
}

// test transaction when NULL API is given
template<>
template<>
void testObj::test<7>(void)
{
  try
  {
    Transaction t( TransactionAPIAutoPtr(NULL) );
    fail("Transaction(NULL) didn't throw");
  }
  catch(const Persistency::ExceptionNULLParameter&)
  {
    // this is expected
  }
}

} // namespace tut
