/*
 * FromXML.cpp
 *
 */
#include <utility>
#include <iterator>
#include <algorithm>
#include <boost/tokenizer.hpp>
#include <cassert>

#include "Base/NullValue.hpp"
#include "Commons/Convert.hpp"
#include "Persistency/Facades/IDAssigner.hpp"
#include "RFCIO/FromXML.hpp"
#include "RFCIO/TimeConverter.hpp"

using namespace std;
using namespace Persistency;
using Base::NullValue;
using Commons::Convert;

namespace RFCIO
{

// helper, unnamed namespace
namespace
{
string parseString(const xmlpp::Element &node)
{
  const xmlpp::TextNode *txt=node.get_child_text();
  if(txt==NULL)
    throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, node.get_path(), "node is not text node");
  return txt->get_content();
}

NullValue<string> parseParameterIfHas(const xmlpp::Element &node, const char *name)
{
  assert(name!=NULL);
  const xmlpp::Attribute *tmp=node.get_attribute(name);
  if(tmp==NULL)
    return NullValue<string>();
  return NullValue<string>( tmp->get_value() );
}

string parseParameter(const xmlpp::Element &node, const char *name)
{
  const NullValue<string> tmp=parseParameterIfHas(node, name);
  if( tmp.get()==NULL )
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, node.get_path(), name);
  return *tmp.get();
}

const xmlpp::Element *findOneChildIfHas(const xmlpp::Element &parent, const char *name)
{
  assert(name!=NULL);
  const xmlpp::Element::NodeList nl=parent.get_children(name);
  if( nl.size()==0 )
    return NULL;
  if( nl.size()>1 )
    throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, parent.get_path(), "requested name is not unique: " + string(name) );
  return dynamic_cast<const xmlpp::Element*>( *nl.begin() );
}

const xmlpp::Element &findOneChild(const xmlpp::Element &parent, const char *name)
{
  const xmlpp::Element *ptr=findOneChildIfHas(parent, name);
  if(ptr==NULL)
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, parent.get_path(), name);
  return *ptr;
}

template<typename T>
NullValue<T> parseChildIfHasAs(const xmlpp::Element &node, const char *name)
{
  const xmlpp::Element *e=findOneChildIfHas(node, name);
  if(e==NULL)
    return NullValue<T>();
  return NullValue<T>( Commons::Convert::to<T>( parseString(*e) ) );
}
} // unnamed namespace

FromXML::FromXML(Persistency::IO::ConnectionPtrNN conn, Persistency::IO::Transaction &t):
  log_("rfcio.fromxml"),
  conn_(conn),
  t_(t)
{
}

