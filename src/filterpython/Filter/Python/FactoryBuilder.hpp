/*
 * FactoryBuilder.hpp
 *
 */
#ifndef INCLUDE_FILTER_PYTHON_FACTORYBUILDER_HPP_FILE
#define INCLUDE_FILTER_PYTHON_FACTORYBUILDER_HPP_FILE

#include <string>

#include "Logger/Logger.hpp"
#include "Filter/Factory.hpp"

namespace Filter
{
namespace Python
{

/** \brief builds same-name filter
 */
class FactoryBuilder: public Factory::TFactoryBuilderBase
{
public:
  /** \brief creates builder's instance.
   */
  FactoryBuilder(void);

private:
  virtual FactoryPtr buildImpl(const Options &options) const;
  virtual const FactoryTypeName &getTypeNameImpl(void) const;

  const Core::Types::Proc::TypeName type_;
  const Logger::Node                log_;
}; // class FactoryBuilder

} // namespace Python
} // namespace Filter

#endif