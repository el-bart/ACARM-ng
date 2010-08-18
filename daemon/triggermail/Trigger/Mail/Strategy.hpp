/*
 * Strategy.hpp
 *
 */
#ifndef INCLUDE_TRIGGER_MAIL_STRATEGY_HPP_FILE
#define INCLUDE_TRIGGER_MAIL_STRATEGY_HPP_FILE

#include "Trigger/Simple/Strategy.hpp"
#include "Trigger/Mail/Config.hpp"

namespace Trigger
{
namespace Mail
{

/** \brief mail triggering strategy
 */
class Strategy: public Trigger::Simple::Strategy
{
public:
  // TODO: explicit is not needed here.
  /** \brief create strategy instance.
   *  \param name name to be applied.
   *  \param cfg configuration to be applied.
   */
  explicit Strategy(const std::string &name, const Config &cfg);

private:
  virtual void triggerImpl(const Node &n);

  const Config cfg_;
}; // class Strategy

} // namespace Mail
} // namespace Trigger

#endif
