#ifndef SIMPLEPRODUCER_H
#define SIMPLEPRODUCER_H

#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/ExceptionListener.h>
#include <memory>

using cms::BytesMessage;
using cms::Message;
using cms::Connection;
using cms::Session;
using cms::Destination;
using cms::MessageProducer;
using activemq::transport::DefaultTransportListener;
using cms::ExceptionListener;
using cms::CMSException;

typedef unsigned char byte;

class SimpleProducer : public ExceptionListener,  public DefaultTransportListener
{
private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    bool useTopic;
    bool clientAck;

private:

    SimpleProducer( const SimpleProducer& );
    SimpleProducer& operator= ( const SimpleProducer& );

public:

    SimpleProducer();

    virtual ~SimpleProducer();

public:
    void close();
    bool create(const std::string& brokerURI, const std::string& destURI, const std::string &username, const std::string &password);
    bool sendDepositMessage(byte cointype, byte state, int uid, int depositid, double amount, int timestamp, const std::string& address);
    bool sendTestMessage(byte cointype, int uid, int messageCode);
    bool sendAddressMessage(byte cointype, byte state, int uid, const std::string& address);
    bool sendBalanceMessage(byte cointype, double amount);
private:
    bool sendMessage(BytesMessage* byte_msg);
    void cleanup();

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException(const CMSException& ex AMQCPP_UNUSED)
    {
        //isConnect = false;
        //matching_logger->error("CMS Exception Occurred, shutting down the client. The Exception Detail Info: {0}", ex.getMessage());
        //matching_logger->flush();
        //exit(1);
    }

    virtual void onException(const decaf::lang::Exception& ex)
    {
        std::cout<<ex.what()<<std::endl;
        //isConnect = false;
        //matching_logger->error("Transport Exception occurred: {0}", ex.getMessage());
        //matching_logger->flush();
    }

    virtual void transportInterrupted()
    {
        //isConnect = false;
        //matching_logger->error("The Connection's Transport has been Interrupted.");
        //matching_logger->flush();
    }

    virtual void transportResumed()
    {
        //isConnect = true;
        //matching_logger->info("The Connection's Transport has been Restored.");
        //matching_logger->flush();
    }
};

#endif // SIMPLEPRODUCER_H
