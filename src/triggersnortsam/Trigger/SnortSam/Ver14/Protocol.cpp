/*
 * Protocol.cpp
 *
 */
#include <ctime>
#include <boost/static_assert.hpp>
#include <cassert>

#include "Logger/Logger.hpp"
#include "Commons/Convert.hpp"
#include "Trigger/SnortSam/Ver14/TwoFish.hpp"
#include "Trigger/SnortSam/Ver14/Protocol.hpp"


namespace Trigger
{
namespace SnortSam
{
namespace Ver14
{

namespace
{
// version of this communuication protocol
static const int PROTO_VERSION  =14;

// trigger -> snort sam
static const int STATUS_CHECKIN =1;
static const int STATUS_CHECKOUT=2;
static const int STATUS_BLOCK   =3;
//static const int STATUS_UNBLOCK =9;

// snort sam -> trigger
static const int STATUS_OK      =4;
static const int STATUS_ERROR   =5;
static const int STATUS_NEWKEY  =6;
static const int STATUS_RESYNC  =7;
static const int STATUS_HOLD    =8;

// 'who' flags
static const int WHO_DESTINATION= 8;
static const int WHO_SOURCE     =16;

// 'how' flags
static const int HOW_IN         =32;
static const int HOW_OUT        =64;


const uint8_t *toBytes(const SamPacket *p)
{
  return static_cast<const uint8_t*>( static_cast<const void*>(p) );
} // toBytes()

const SamPacket *toPacket(const uint8_t *b)
{
  return static_cast<const SamPacket*>( static_cast<const void*>(b) );
} // toPacket()
} // unnamed namespace


Protocol::Protocol(const Who             who,
                   const How             how,
                   const unsigned int    duration,
                   const std::string    &key,
                   std::auto_ptr<NetIO>  netIO):
  who_(who),
  how_(how),
  duration_(duration),
  key_(key),
  verStr_( Commons::Convert::to<std::string>(PROTO_VERSION) ),
  netIO_( netIO.release() ),
  connected_(false),
  localSeqNo_(0),
  remoteSeqNo_(0),
  lastContact_(0),
  lastSessionKey_(key),
  encPacketSize_(0)
{
  // check if network is fine
  if(netIO_.get()==NULL)
    throw Exception(SYSTEM_SAVE_LOCATION, "network IO must be set");
}


const std::string &Protocol::getProtocolVersionImpl(void) const
{
  return verStr_;
}


bool Protocol::isConnectedImpl(void) const
{
  return connected_;
}


void Protocol::initImpl(void)
{
  // (re)init create required cryptography entries
  connected_=false;
  cryptoDefault_.reset( makeCrypto(key_).release() );
  cryptoStation_.reset();

  // randomize some sequence number
  do
  {
    localSeqNo_=rand();
  }
  while(localSeqNo_<20 || localSeqNo_>65500);

  // create some random key modifier
  for(size_t i=0; i<sizeof(localKeyMod_); ++i)
    localKeyMod_[i]=rand();

  // checkin to the remote agent
  checkIn();

  // we're connected now!
  connected_=true;
}


void Protocol::deinitImpl(void)
{
  // perform checkout protocol
  checkOut();

  // mark required fields
  connected_  =false;
  lastContact_=0;
  cryptoStation_.reset();
}


void Protocol::blockImpl(const Config::IP &/*from*/, const Config::IP &/*to*/)
{
  // TODO
}


std::auto_ptr<Crypto> Protocol::makeCrypto(const std::string &key) const
{
  std::auto_ptr<Crypto> ptr(new TwoFish(key));
  return ptr;
}

DataRef Protocol::encrypt(const uint8_t *data, size_t size)
{
  // use station entry
  if(cryptoStation_.get()!=NULL)
  {
    LOGMSG_DEBUG(log_, "encrypting using station key");
    return cryptoStation_->encrypt(data, size);
  }
  // fallback to deafult
  if(cryptoDefault_.get()!=NULL)
  {
    LOGMSG_DEBUG(log_, "encrypting using default key");
    return cryptoDefault_->encrypt(data, size);
  }
  // oops - error...
  throw Exception(SYSTEM_SAVE_LOCATION, "cryptography not set - cannot encrypt");
}


DataRef Protocol::decrypt(const uint8_t *data, size_t size)
{
  // use station entry
  if(cryptoStation_.get()!=NULL)
  {
    LOGMSG_DEBUG(log_, "trying decrypt using station key");
    DataRef tmp=cryptoStation_->decrypt(data, size);
    if(tmp.size()==sizeof(SamPacket))
      return tmp;
    LOGMSG_DEBUG(log_, "decrypting using station key failed");
  }
  // fallback to deafult
  if(cryptoDefault_.get()!=NULL)
  {
    LOGMSG_DEBUG(log_, "trying decrypt using default key");
    DataRef tmp=cryptoDefault_->decrypt(data, size);
    if(tmp.size()!=sizeof(SamPacket))
      throw Exception(SYSTEM_SAVE_LOCATION, "decryption failed");
    return tmp;
  }
  // oops - error...
  throw Exception(SYSTEM_SAVE_LOCATION, "cryptography not set - cannot decrypt");
}


void Protocol::send(const SamPacket &p)
{
  // encrypt data
  DataRef d=encrypt( toBytes(&p), sizeof(p) );
  encPacketSize_=d.size();
  // send it over a network channel
  assert(netIO_.get()!=NULL);
  netIO_->send(d.data(), d.size());
  // memorize fact of access to the remote side
  lastContact_=time(NULL);
}


SamPacket Protocol::receive(void)
{
  // receive data from remote host
  assert(netIO_.get()!=NULL);
  DataRef d=netIO_->receive(encPacketSize_);
  // decrypt data
  DataRef p=decrypt( d.data(), d.size() );
  // move to final destination
  SamPacket out;
  assert(p.size()==sizeof(out));
  memmove(&out, p.data(), sizeof(out));
  // memorize fact of access to the remote side
  lastContact_=time(NULL);
  // return received data
  return out;
}


void Protocol::checkIn(void)
{
  NetIO::ConnectionGuard cg(*netIO_);   // ensures disconnection, whatever the result will be
  sendCheckIn();                        // send message
  handleCheckInResponse();              // process result
}


void Protocol::sendCheckIn(void)
{
  LOGMSG_DEBUG(log_, "preparing checkin message");
  Message m;
  lastSessionKey_=key_;                         // reset previous key to known value
  m.p_.version_  =PROTO_VERSION;                // protocol version
  m.p_.status_   =STATUS_CHECKIN;               // message type
  m.setNum(m.p_.snortSeqNo_, localSeqNo_);      // local sequence number
  BOOST_STATIC_ASSERT(sizeof(localKeyMod_)==sizeof(m.p_.duration_));
  for(size_t i=0; i<sizeof(localKeyMod_); ++i)  // sends local key modifier in 'duration' filed (no comments on that nasty hack)
    m.p_.duration_[i]=localKeyMod_[i];
  // send the message
  LOGMSG_DEBUG(log_, "sending checkin message");
  send(m.p_);
  LOGMSG_DEBUG(log_, "checkin send - waiting for response");
}


void Protocol::handleCheckInResponse(void)
{
  const Message m( receive() );
  LOGMSG_DEBUG_S(log_)<<"got response; protocol version: "<<static_cast<int>(m.p_.version_)
                      <<" message type: "<<static_cast<int>(m.p_.status_);
  // basic checks
  if(m.p_.version_!=PROTO_VERSION)
    throw Exception(SYSTEM_SAVE_LOCATION, "unsupported protocol version detected");
  if(m.p_.status_!=STATUS_OK && m.p_.status_!=STATUS_NEWKEY && m.p_.status_!=STATUS_RESYNC)
    throw Exception(SYSTEM_SAVE_LOCATION, "unexpected message received");
  handleDirectResponse(m);
}


void Protocol::blockEntry(void)
{
  // TODO
}


void Protocol::sendBlockEntry(void)
{
  // TODO
}


void Protocol::handleBlockEntryResponse(void)
{
  // TODO
}


void Protocol::checkOut(void)
{
  NetIO::ConnectionGuard cg(*netIO_);   // ensures disconnection, whatever the result will be
  sendCheckOut();                       // send message
  handleCheckOutResponse();             // process result
}


void Protocol::sendCheckOut(void)
{
  LOGMSG_DEBUG(log_, "preparing checkout message");
  localSeqNo_+=remoteSeqNo_;
  Message m;
  m.p_.version_=PROTO_VERSION;
  m.p_.status_ =STATUS_CHECKOUT;
  m.setNum(m.p_.snortSeqNo_, localSeqNo_ );
  m.setNum(m.p_.fwSeqNo_,    remoteSeqNo_);
  // send the message
  LOGMSG_DEBUG(log_, "sending checkout message");
  send(m.p_);
  LOGMSG_DEBUG(log_, "checkout message send - waiting for response");
}


void Protocol::handleCheckOutResponse(void)
{
  const Message m( receive() );
  LOGMSG_DEBUG_S(log_)<<"got response; protocol version: "<<static_cast<int>(m.p_.version_)
                      <<" message type: "<<static_cast<int>(m.p_.status_);
  // basic checks
  if(m.p_.version_!=PROTO_VERSION)
    throw Exception(SYSTEM_SAVE_LOCATION, "unsupported protocol version detected");
  // handle response
  handleDirectResponse(m);
  LOGMSG_DEBUG(log_, "checkout confirmation received");
}


void Protocol::handleDirectResponse(const Message &m)
{
  // remember remote sequence number
  remoteSeqNo_=m.getNum(m.p_.fwSeqNo_);

  // handle known types of response packages
  switch(m.p_.status_)
  {
    case STATUS_OK:
      LOGMSG_DEBUG(log_, "got valid confirmation");
      break;

    case STATUS_ERROR:
      LOGMSG_ERROR(log_, "got error message from the agent");
      throw Exception(SYSTEM_SAVE_LOCATION, "error response erceived from the agent");
      break;

    case STATUS_RESYNC:
      resync(m);
      break;

    case STATUS_NEWKEY:
      resync(m);
      makeNewSessionKey(m);
      LOGMSG_DEBUG(log_, "new key created");
      break;

    default:
      LOGMSG_ERROR_S(log_)<<"got invalid direct response: "<<m.p_.status_;
      throw Exception(SYSTEM_SAVE_LOCATION, "invalid direct response received");
      break;
  } // switch(packet_type)

  LOGMSG_DEBUG(log_, "handling direct response finished");
}


void Protocol::resync(const Message &m)
{
  for(size_t i=0; i<sizeof(remoteKeyMod_); ++i)         // copy remote key modifiers
    remoteKeyMod_[i]=m.p_.duration_[i];
  LOGMSG_DEBUG(log_, "resunchronization compleated");
}


void Protocol::makeNewSessionKey(const Message &m)
{
  LOGMSG_DEBUG(log_, "making new session key");
  // note that this code is TwoFish-dependant, though makeCrypto() allows changing
  // of this algorithm. this is unfortunetely the only way to do this, since SnortSam
  // assumes it that way. :/
  // fortunetelly it is not a big limitation in real life, though looks a bit confusing.
  char newKey[TwoFish::KEY_LENGTH+1];
  assert( sizeof(newKey)>=8+1 && "array accesses will be out of bound");

  // use some data from remote service
  newKey[0]=m.p_.snortSeqNo_[0];
  newKey[1]=m.p_.snortSeqNo_[1];
  newKey[2]=m.p_.fwSeqNo_[0];
  newKey[3]=m.p_.fwSeqNo_[1];
  newKey[4]=m.p_.protocol_[0];
  newKey[5]=m.p_.protocol_[1];

  // append old key
  strncpy(newKey+6, lastSessionKey_.c_str(), TwoFish::KEY_LENGTH-6);
  newKey[TwoFish::KEY_LENGTH]=0;

  // mofidy with the data exchanged during check-in
  newKey[0]^=localKeyMod_[0];
  newKey[1]^=localKeyMod_[1];
  newKey[2]^=localKeyMod_[2];
  newKey[3]^=localKeyMod_[3];
  newKey[4]^=remoteKeyMod_[0];
  newKey[5]^=remoteKeyMod_[1];
  newKey[6]^=remoteKeyMod_[2];
  newKey[7]^=remoteKeyMod_[3];

  // ensure that string will not end up to soon
  for(int i=0; i<=7; ++i)
    if(newKey[i]==0)
      ++newKey[i];

  // create cryptographybased on new key and store it
  std::string newKeyStr(newKey);                            // make string out of it
  cryptoStation_.reset( makeCrypto(newKeyStr).release() );  // install new key
  lastSessionKey_.swap(newKeyStr);                          // save negociated key
  LOGMSG_DEBUG(log_, "new session key created");
}

} // namespace Ver14
} // namespace SnortSam
} // namespace Trigger
