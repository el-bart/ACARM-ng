/*
 * AlertsFifo.hpp
 *
 */
#ifndef INCLUDE_CORE_TYPES_ALERTSFIFLO_HPP_FILE
#define INCLUDE_CORE_TYPES_ALERTSFIFLO_HPP_FILE

/* public header */

#include "Base/Threads/FifoQueue.hpp"
#include "Persistency/Alert.hpp"

namespace Core
{
namespace Types
{

/** \brief input fifo queue type holding incomming alerts.
 */
typedef Base::Threads::FifoQueue<Persistency::AlertPtrNN> AlertsFifo;

} // namespace Types
} // namespace Core

#endif
