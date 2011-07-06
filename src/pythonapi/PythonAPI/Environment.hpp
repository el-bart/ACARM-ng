/*
 * Environment.hpp
 *
 */
#ifndef INCLUDE_PYTHONAPI_ENVIRONMENT_HPP_FILE
#define INCLUDE_PYTHONAPI_ENVIRONMENT_HPP_FILE

/* public header */

#include <string>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "Logger/Node.hpp"
#include "PythonAPI/Exception.hpp"
#include "PythonAPI/ExceptionFromScript.hpp"


namespace PythonAPI
{

class Environment: private boost::noncopyable
{
public:
  typedef PyObject* (*ModuleInitFunc)(void);

  class StaticImporter
  {
  public:
    StaticImporter(const char *module, ModuleInitFunc init);
    ~StaticImporter(void);

  private:
    bool imported_;     // will be 0 (false) when declared static
  }; // struct StaticImporter


  Environment(void);

  void run(const std::string &script);

  /** \brief gets variable from python.
   *
   *  this returns objects by value, which is not always expected. in particular
   *  it is not valid for classes, returned by pointers, since they may be
   *  salvaged by python's garbage collector. to get these the safe way use
   *  boost::shared_ptr<MyClass> as a destination type for MyClass pointers.
   *
   *  \note use boost::shared_ptr<MyClass> to take object with ownership.
   *
   *  \param name name of the variable to get.
   *  \return variable's value.
   */
  template<typename T>
  T var(const std::string &name)
  {
    return boost::python::extract<T>( (mainNamespace_[name]) );
  }

  static void importModule(const std::string &module, ModuleInitFunc init);

  void importModule(const std::string &module);

private:
  Logger::Node          log_;
  boost::python::object mainModule_;
  boost::python::object mainNamespace_;
}; // class ExceptionHandle

} // namespace PythonAPI

#endif
