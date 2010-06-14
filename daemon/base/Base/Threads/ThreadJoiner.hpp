/*
 * ThreadJoiner.hpp
 *
 */
#ifndef INCLUDE_BASE_THREADS_THREADJOINER_HPP_FILE
#define INCLUDE_BASE_THREADS_THREADJOINER_HPP_FILE

/* public header */

#include <boost/thread.hpp>
#include <boost/any.hpp>            // TODO: think of smarter way of doing this
#include <boost/noncopyable.hpp>
#include <cassert>

namespace Base
{
namespace Threads
{

/** \brief wrapper for boost::thread that ensures thread is interrupted
 *         and joined when d-tor is called.
 *
 *  thanks to this class threads are known to be interrupted and joined even
 *  when normal flow (i.e. non-exception one) would not allow this.
 */
class ThreadJoiner: private boost::noncopyable
{
public:
  /** \brief create and start thread from any object that would be accepted
   *         by boost::thread itself.
   *  \param t function object to be run in thread.
   */
  template<typename T>
  explicit ThreadJoiner(const T &t):
    holder_(t),
    //th_(t)    // TODO: why this variant's not always working?
    th_( Operate<T>( boost::any_cast<T>(&holder_) ) )
  {
    // ensure pointers are valid (i.e. from the same object)
    assert( boost::any_cast<T>(&holder_)==boost::any_cast<T>(&holder_) );
  }
  /** \brief interrupr and join thread.
   *
   *  interrupt thread and join it. call block until thread
   *  will finish processing.
   */
  ~ThreadJoiner(void)
  {
    assert( boost::this_thread::get_id()!=th_.get_id() );   // sanity check
    th_.interrupt();
    th_.join();
  }

  /** \brief arrow operator to access underlying thread directly.
   *  \return pointer to thread held inside.
   *  \note returned pointer is guaranteed to be non-NULL.
   */
  boost::thread *operator->(void)
  {
    return &th_;
  }

private:
  template<typename T>
  struct Operate
  {
    explicit Operate(T *t):
      t_(t)
    {
      assert(t_!=NULL);
    }
    void operator()(void)
    {
      assert(t_!=NULL);
      (*t_)();
    }

  private:
    T *t_;
  }; // struct Operate

  boost::any    holder_;
  boost::thread th_;
}; // class ThreadJoiner

} // namespace Threads
} // namespace Base

#endif
