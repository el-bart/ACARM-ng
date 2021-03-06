/*
 * Alert.hpp
 *
 */
#ifndef INCLUDE_PERSISTENCY_IO_ALERT_HPP_FILE
#define INCLUDE_PERSISTENCY_IO_ALERT_HPP_FILE

/* public header */

#include <memory>
#include <boost/noncopyable.hpp>

#include "Persistency/Alert.hpp"
#include "Persistency/IO/Transaction.hpp"
#include "Persistency/IO/Exception.hpp"

namespace Persistency
{
namespace IO
{

/** \brief operations on Persistency::Alert API
 */
class Alert: private boost::noncopyable
{
public:
  /** \brief create alert persistency object for given alert.
   *  \param alert alert to create object for.
   *  \param t     associated transaction.
   */
  Alert(Persistency::AlertPtrNN  alert,
        Transaction             &t);
  /** \brief virtual d-tor for polymorphic base class.
   */
  virtual ~Alert(void);
  /** \brief save alert to persistent storage.
   */
  void save(void);

protected:
  /** \brief gets read-only acces to alert.
   *  \return not null pointer to alert to operate on.
   */
  Persistency::AlertPtrNN get(void);

private:
  virtual void saveImpl(Transaction &t) = 0;

  Persistency::AlertPtrNN  alert_;
  Transaction             &t_;
}; // class Alert


/** \brief auto pointer to IO::Alert class. */
typedef std::auto_ptr<Alert> AlertAutoPtr;

} // namespace IO
} // namespace Persistency

#endif
