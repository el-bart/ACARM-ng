/*
 * FactoryBuilderBase.t.cpp
 *
 */
#include <tut.h>

#include "Commons/Factory/detail/FactoryBuilderBase.hpp"

using namespace Commons::Factory::detail;


namespace
{

struct TestFactory
{
  template<typename T>
  explicit TestFactory(const T &)
  {
  }
};

typedef FactoryBuilderBase<TestFactory> TestBuilderBase;


struct TestBuilder: public TestBuilderBase
{
  typedef TestBuilderBase Base;

  TestBuilder(void):
    name_("test-factory-1")
  {
  }

  virtual Base::FactoryPtr build(const Base::Options &options) const
  {
    return Base::FactoryPtr( new TestFactory(options) );
  }

  virtual const Base::FactoryTypeName &getTypeName(void) const
  {
    return name_;
  }

  Base::FactoryTypeName name_;
};

struct FactoryBuilderBaseTestClass
{
  FactoryBuilderBaseTestClass(void):
    base_(tb_)
  {
  }

  TestBuilder      tb_;
  TestBuilderBase &base_;
};

typedef FactoryBuilderBaseTestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Commons/Factory/detail/FactoryBuilderBase");
} // unnamed namespace


namespace tut
{

template<>
template<>
void testObj::test<1>(void)
{
  ensure_equals("invalid name", base_.getTypeName(), "test-factory-1");
}

template<>
template<>
void testObj::test<2>(void)
{
  const TestBuilderBase::Options opts;
  TestBuilderBase::FactoryPtr    ptr=base_.build(opts);
  ensure("NULL pointer returned", ptr.get()!=NULL);
  // check if type's fine
  TestFactory                    tf =*ptr;
}

} // namespace tut