Persistency::GraphNodePtrNN FromXML::parseAlert(const xmlpp::Element &alert) const
{
  // sanity check
  ensureNode("Alert", alert);
  // analyzer (only one can be in XML)
  const AnalyzerPtrNN          analyzer=parseAnalyzer( findOneChild(alert, "Analyzer") );
  const Alert::SourceAnalyzers analyzers(analyzer);
  // creation time
  const Timestamp createTime=parseCreateTime( findOneChild(alert, "CreateTime") );
  // detect time, if present
  Timestamp             detectTimeValue(0);
  const Timestamp      *detectTime    =NULL;
  const xmlpp::Element *detectTimeNode=findOneChildIfHas(alert, "DetectTime");
  if(detectTimeNode!=NULL)
  {
    detectTimeValue=parseDetectTime(*detectTimeNode);
    detectTime     =&detectTimeValue;
  }
  // source host, if present
  Alert::ReportedHosts  sourceHosts;
  const xmlpp::Element *sourceNode=findOneChildIfHas(alert, "Source");
  if(sourceNode!=NULL)
    sourceHosts.push_back( parseSource(*sourceNode) );
  // destination host, if present
  Alert::ReportedHosts  targetHosts;
  const xmlpp::Element *targetNode=findOneChildIfHas(alert, "Target");
  if(targetNode!=NULL)
    targetHosts.push_back( parseTarget(*targetNode) );
  // classification -> alert's name
  const Classification classification=parseClassification( findOneChild(alert, "Classification") );
  const string         name          =classification.get<0>();
  // note that ReferenceURL (aka: classification.get<1>()) is
  // additional data -> description
  string                description;
  const xmlpp::Element *additionalData=findOneChildIfHas(alert, "AdditionalData");
  if(additionalData!=NULL)
    description=parseAdditionalData(*additionalData);
  // assessment -> {severity, certainty}
  const Assessment assessment=parseAssessment( findOneChild(alert, "Assessment") );
  const Severity   severity  =assessment.get<0>();
  const Certainty  certainty =assessment.get<1>();

  // create alert
  const AlertPtrNN alertPtr( new Alert(name, analyzers, detectTime, createTime, severity, certainty,
                                       description, sourceHosts, targetHosts) );

  // warn about not using ID from XML, if present
  const NullValue<string> messageID=parseParameterIfHas(alert, "messageid");
  const MetaAlert::ID     id       =Facades::IDAssigner::get()->assignMetaAlertID(conn_, t_);
  if( messageID.get()!=NULL )
    LOGMSG_WARN_S(log_)<<"although alert's ID is set to '"<<*messageID.get()
                       <<"' setting local (in system) ID to "<<id.get();

  // return final result
  return GraphNodePtrNN( new GraphNode(alertPtr, id, conn_, t_) );
}

Persistency::AnalyzerPtrNN FromXML::parseAnalyzer(const xmlpp::Element &analyzer) const
{
  ensureNode("Analyzer", analyzer);
  const FromXML::NodeData  nodeData   =parseNode( findOneChild(analyzer, "Node") );
  const char              *name       =(nodeData.first.get()==NULL)?NULL:nodeData.first->c_str();
  const NullValue<string>  versionData=parseParameterIfHas(analyzer, "version");
  const char              *version    =(versionData.get()==NULL)?NULL:versionData.get()->c_str();
  const NullValue<string>  osData     =parseParameterIfHas(analyzer, "ostype");
  const char              *os         =(osData.get()==NULL)?NULL:osData.get()->c_str();
  Persistency::AnalyzerPtrNN out=analyzersCreator_.construct( conn_, t_, name, version, os, nodeData.second.get() );
  // check if ID is assigned the same way the one in file was. if not, report an error and proceed.
  const NullValue<string>  idData     =parseParameterIfHas(analyzer, "analyzerid");
  const char              *id         =(idData.get()==NULL)?NULL:idData.get()->c_str();
  if(id!=NULL)
  {
    if( Convert::to<string>( out->getID().get() )!=id )
      LOGMSG_ERROR_S(log_) << "parsed analyzer has ID '" << id << "', but system assigned it ID "
                           << out->getID().get() << "; this can happen if alert comes from different "
                              "system that assignes IDs independently; proceeding with id "
                           << out->getID().get();
  }
  // return the result
  return out;
}

Persistency::Timestamp FromXML::parseCreateTime(const xmlpp::Element &createTime) const
{
  return parseTimestamp("CreateTime", createTime);
}

Persistency::Timestamp FromXML::parseDetectTime(const xmlpp::Element &detectTime) const
{
  return parseTimestamp("DetectTime", detectTime);
}

// unnamed namespace for helper calls
namespace
{
SeverityLevel parseImpactValue(const string &imp)
{
  if(imp=="info")
    return SeverityLevel::NOTICE;
  if(imp=="low")
    return SeverityLevel::INFO;
  if(imp=="medium")
    return SeverityLevel::PROBLEM;
  if(imp=="high")
    return SeverityLevel::CRITICAL;
  // by default report parse error
  throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, "Classification.Impact(severity)",
                                "invalid imput value: " + imp);
} // parseImpactValue()
} // unnamed namespace

