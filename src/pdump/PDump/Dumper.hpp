/*
 * Dumper.hpp
 *
 */
#ifndef INCLUDE_PDUMP_DUMPER_HPP_FILE
#define INCLUDE_PDUMP_DUMPER_HPP_FILE

#include <iosfwd>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

#include "Plugins/Loader.hpp"
#include "Persistency/IO/Restorer.hpp"

namespace PDump
{

/** \brief class that reads nodes and saves them to files.
 *  \note for now only alerts are saved.
 */
class Dumper: private boost::noncopyable
{
public:
  /** \brief stats after writing alerts to directory.
   */
  struct Stats
  {
    /** \brief create stats object from given values.
     *  \param total    total number of processed elements.
     *  \param attempts elements that were ment to be written.
     *  \param writes   elements that have been acctually written.
     */
    Stats(size_t total, size_t attempts, size_t writes);

    /** \brief gets total numer of processed elements.
     *  \return count.
     */
    size_t getTotal(void) const;
    /** \brief gets numer to writes performed.
     *  \return count.
     */
    size_t getWrites(void) const;
    /** \brief gets number of elements that were ment to be written.
     *  \return count.
     */
    size_t getAttempts(void) const;

  private:
    size_t total_;
    size_t writes_;
    size_t attempts_;
  }; // struct Stats

  /** \brief typedef on nodes vector. */
  typedef Persistency::IO::Restorer::NodesVector NodesVector;

  /** \brief initialize object, with a proper output streams.
   *  \param out    stream for outputing normal events.
   *  \param errOut stream for outputing error events.
   */
  Dumper(std::ostream &out, std::ostream &errOut);

  /** \brief restores all meta-alerts in given time range.
   *  \param from     time to start dumping from.
   *  \param to       time to stop dumping at.
   *  \param nodesOut output collection.
   */
  void restoreBetween(const Persistency::Timestamp  from,
                      const Persistency::Timestamp  to,
                      NodesVector                  &nodesOut);

  /** \brief outputs nodes to a given directory.
   *  \param nodes  nodes to be saved as files.
   *  \param outDir output directory to use.
   *  \return statistics of operation.
   */
  Stats writeToDir(const NodesVector &nodes, const boost::filesystem::path &outDir);

private:
  std::ostream    &out_;
  std::ostream    &errOut_;
  Plugins::Loader  loader_;
}; // class Dumper

} // namespace PDump

#endif
