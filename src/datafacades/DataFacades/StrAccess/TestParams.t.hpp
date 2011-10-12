/*
 * TestParams.t.hpp
 *
 */
#ifndef INCLUDE_DATAFACADES_SACCESS_TESTPARAMS_T_HPP_FILE
#define INCLUDE_DATAFACADES_SACCESS_TESTPARAMS_T_HPP_FILE

#include "DataFacades/StrAccess/Params.hpp"
#include "DataFacades/StrAccess/DefaultHandleMap.hpp"
#include "DataFacades/StrAccess/TestCallback.t.hpp"

namespace
{

typedef Facades::StrAccess::Params<Facades::StrAccess::DefaultHandleMap,
                                   Facades::StrAccess::TestCallback>     TestParams;

} // unnamed namespace

#endif
