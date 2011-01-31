/*
 * Parse.t.cpp
 *
 */
#include <tut.h>
#include <cassert>

#include "XML/XMLpp/SaxParser.hpp"
#include "ConfigIO/Preprocessor/Parse.hpp"

using namespace std;
using namespace ConfigIO::Preprocessor;

namespace
{

const char *xmlEmpty=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor/>"
  "</acarm_ng>"
  "";

const char *xmlNoPreprocessor=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "</acarm_ng>"
  "";

const char *xmlTermInSection=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <true/>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlMultipleSections=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <true/>"
  "    </accept>"
  "    <reject>"
  "      <false/>"
  "    </reject>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlTermRuleEquals=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <rule path=\"a.b.c\" equals=\"alice has a cat\"/>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlTermRuleContains=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <rule path=\"a.b.c\" contains=\"alice has a cat\"/>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionOr=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <or>"
  "        <true/>"
  "        <false/>"
  "      </or>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionAnd=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <and>"
  "        <true/>"
  "        <false/>"
  "      </and>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionNot=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <not>"
  "        <true/>"
  "      </not>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionNotWithoutChildren=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <not/>"    // <-- not must have children
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionOrWithoutChildren=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <or/>"     // <-- or must have children
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionAndWithoutChildren=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <and/>"    // <-- and must have children
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlTermTrueWithChildren=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <true>"
  "        <true/>" // <-- true can't have children
  "      </true>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionAndWithoutOneChild=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <and>"
  "        <true/>" // <-- too few arguments for AND
  "      </and>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlTermRuleWithoutPath=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <rule pathXYZ=\"a.b.c\" contains=\"alice has a cat\"/>"    // <-- no 'path'
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlTermRuleWithoutValidMode=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <rule path=\"a.b.c\" abc=\"alice has a cat\"/>"            // <-- invalid 'mode'
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlTermRulePathAndModeSwapped=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <rule contains=\"alice has a cat\" path=\"a.b.c\"/>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionNested=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <and>"
  "        <or>"
  "          <true/>"
  "          <true/>"
  "        </or>"
  "        <false/>"
  "      </and>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";

const char *xmlExpressionAndWithOver2Children=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<acarm_ng>"
  "  <preprocessor>"
  "    <accept>"
  "      <and>"
  "        <true/>"
  "        <false/>"
  "        <true/>"
  "      </and>"
  "    </accept>"
  "  </preprocessor>"
  "</acarm_ng>"
  "";




struct TestClass
{
  // return copyied, parsed subtree
  XML::Node getNodes(const char *xml) const
  {
    assert(xml!=NULL);
    XML::XMLpp::SaxParser sp;
    const XML::Tree       tree=sp.parseContent(xml);
    return tree.getRoot().getChild("preprocessor");
  }

  // return copyied appenders configs
  Config getConfig(const char *xml) const
  {
    const XML::Node n=getNodes(xml);
    const Parse     ppc(n);
    return ppc.getConfig();
  }

