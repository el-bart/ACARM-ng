/*
 * Alert.cpp
 *
 */
#include <cassert>

#include "Persistency/Alert.hpp"
#include "Base/ViaPointer.hpp"
#include "Commons/ViaUnorderedCollection.hpp"

using namespace std;

namespace Persistency
{

Alert::Alert(const Name          &name,
             SourceAnalyzers      analyzers,
             const Timestamp     *detected,
             const Timestamp     &created,
             Severity             severity,
             Certainty            certainty,
             const std::string   &description,
             const ReportedHosts &sourceHosts,
             const ReportedHosts &targetHosts):
  name_(name),
  analyzers_(analyzers),
  detected_( (detected!=NULL)?( new Timestamp(*detected) ):NULL ),
  created_(created),
  severity_(severity),
  certainty_(certainty),
  description_(description),
  sourceHosts_(sourceHosts),
  targetHosts_(targetHosts)
{
}

const Alert::Name &Alert::getName(void) const
{
  return name_;
}

const Alert::SourceAnalyzers &Alert::getSourceAnalyzers(void) const
{
  assert( analyzers_.size()>0 );
  return analyzers_;
}

const Timestamp *Alert::getDetectionTime(void) const
{
  return detected_.get();
}

const Timestamp &Alert::getCreationTime(void) const
{
  return created_;
}

const Severity &Alert::getSeverity(void) const
{
  return severity_;
}

Certainty Alert::getCertainty(void) const
{
  return certainty_;
}

const std::string &Alert::getDescription(void) const
{
  return description_;
}

const Alert::ReportedHosts &Alert::getReportedSourceHosts(void) const
{
  return sourceHosts_;
}

const Alert::ReportedHosts &Alert::getReportedTargetHosts(void) const
{
  return targetHosts_;
}

bool Alert::operator==(const Alert &other) const
{
  if(this==&other)
    return true;

  if( getName()!=other.getName() )
    return false;
  if( !Commons::ViaUnorderedCollection::equal( getSourceAnalyzers(), other.getSourceAnalyzers() ) )
    return false;
  if( !Base::ViaPointer::equal( getDetectionTime(), other.getDetectionTime() ) )
    return false;
  if( getCreationTime()!=other.getCreationTime() )
    return false;
  if( getSeverity()!=other.getSeverity() )
    return false;
  if( getCertainty()!=other.getCertainty() )
    return false;
  if( getDescription()!=other.getDescription() )
    return false;
  if( !Commons::ViaUnorderedCollection::equal( getReportedSourceHosts(), other.getReportedSourceHosts() ) )
    return false;
  if( !Commons::ViaUnorderedCollection::equal( getReportedTargetHosts(), other.getReportedTargetHosts() ) )
    return false;
  // if all fields are equal, objects are equal too.
  return true;
}

} // namespace Persistency