FromXML::Assessment FromXML::parseAssessment(const xmlpp::Element &assessment) const
{
  ensureNode("Assessment", assessment);

  // try parsing severity
  const xmlpp::Element *severityElem=findOneChildIfHas(assessment, "Impact");
  SeverityLevel         sl(SeverityLevel::DEBUG);
  if(severityElem!=NULL)
    sl=parseImpactValue( parseParameter(*severityElem, "severity") );
  else
    LOGMSG_WARN(log_, "missing 'Impact' section, while severity is required in the system. "
                      "leaving DEBUG severity as the default for this report.");

  // try parsing certainty
  const xmlpp::Element *certaintyElem=findOneChildIfHas(assessment, "Confidence");
  double                cert         =0.1;
  if(certaintyElem!=NULL)
    cert=parseConfidenceValue( parseParameter(*certaintyElem, "rating"), *certaintyElem );
  else
    LOGMSG_WARN(log_, "missing 'Confidence' section, while certainty is required in the system. "
                      "leaving 10% confidence as the default");

  // ok - return parsed value
  return Assessment( Severity(sl), Certainty(cert) );
}

FromXML::Classification FromXML::parseClassification(const xmlpp::Element &classification) const
{
  ensureNode("Classification", classification);
  const string          description=parseParameter(classification, "text");
  const xmlpp::Element *reference  =findOneChildIfHas(classification, "Reference");
  ReferenceURLPtr       ref;
  if(reference!=NULL)
    ref=parseReferenceURL(*reference);
  return FromXML::Classification(description, ref);
}

Persistency::ReferenceURLPtrNN FromXML::parseReferenceURL(const xmlpp::Element &ref) const
{
  ensureNode("Reference", ref);
  const string name=parseString( findOneChild(ref, "name") );
  const string url =parseString( findOneChild(ref, "url") );
  return ReferenceURLPtrNN( new ReferenceURL(name, url) );
}

std::string FromXML::parseAdditionalData(const xmlpp::Element &data) const
{
  ensureNode("AdditionalData", data);
  const string type =parseParameter(data, "type");
  const string value=parseString( findOneChild(data, type.c_str() ) );
  if(type!="string")
    return type + ": " + value;
  return value;
}

FromXML::IP FromXML::parseAddress(const xmlpp::Element &address) const
{
  ensureNode("Address", address);
  const string ip=parseString( findOneChild(address, "address") );
  return FromXML::IP::from_string(ip);
}


// unnamed namespace for parser helpers
namespace
{
typedef vector<PortNumber::Numeric> PortList;

void parseSinglePort(PortList &out, const string &in)
{
  out.push_back( Convert::to<PortNumber::Numeric>(in) );
} // parseSinglePort()

void parsePortRange(PortList &out, const string &in)
{
  // tokenize
  typedef boost::char_separator<char> Separator;
  typedef boost::tokenizer<Separator> Tokenizer;
  const Separator sep("-");
  const Tokenizer tokens(in, sep);

  // parse tokens
  PortNumber::Numeric nums[2];
  int                 i=0;
  for(Tokenizer::const_iterator it=tokens.begin(); it!=tokens.end(); ++it, ++i)
  {
    if(i==2)
      throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, "portlist", "port range is invalid (too many tokens): " + in);
    nums[i]=Convert::to<PortNumber::Numeric>(*it);
  }
  // check order
  const PortNumber::Numeric from=nums[0];
  const PortNumber::Numeric to  =nums[1];
  if(from>to)
    throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, "portlist", "port range is invalid (reverse order): " + in);

  // add all ports in range to the collection
  for(PortNumber::Numeric i=from; i<=to; ++i)
    out.push_back(i);
} // parsePortRange()