  void testThrow(const char *xml, const char *message) const
  {
    try
    {
      getConfig(xml);       // should throw
      tut::fail(message);
    }
    catch(const ConfigIO::ExceptionParseError &)
    {
      // this is expected
    }
    catch(const XML::Exception &)
    {
      // this is expected
    }
  }
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("ConfigIO/Preprocessor/Parse");
} // unnamed namespace


namespace tut
{

// self-test
template<>
template<>
void testObj::test<1>(void)
{
  const XML::Node n=getNodes(xmlEmpty);
  ensure_equals("invalid node returned by helper", n.getName(), "preprocessor");
}

// test throw on no section
template<>
template<>
void testObj::test<2>(void)
{
  testThrow(xmlNoPreprocessor, "parser didn't thorow on lack of preprocessor's section");
}

// test section with term
template<>
template<>
void testObj::test<3>(void)
{
  const Config      c=getConfig(xmlTermInSection);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::TERM);
  const Rule       &r=e.getRules().at(0);
  ensure("invalid rule type", r.getType()==Rule::Type::RULE_TRUE);
}

// test multiple sections
template<>
template<>
void testObj::test<4>(void)
{
  const Config c=getConfig(xmlMultipleSections);
  ensure_equals("invalid number of sections", c.getSections().size(), 2u);
  // test section 1
  {
    const Section    &s=c.getSections().at(0);
    ensure("invalid section 1 type", s.getType()==Section::Type::ACCEPT);
    const Expression &e=s.getExpression();
    ensure("invalid expression 1 type", e.getType()==Expression::Type::TERM);
    const Rule       &r=e.getRules().at(0);
    ensure("invalid rule 1 type", r.getType()==Rule::Type::RULE_TRUE);
  }
  // test section 2
  {
    const Section    &s=c.getSections().at(1);
    ensure("invalid section 2 type", s.getType()==Section::Type::REJECT);
    const Expression &e=s.getExpression();
    ensure("invalid expression 2 type", e.getType()==Expression::Type::TERM);
    const Rule       &r=e.getRules().at(0);
    ensure("invalid rule 2 type", r.getType()==Rule::Type::RULE_FALSE);
  }
}

// test term-rule-equals:
template<>
template<>
void testObj::test<5>(void)
{
  const Config      c=getConfig(xmlTermRuleEquals);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::TERM);
  const Rule       &r=e.getRules().at(0);
  ensure("invalid rule type", r.getType()==Rule::Type::RULE);
  ensure("invalid rule mode", r.getMode()==Rule::Mode::EQUALS);
  ensure_equals("invalid path", r.getPath(), "a.b.c");
  ensure_equals("invalid value", r.getValue(), "alice has a cat");
}

// test term-rule-contains
template<>
template<>
void testObj::test<6>(void)
{
  const Config      c=getConfig(xmlTermRuleContains);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::TERM);
  const Rule       &r=e.getRules().at(0);
  ensure("invalid rule type", r.getType()==Rule::Type::RULE);
  ensure("invalid rule mode", r.getMode()==Rule::Mode::CONTAINS);
  ensure_equals("invalid path", r.getPath(), "a.b.c");
  ensure_equals("invalid value", r.getValue(), "alice has a cat");
}

// test expression-or
template<>
template<>
void testObj::test<7>(void)
{
  const Config      c=getConfig(xmlExpressionOr);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::OR);
  ensure_equals("invalid OR expressions count", e.getExpressions().size(), 2u);
  // test expression 1
  {
    const Expression &tmp=e.getExpressions().at(0);
    ensure("invalid expression 1 type", tmp.getType()==Expression::Type::TERM);
  }
  // test expression 2
  {
    const Expression &tmp=e.getExpressions().at(1);
    ensure("invalid expression 2 type", tmp.getType()==Expression::Type::TERM);
  }
}

// test expression-and
template<>
template<>
void testObj::test<8>(void)
{
  const Config      c=getConfig(xmlExpressionAnd);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::AND);
  ensure_equals("invalid OR expressions count", e.getExpressions().size(), 2u);
  // test expression 1
  {
    const Expression &tmp=e.getExpressions().at(0);
    ensure("invalid expression 1 type", tmp.getType()==Expression::Type::TERM);
  }
  // test expression 2
  {
    const Expression &tmp=e.getExpressions().at(1);
    ensure("invalid expression 2 type", tmp.getType()==Expression::Type::TERM);
  }
}

// test expression-not
template<>
template<>
void testObj::test<9>(void)
{
  const Config      c=getConfig(xmlExpressionNot);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::NOT);
  ensure_equals("invalid OR expressions count", e.getExpressions().size(), 1u);
  // test expression
  const Expression &tmp=e.getExpressions().at(0);
  ensure("invalid expression type", tmp.getType()==Expression::Type::TERM);
}

// test 'not' without children
template<>
template<>
void testObj::test<10>(void)
{
  testThrow(xmlExpressionNotWithoutChildren,
            "parse didn't throw on no children for 'not'");
}

