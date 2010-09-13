/*
 * ToXML.hpp
 *
 */
#ifndef INCLUDE_RFCIO_XMLCREATOR_HPP_FILE
#define INCLUDE_RFCIO_XMLCREATOR_HPP_FILE

#include <string>
#include <boost/noncopyable.hpp>
#include <libxml++/libxml++.h>
#include <cassert>

#include "Commons/LimitedString.hpp"
#include "Commons/LimitedNULLString.hpp"
#include "Logger/Node.hpp"
#include "Persistency/GraphNode.hpp"
#include "RFCIO/Exception.hpp"
#include "RFCIO/ExceptionUnexpectedNULL.hpp"

namespace RFCIO
{

/** \brief class converting given elements to XML and adding them to
 *         a given parent.
 */
class ToXML: private boost::noncopyable
{
public:
  /** \brief create class adding elements to XML tree of a specified parent.
   *  \param parent node to add elements to.
   *  \note parent node is taken via reference and held this way, thus object it
   *        refers to must live at least as long as this object does.
   */
  explicit ToXML(xmlpp::Element &parent);

  /** \brief adds alert to parent node and returns reference to new element.
   *  \param leaf leaf (i.e. alert) to be transformed to XML.
   *  \return reference to newly added element.
   */
  xmlpp::Element &addAlert(const Persistency::GraphNode &leaf);

  /** \brief add analyzer's node and return reference to it.
   *  \param a analyzer to be added.
   *  \return reference to newly added element.
   */
  xmlpp::Element &addAnalyzer(const Persistency::Analyzer &a);

  /** \brief returns parent node's reference.
   *  \return reference to node given as a parent in c-tor.
   */
  xmlpp::Element &getParent(void)
  {
    return parent_;
  }


private:
  template<uint16_t N>
  xmlpp::Element &addString(const char *name, const Commons::LimitedNULLString<N> &str)
  {
    assert(name!=NULL);
    if( str.get()==NULL )   // if it's NULL it should not be here in the first place
      throw ExceptionUnexpectedNULL(SYSTEM_SAVE_LOCATION, name);
    return addStringImpl( name, str.get() );
  }
  template<uint16_t N>
  xmlpp::Element &addString(const char *name, const Commons::LimitedString<N> &str)
  {
    return addStringImpl( name, str.get() );
  }
  xmlpp::Element &addString(const char *name, const char *str);
  xmlpp::Element &addStringImpl(const char *name, const char *std);

  xmlpp::Element &addChild(xmlpp::Element &parent, const char *name);

  void addParameterIfNotNULL(const char *name, const char *value);
  void addParameter(const char *name, const char *value);

  Logger::Node    log_;
  xmlpp::Element &parent_;
}; // class ToXML

} // namespace RFCIO

#endif