void parsePortList(PortList &out, const string &in)
{
  typedef boost::char_separator<char> Separator;
  typedef boost::tokenizer<Separator> Tokenizer;
  const Separator sep(" ");
  const Tokenizer tokens(in, sep);
  // got through all tokens
  for(Tokenizer::const_iterator it=tokens.begin(); it!=tokens.end(); ++it)
  {
    if( strstr( it->c_str(), "-" )!=NULL )
      parsePortRange(out, *it);
    else
      parseSinglePort(out, *it);
  }
} // parsePortList()
} // unnamed namespace

FromXML::ServiceVector FromXML::parseService(const xmlpp::Element &service) const
{
  ensureNode("Service", service);

  // get name, if available
  const xmlpp::Element *nameElem=findOneChildIfHas(service, "name");
  const string name=(nameElem!=NULL)?( parseString(*nameElem).c_str() ):"unknown";

  // read all ports
  PortList portList;
  const xmlpp::Element *portListElem=findOneChildIfHas(service, "portlist");
  if(portListElem==NULL)
    parseSinglePort(portList, parseString( findOneChild(service, "port") ) );
  else
    parsePortList(portList, parseString(*portListElem) );

  // found anything?
  if( portList.size()==0 )
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, service.get_path(), "port");

  // produce output
  const Service::Protocol protocol(NULL);
  const ReferenceURLPtr   nullRef;
  ServiceVector out( ServicePtrNN( new Service(name, portList[0], protocol, nullRef) ) );
  // if multiple ports are specified, make multiple services
  for(PortList::const_iterator it=portList.begin()+1 /* 0 is already added */; it!=portList.end(); ++it)
    out.push_back( ServicePtrNN( new Service(name, *it, protocol, nullRef) ) );

  // done
  return out;
}


namespace
{
NullValue<string> parseAndMergeArguments(const xmlpp::Element &parent)
{
  const xmlpp::Element::NodeList nl=parent.get_children("arg");
  // if no such elements, no arguments are present
  if( nl.size()==0 )
    return NullValue<string>();
  // merge all arguments
  stringstream  ss;
  const char   *sep="";
  for(xmlpp::Element::NodeList::const_iterator it=nl.begin(); it!=nl.end(); ++it)
  {
    assert(*it!=NULL);
    ss<<sep<<parseString( dynamic_cast<const xmlpp::Element&>(**it) );
    sep=" ";
  }
  // return merged result
  return NullValue<string>( ss.str() );
}
} // unnamed namespace

Persistency::ProcessPtrNN FromXML::parseProcessAndUser(const xmlpp::Element &process) const
{
  // check for process
  const xmlpp::Element *procElem=findOneChildIfHas(process, "Process");
  if(procElem==NULL)
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, process.get_path(), "Process");

  // check for user
  const xmlpp::Element *userRootElem=findOneChildIfHas(process, "User");
  if(userRootElem==NULL)
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, process.get_path(), "User");
  const xmlpp::Element *userElem=findOneChildIfHas(*userRootElem, "UserId");
  if(userElem==NULL)
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, userRootElem->get_path(), "UserId");

  // prepare some elements
  const NullValue<string> path    =parseChildIfHasAs<string>(*procElem, "path");
  const Process::Name     name    =parseString( findOneChild(*procElem, "name") );
  const NullValue<pid_t>  pid     =parseChildIfHasAs<pid_t>(*procElem, "pid");
  const NullValue<string> args    =parseAndMergeArguments(*procElem);
  const NullValue<int>    uid     =parseChildIfHasAs<int>(*userElem, "number");
  const Process::Username username=parseString( findOneChild(*userElem, "name") );
  const ReferenceURLPtr   ref;    // this is ACARM-ng's extension

  // return parsed object
  const char *argsStr=( args.get()==NULL )?NULL:args.get()->c_str();
  const char *pathStr=( path.get()==NULL )?NULL:path.get()->c_str();
  return ProcessPtrNN( new Process(pathStr, name, NULL, pid.get(), uid.get(), username, argsStr, ref) );
}

