#ifndef SIMPLECONSUMER_H
#define SIMPLECONSUMER_H

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
using cms::MessageConsumer;
using activemq::transport::DefaultTransportListener;
using cms::ExceptionListener;
using cms::CMSException;

class SimpleConsumer : public ExceptionListener, public DefaultTransportListener
{
public:
    SimpleConsumer();
    virtual ~SimpleConsumer();
private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    bool useTopic;
    bool clientAck;
public:
    bool create(const std::string& brokerURI, const std::string& destURI, const std::string &username, const std::string &password, bool useTopic = false, bool clientAck = false);
    Message* receiveMq(int time_ms);
    void close();
private:
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

#endif // SIMPLECONSUMER_H
