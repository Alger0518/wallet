#include "simpleproducer.h"
using activemq::core::ActiveMQConnectionFactory;
using activemq::core::ActiveMQConnection;
using cms::DeliveryMode;

SimpleProducer::SimpleProducer():
    connection(NULL),session(NULL),destination(NULL),producer(NULL),useTopic(false), clientAck(false)
{}

SimpleProducer::~SimpleProducer()
{
    cleanup();
}

void SimpleProducer::close()
{
    this->cleanup();
}

bool SimpleProducer::create(const std::string& brokerURI, const std::string& destURI, const std::string &username, const std::string &password)
{
    try
    {
        // Create a ConnectionFactory
        std::unique_ptr<ActiveMQConnectionFactory> connFactory(new ActiveMQConnectionFactory(brokerURI, username, password));

        // Create a Connection
        connection = connFactory->createConnection();

        // register TransportListener for the connection
        ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection);
        if (amqConnection != NULL)
        {
            amqConnection->addTransportListener(this);
        }
        connection->start();

        // register the ExceptionListener for the connection
        connection->setExceptionListener(this);

        // Create a Session
        session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

        // Create the destination for down message queue
        destination = session->createQueue(destURI);

        // Create a MessageProducer from the Session to the down message queue
        producer = session->createProducer(destination);
        producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);//set the persistent
    }
    catch(CMSException& ex)
    {
        //exception is happen when matching start
        //->error("Start Matching Error : {0}", ex.getMessage());
        //matching_logger->flush();
        return false;
    }
    return true;
}

bool SimpleProducer::sendDepositMessage(byte cointype, byte state, int uid, int depositid, double amount, int timestamp, const std::string &address)
{
    // send bytes message
    try
    {
        BytesMessage* message = session->createBytesMessage();
        message->writeByte(2);
        message->writeByte(cointype);
        message->writeByte(state);
        message->writeInt(uid);
        message->writeInt(depositid);
        message->writeDouble(amount);
        message->writeLong(timestamp);
        message->writeString(address);
        producer->send(message);
        delete message;
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool SimpleProducer::sendAddressMessage(byte cointype, byte state, int uid, const std::string &address)
{
    // send bytes message
    try
    {
        BytesMessage* message = session->createBytesMessage();
        message->writeByte(1);
        message->writeByte(cointype);
        message->writeByte(state);
        message->writeInt(uid);
        message->writeString(address);
        producer->send(message);
        delete message;
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool SimpleProducer::sendBalanceMessage(byte cointype, double amount)
{
    // send bytes message
    try
    {
        BytesMessage* message = session->createBytesMessage();
        message->writeByte(4);
        message->writeByte(cointype);
        message->writeDouble(amount);
        producer->send(message);
        delete message;
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool SimpleProducer::sendTestMessage(byte cointype, int uid, int messageCode)
{
    // send bytes message
    try
    {
        BytesMessage* message = session->createBytesMessage();
        message->writeByte(messageCode);
        message->writeByte(cointype);
        message->writeInt(uid);
        producer->send(message);
        delete message;
    }
    catch(...)
    {
        return false;
    }
    return true;
}

bool SimpleProducer::sendMessage(BytesMessage* byte_msg)
{
    producer->send(byte_msg);
    return true;
}

void SimpleProducer::cleanup()
{
    // Destroy resources.
    try
    {
        if( destination != NULL )
            delete destination;
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
    destination = NULL;

    try
    {
        if( producer != NULL )
            delete producer;
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
    producer = NULL;

    // Close open resources.
    try
    {
        if( session != NULL )
            session->close();
        if( connection != NULL )
            connection->close();
    }
    catch ( CMSException& e )
    { e.printStackTrace(); }

    try
    {
        if( session != NULL )
            delete session;
    }catch ( CMSException& e )
    { e.printStackTrace(); }
    session = NULL;

    try
    {
        if( connection != NULL )
            delete connection;
    }catch ( CMSException& e )
    { e.printStackTrace(); }
    connection = NULL;
}