Persistency::HostPtrNN FromXML::parseSource(const xmlpp::Element &source) const
{
  return parseHost("Source", source);
}

Persistency::HostPtrNN FromXML::parseTarget(const xmlpp::Element &target) const
{
  return parseHost("Target", target);
}

FromXML::NodeData FromXML::parseNode(const xmlpp::Element &node) const
{
  ensureNode("Node", node);

  // parse name, if present
  StringNull name;
  const xmlpp::Element *nameElem=findOneChildIfHas(node, "name");
  if(nameElem!=NULL)
    name=StringNull( parseString(*nameElem) );
  // parse IP, if present
  IPNull ip;
  const xmlpp::Element *ipElem=findOneChildIfHas(node, "Address");
  if(ipElem!=NULL)
    ip=IPNull( parseAddress(*ipElem) );

  return NodeData(name, ip);
}

void FromXML::ensureNode(const char *name, const xmlpp::Element &node) const
{
  assert(name!=NULL);
  LOGMSG_DEBUG_S(log_)<<"processing node '"<<name<<"'";
  if( node.get_name()!=name )
    throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, node.get_path(),
                                  "unexpected name for " + string(name) );
  LOGMSG_DEBUG_S(log_)<<"node '"<<name<<"' is valid as a given parser element";
}

Persistency::Timestamp FromXML::parseTimestamp(const char *name, const xmlpp::Element &ts) const
{
  ensureNode(name, ts);
  assert(name!=NULL);
  // get string to convert
  const xmlpp::TextNode *txt=ts.get_child_text();
  if(txt==NULL)
    throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, ts.get_path(), "node is not text node");
  // convert to Timestamp
  TimeConverter tc;
  return tc.fromString( txt->get_content() ).first;
}

double FromXML::parseConfidenceValue(const std::string &rating, const xmlpp::Element &node) const
{
  // floating-point value?
  if(rating=="numeric")
    return Convert::to<double>( parseString(node) );
  // discrete value?
  if(rating=="low")
    return 0.3;
  if(rating=="medium")
    return 0.6;
  if(rating=="high")
    return 1;
  throw ExceptionInvalidElement(SYSTEM_SAVE_LOCATION, "Classification.Confidence(rating)",
                                "invalid value: " + rating);
}

Persistency::HostPtrNN FromXML::parseHost(const char *type, const xmlpp::Element &host) const
{
  assert(type!=NULL);
  ensureNode(type, host);

  // get node data (ip/name)
  const NodeData  nodeData=parseNode( findOneChild(host, "Node") );
  const char     *name    =( nodeData.first.get()==NULL ) ? NULL : nodeData.first->c_str();
  // IP is obligatory here
  if( nodeData.second.get()==NULL )
    throw ExceptionMissingElement(SYSTEM_SAVE_LOCATION, findOneChild(host, "Node").get_path(), "Address");

  // get and copy reported services
  Host::ReportedServices  rSrvs;
  const xmlpp::Element   *srvNode=findOneChildIfHas(host, "Service");
  if(srvNode!=NULL)
  {
    const ServiceVector                          tmp=parseService(*srvNode);
    std::insert_iterator<Host::ReportedServices> insertIt( rSrvs, rSrvs.begin() );
    std::copy( tmp.begin(), tmp.end(), insertIt );
  } // if(srvNode!=NULL)

  // get reported process
  Host::ReportedProcesses rProcs;
  if( findOneChildIfHas(host, "Process")!=NULL && findOneChildIfHas(host, "User")!=NULL )
    rProcs.push_back( parseProcessAndUser(host) );

  // return new object
  assert( nodeData.second.get()!=NULL );
  return Persistency::HostPtrNN(
      new Host( *nodeData.second.get(), NULL, NULL, ReferenceURLPtr(), rSrvs, rProcs, name ) );
}

} // namespace RFCIO