// test 'or' without childrent
template<>
template<>
void testObj::test<11>(void)
{
  testThrow(xmlExpressionOrWithoutChildren,
            "parse didn't throw on no children for 'or'");
}

// test 'and' without children
template<>
template<>
void testObj::test<12>(void)
{
  testThrow(xmlExpressionAndWithoutChildren,
            "parse didn't throw on no children for 'and'");
}

// test throw on 'true' withc children
template<>
template<>
void testObj::test<13>(void)
{
  testThrow(xmlTermTrueWithChildren,
            "parse didn't throw on children for 'true'");
}

// test throw on single child given for AND
template<>
template<>
void testObj::test<14>(void)
{
  testThrow(xmlExpressionAndWithoutOneChild, "parse didn't throw on 1 child");
}

// test throw on invalid path given for term-rule
template<>
template<>
void testObj::test<15>(void)
{
  testThrow(xmlTermRuleWithoutPath, "parse didn't throw on no path for rule");
}

// test throw when invalid mode is given for rule
template<>
template<>
void testObj::test<16>(void)
{
  testThrow(xmlTermRuleWithoutValidMode, "parse didn't throw on invalid rule mode");
}

// test if parsing is valid when 'path' and 'mode' are swaped in config
template<>
template<>
void testObj::test<17>(void)
{
  const Config      c=getConfig(xmlTermRulePathAndModeSwapped);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::TERM);
  const Rule       &r=e.getRules().at(0);
  ensure("invalid rule type", r.getType()==Rule::Type::RULE);
  ensure("invalid rule mode", r.getMode()==Rule::Mode::CONTAINS);
  ensure_equals("invalid path", r.getPath(), "a.b.c");
  ensure_equals("invalid value", r.getValue(), "alice has a cat");
}

// test nested expressions
template<>
template<>
void testObj::test<18>(void)
{
  const Config      c=getConfig(xmlExpressionNested);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::AND);
  ensure_equals("invalid arguments count for root 'and'", e.getExpressions().size(), 2u);
  // test 'or' argument
  {
    const Expression &eTmp=e.getExpressions().at(0);
    ensure("invalid expression 1", eTmp.getType()==Expression::Type::OR);
    ensure_equals("invalid children count", eTmp.getExpressions().size(), 2u);
    // test rule 1
    {
      const Expression &te=eTmp.getExpressions().at(0);
      ensure("invalid term/expression 1", te.getType()==Expression::Type::TERM);
    }
    // test rule 2
    {
      const Expression &te=eTmp.getExpressions().at(1);
      ensure("invalid term/expression 2", te.getType()==Expression::Type::TERM);
    }
  }
  // test 'true' term
  {
    const Expression &eTmp=e.getExpressions().at(1);
    ensure("invalid term 2", eTmp.getType()==Expression::Type::TERM);
  }
}

// test args>2 for 'and'
template<>
template<>
void testObj::test<19>(void)
{
  const Config      c=getConfig(xmlExpressionAndWithOver2Children);
  ensure_equals("invalid number of sections", c.getSections().size(), 1u);
  const Section    &s=c.getSections().at(0);
  ensure("invalid section type", s.getType()==Section::Type::ACCEPT);
  const Expression &e=s.getExpression();
  ensure("invalid expression type", e.getType()==Expression::Type::AND);
  ensure_equals("invalid OR expressions count", e.getExpressions().size(), 3u);
  // test expression 1
  {
    const Expression &tmp=e.getExpressions().at(0);
    ensure("invalid expression 1 type", tmp.getType()==Expression::Type::TERM);
  }
  // test expression 2
  {
    const Expression &tmp=e.getExpressions().at(1);
    ensure("invalid expression 2 type", tmp.getType()==Expression::Type::TERM);
  }
  // test expression 3
  {
    const Expression &tmp=e.getExpressions().at(2);
    ensure("invalid expression 3 type", tmp.getType()==Expression::Type::TERM);
  }
}

} // namespace tut
