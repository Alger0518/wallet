#include "simpleconsumer.h"
using activemq::core::ActiveMQConnectionFactory;
using activemq::core::ActiveMQConnection;
using cms::DeliveryMode;

SimpleConsumer::SimpleConsumer() :
    connection(NULL), session(NULL), destination(NULL), consumer(NULL), useTopic(false), clientAck(false)
{}

SimpleConsumer::~SimpleConsumer()
{
    this->cleanup();
}

bool SimpleConsumer::create(const std::string& brokerURI, const std::string& destURI, const std::string &username, const std::string &password, bool useTopic, bool clientAck)
{
    try
    {
        // Create a ConnectionFactory
        std::unique_ptr<ActiveMQConnectionFactory> connFactory(new ActiveMQConnectionFactory(brokerURI, username, password));

        // Create a Connection
        connection = connFactory->createConnection();

        ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection);
        if (amqConnection != NULL)
        {
            amqConnection->addTransportListener(this);
        }
        connection->start();
        connection->setExceptionListener(this);
        // Create a Session
        if (clientAck)
        {
            session = connection->createSession(Session::CLIENT_ACKNOWLEDGE);
        }
        else
        {
            session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        // Create the destination (Topic or Queue)
        if (useTopic)
        {
            destination = session->createTopic(destURI);
        }
        else
        {
            destination = session->createQueue(destURI);
        }
        // Create a MessageConsumer from the Session to the Topic or Queue
        consumer = session->createConsumer(destination);
    }
    catch (CMSException& e)
    {
        e.printStackTrace();
        return false;
    }
    return true;
}

cms::Message *SimpleConsumer::receiveMq(int time_ms)
{
    if(0 == time_ms)
    {
        return consumer->receiveNoWait();
    }
    return consumer->receive(time_ms);
}

void SimpleConsumer::close()
{
    this->cleanup();
}

void SimpleConsumer::cleanup()
{

    //*************************************************
    // Always close destination, consumers and producers before
    // you destroy their sessions and connection.
    //*************************************************

    // Destroy resources.
    try{
        if( destination != NULL ) delete destination;
    }catch (CMSException& e) {}
    destination = NULL;

    try{
        if( consumer != NULL ) delete consumer;
    }catch (CMSException& e) {}
    consumer = NULL;

    // Close open resources.
    try{
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }catch (CMSException& e) {}

    // Now Destroy them
    try{
        if( session != NULL ) delete session;
    }catch (CMSException& e) {}
    session = NULL;

    try{
        if( connection != NULL ) delete connection;
    }catch (CMSException& e) {}
    connection = NULL;
}
