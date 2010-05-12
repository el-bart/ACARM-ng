/*
 * TestHelpers.t.cpp
 *
 */
#include "Persistency/MetaAlert.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Persistency/IO/Postgres/TestHelpers.t.hpp"
#include "Persistency/IO/Postgres/TransactionAPI.hpp"
#include "Persistency/IO/Postgres/detail/Appender.hpp"

using namespace std;
using namespace pqxx;

using namespace Persistency::IO::Postgres::detail;
using namespace Persistency;

namespace Persistency
{
namespace IO
{
namespace Postgres
{

IO::ConnectionPtrNN makeConnection(void)
{
  IO::BackendFactory::Options opts;
  opts["host"]  ="localhost";
  opts["port"]  ="5432";
  opts["dbname"]="acarm_ng_test";
  opts["user"]  ="acarm-ng-daemon";
  opts["pass"]  ="test.daemon";
  return IO::ConnectionPtrNN( Persistency::IO::BackendFactory::create("postgres", opts) );
}

DataBaseID getID(IO::Transaction &t, const std::string &name)
{
  DataBaseID id;
  const Persistency::MetaAlert::Name node(name);
  stringstream ss;
  ss << "SELECT * FROM meta_alerts WHERE name = ";
  Appender::append(ss, node.get());
  ss << ";";
  const result r = t.getAPI<TransactionAPI>().exec(ss);
  // TODO: SEGV when no result returned
  r[0]["id"].to(id);
  return id;
}
void removeNodeConnection(const std::string &parentName, const std::string &childName)
{
  Persistency::IO::ConnectionPtrNN conn( Persistency::IO::create() );
  IO::Transaction t( conn->createNewTransaction("delete_data_transaction") );

  stringstream ss;
  ss << "DELETE FROM meta_alerts_tree WHERE id_node = " << getID(t, parentName) << " AND id_child = " << getID(t, childName)<< ";";
  t.getAPI<TransactionAPI>().exec(ss);
  t.commit();
}

AlertPtr makeNewAlert(const char *name, const Timestamp &t)
{
  const Persistency::Alert::SourceAnalyzers sa( makeNewAnalyzer() );
  return AlertPtr( new Persistency::Alert(name,
                             sa,
                             NULL,
                             t,
                             Severity(SeverityLevel::INFO),
                             Certainty(0.42),
                             "some test allert",
                             Persistency::Alert::ReportedHosts(),
                             Persistency::Alert::ReportedHosts() ) );
}

MetaAlertPtr makeNewMetaAlert(const char *name, const Timestamp &t)
{
  return MetaAlertPtrNN( new Persistency::MetaAlert( Persistency::MetaAlert::Name(name),
                                        0.1, 0.2,
                                        makeNewReferenceURL(),
                                        t ) );
}


AnalyzerPtrNN makeNewAnalyzer(const char *name)
{
  return AnalyzerPtrNN( new Persistency::Analyzer(name, NULL, NULL, NULL) );
}

HostPtr makeNewHost(void)
{
  const Persistency::Host::Netmask_v4 mask(mask4_bytes);
  return makeNewHost4("1.2.3.4", &mask, "linux");
}

HostPtr makeNewHost4(const char             *ip,
                     const Persistency::Host::Netmask_v4 *mask,
                     const char             *os,
                     bool                    nullRef)
{
  return HostPtr( new Persistency::Host( Persistency::Host::IPv4::from_string(ip),
                            mask,
                            os,
                            (nullRef)?(ReferenceURLPtr()):(makeNewReferenceURL()),
                            Persistency::Host::ReportedServices(),
                            Persistency::Host::ReportedProcesses(),
                            "dns.org" ) );
}

HostPtr makeNewHost6(const char             *ip,
                     const Persistency::Host::Netmask_v6 *mask,
                     const char             *os,
                     bool                    nullRef)
{
  return HostPtr( new Persistency::Host( Persistency::Host::IPv6::from_string(ip),
                            mask,
                            os,
                            (nullRef)?(ReferenceURLPtr()):(makeNewReferenceURL()),
                            Persistency::Host::ReportedServices(),
                            Persistency::Host::ReportedProcesses(),
                            "dns.org" ) );
}

ProcessPtr makeNewProcess(const char *name)
{
  return ProcessPtr( new Persistency::Process( "/path/to/bin",
                                 name,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 ReferenceURLPtr() ) );
}

ReferenceURLPtr makeNewReferenceURL(const char *url)
{
  return ReferenceURLPtr( new Persistency::ReferenceURL("some name", url) );
}

GraphNodePtrNN makeNewLeaf(const char      *name,
                           const Timestamp &time)
{
  Persistency::IO::ConnectionPtrNN conn( Persistency::IO::create() );
  IO::Transaction t( conn->createNewTransaction("make_leaf_transaction") );
  GraphNodePtrNN graphNode( new Persistency::GraphNode( makeNewAlert(name, time), conn, t) );
  t.commit();
  return graphNode;
}

GraphNodePtrNN makeNewNode(void)
{
  return makeNewNode( makeNewLeaf("some name 1"), makeNewLeaf("some name 2"), "another meta alert name");
}


GraphNodePtrNN makeNewNode(GraphNodePtrNN child1,
                           GraphNodePtrNN child2,
                           const char *name,
                           const Timestamp &time)
{
  Persistency::IO::ConnectionPtrNN conn( Persistency::IO::create() );
  IO::Transaction t( conn->createNewTransaction("make_node_transaction") );
  const Persistency::NodeChildrenVector ncv(child1, child2);
  GraphNodePtrNN graphNode( new Persistency::GraphNode( makeNewMetaAlert(name, time),
                                                     conn, t, ncv) );
  t.commit();
  return graphNode;
}

GraphNodePtrNN makeNewTree1(void)
{
  return makeNewNode( makeNewNode(makeNewLeaf("leaf1"), makeNewLeaf("leaf2"), "node1" ),
                      makeNewNode(
                         makeNewNode(makeNewLeaf("leaf3"),
                                     makeNewLeaf("leaf4"),
                                     "node3"), makeNewLeaf("leaf5"), "node2"), "root" );
}

GraphNodePtrNN makeNewTree2(void)
{
  GraphNodePtrNN node1=makeNewNode();
  return makeNewNode( node1,
                      makeNewNode(
                        makeNewNode( makeNewLeaf(), node1 ), node1 ) );
}

Restorer::NodesVector makeNewTree3(void)
{
  Restorer::NodesVector vec;
  GraphNodePtrNN leaf1 = makeNewLeaf("leaf1");
  GraphNodePtrNN leaf2 = makeNewLeaf("leaf2");
  vec.push_back(leaf1);
  vec.push_back(leaf2);
  GraphNodePtrNN node1 = makeNewNode(leaf1,
                                     leaf2,
                                     "node1");
  vec.push_back(node1);

  GraphNodePtrNN leaf3 = makeNewLeaf("leaf3");
  GraphNodePtrNN leaf4 = makeNewLeaf("leaf4");
  vec.push_back(leaf3);
  vec.push_back(leaf4);
  GraphNodePtrNN node2 = makeNewNode(leaf3,
                                     leaf4,
                                     "node2");
  vec.push_back(node2);

  GraphNodePtrNN leaf5 = makeNewLeaf("leaf5");
  GraphNodePtrNN leaf6 = makeNewLeaf("leaf6");
  vec.push_back(leaf5);
  vec.push_back(leaf6);
  GraphNodePtrNN node3 = makeNewNode(leaf5,
                                     leaf6,
                                     "node3");

  vec.push_back(node3);
  GraphNodePtrNN root1 = makeNewNode(node1,
                                     node2,
                                     "root1");
  vec.push_back(root1);
  GraphNodePtrNN root2 = makeNewNode(node2,
                                     node3,
                                     "root2");
  vec.push_back(root2);
  return vec;
}

Persistency::GraphNodePtrNN makeNewTree4(void)
{
  return makeNewNode( makeNewNode(makeNewLeaf("leaf1"), makeNewLeaf("leaf2"), "node1" ),
                      makeNewNode(
                         makeNewNode(makeNewLeaf("leaf3"),
                                     makeNewLeaf("leaf4"),
                                     "node3"),
                         makeNewNode(makeNewLeaf("leaf5"),
                                     makeNewLeaf("leaf6"),
                                     "node4"), "node2"), "root" );
}

Restorer::NodesVector makeNewTree5(void)
{
  Restorer::NodesVector vec;
  GraphNodePtrNN leaf1 = makeNewLeaf("leaf1");
  GraphNodePtrNN leaf2 = makeNewLeaf("leaf2");
  vec.push_back(leaf1);
  vec.push_back(leaf2);
  GraphNodePtrNN node3 = makeNewNode(leaf1,
                                     leaf2,
                                     "node3");
  vec.push_back(node3);

  GraphNodePtrNN leaf3 = makeNewLeaf("leaf3");
  GraphNodePtrNN leaf4 = makeNewLeaf("leaf4");
  vec.push_back(leaf3);
  vec.push_back(leaf4);
  GraphNodePtrNN node4 = makeNewNode(leaf3,
                                     leaf4,
                                     "node4");
  vec.push_back(node4);

  GraphNodePtrNN leaf5 = makeNewLeaf("leaf5");
  GraphNodePtrNN leaf6 = makeNewLeaf("leaf6");
  vec.push_back(leaf5);
  vec.push_back(leaf6);
  GraphNodePtrNN node5 = makeNewNode(leaf5,
                                     leaf6,
                                     "node5");

  vec.push_back(node5);

  GraphNodePtrNN node1 = makeNewNode(node3,
                                     node4,
                                     "node1");
  vec.push_back(node1);

  GraphNodePtrNN node2 = makeNewNode(node4,
                                     node5,
                                     "node2");

  vec.push_back(node2);
  GraphNodePtrNN root = makeNewNode(node1,
                                     node2,
                                     "root");
  vec.push_back(root);
  return vec;
}

Restorer::NodesVector makeNewTree6(void)
{
  Restorer::NodesVector vec;
  GraphNodePtrNN leaf1 = makeNewLeaf("leaf1");
  GraphNodePtrNN leaf2 = makeNewLeaf("leaf2");

  GraphNodePtrNN node1 = makeNewNode(leaf1,
                                     leaf2,
                                     "node1");
  vec.push_back(leaf1);
  vec.push_back(leaf2);
  vec.push_back(node1);

  GraphNodePtrNN leaf3 = makeNewLeaf("leaf3");
  GraphNodePtrNN leaf4 = makeNewLeaf("leaf4");

  GraphNodePtrNN node2 = makeNewNode(leaf3,
                                     leaf4,
                                     "node2");
  vec.push_back(leaf3);
  vec.push_back(leaf4);
  vec.push_back(node2);
  return vec;
}

Restorer::NodesVector makeNewTree7(void)
{
  Restorer::NodesVector vec;
  GraphNodePtrNN leaf1 = makeNewLeaf("leaf1");
  GraphNodePtrNN leaf2 = makeNewLeaf("leaf2");

  GraphNodePtrNN node1 = makeNewNode(leaf1,
                                     leaf2,
                                     "node1");
  vec.push_back(leaf1);
  vec.push_back(leaf2);
  vec.push_back(node1);
  return vec;
}
} // namespace Postgres
} // namespace IO
} // namespace Persistency
