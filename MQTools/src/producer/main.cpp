#include <iostream>
#include <map>
#include "config.h"
#include "simpleproducer.h"
#include "simpleconsumer.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "mysqlhelper.h"


//#define PRODUCER 1
#define CONSUMER 1
//#define DEPOSIT 1
#define BALANCE 1
using namespace std;

enum DepositMsg
{
    RequestNewAddress = 1,
    ResponseDepositInfo=2,
    RequestWalletBalance=4
};

void sigdemo(int sig);
bool setMqParam(string& senduri, string &senddest, string& receiveuri, string& receivedest, string& mquser, string& mqpwd, string& sendbalancedest, string& receivebalancedest, const map<string, string> &config_map);
bool setOtherParam(int &messageCode, int &isLoop, int &coinId, int &loopStartPos, int &loopEndPos, const map<string, string> &config_map);
bool setMySqlParam(string& mysqluser, string& mysqlpwd, string& mysqlip, string& mysqldb, int& mysqlport, const map<string, string> &config_map);
int main()
{
    map<string, string> config_map;
    string configfile("");
    if("" == configfile)
        configfile = "./config/config.ini";
    if(!Config::ReadConfig(configfile, config_map))
    {
        cout<<"read config file error!"<<endl;
        Config::PrintConfig(config_map);
        return 2;
    }
    string sendURI, sendDest, receiveURI, receiveDest, mquser, mqpwd, sendbalancedest, receivebalancedest;
    if(!setMqParam(sendURI, sendDest, receiveURI, receiveDest, mquser, mqpwd, sendbalancedest, receivebalancedest, config_map))
        return 6;
    int messageCode, coinId, loopStartPos, loopEndPos, isLoop;
    if(!setOtherParam(messageCode, isLoop, coinId, loopStartPos, loopEndPos, config_map))
        return 7;
    string mysqlUser, mysqlPwd, mysqlIp, mysqlSchema;
    int mysqlPort;
    if(!setMySqlParam(mysqlUser, mysqlPwd, mysqlIp, mysqlSchema, mysqlPort, config_map))
        return 5;

#ifdef CONSUMER
    CMySqlHelper sqlHelper;
    sqlHelper.setArgs(mysqlUser,mysqlPwd,mysqlIp,mysqlSchema,mysqlPort);
    if(!sqlHelper.connect())
        exit(-1);
#endif

    activemq::library::ActiveMQCPP::initializeLibrary();
#ifdef PRODUCER
    SimpleProducer producer;

    if(RequestNewAddress == messageCode)
    {
        if(!producer.create(sendURI, sendDest+to_string(coinId), mquser, mqpwd))
        {
            return 1;
        }
    }
    else if(ResponseDepositInfo == messageCode)
    {
        if(!producer.create(sendURI, sendbalancedest+to_string(coinId), mquser, mqpwd))
        {
            return 1;
        }
    }
    else
    {
        std::cout<<"messageCode Error! messageCode = "<<messageCode<<std::endl;
        return -1;
    }

    if(isLoop)
    {
        for(auto i = loopStartPos ; i <= loopEndPos; i++)
            producer.sendTestMessage(coinId, i, messageCode);
    }
    else
    {
        string uid;
        do{
            std::cout<<"input uid! input eof end!"<<std::endl;
            getline(cin, uid);
            if("eof" != uid)
                producer.sendTestMessage(coinId, atoi(uid.c_str()), messageCode);
        }while("eof" != uid);
    }
    producer.close();
#endif
#ifdef CONSUMER
#ifdef DEPOSIT
    const string topic = receiveDest;
#endif
#ifdef BALANCE
    const string topic = receivebalancedest;
#endif

    SimpleConsumer consumer;

    if(!consumer.create(receiveURI, topic, mquser, mqpwd))
    {
        return 1;
    }
    while(true)
    {
        if(signal(SIGINT,sigdemo) == SIG_ERR)
        {
            return -1;
        }
        std::shared_ptr<cms::BytesMessage> pMsg = std::shared_ptr<cms::BytesMessage>(dynamic_cast<cms::BytesMessage*>(consumer.receiveMq(0)));

        if(nullptr == pMsg)
        {
            sleep(1);
            continue;
        }

        const int message = pMsg->readByte();
        if(RequestNewAddress == message)
        {
            const int coinType = pMsg->readByte();
            const int state = pMsg->readByte();
            const int uid = pMsg->readInt();
            const std::string address = pMsg->readString();

            std::vector<std::string> columns = {"messagecode", "cointype", "state", "uid", "address"};
            std::vector<std::string> values;
            const std::string deposit_table_name = "address_test";

            values.push_back(std::to_string(message));
            values.push_back(std::to_string(coinType));
            values.push_back(std::to_string(state));
            values.push_back(std::to_string(uid));
            values.push_back("'" + address + "'");

            if(sqlHelper.insert(deposit_table_name, columns, values) > 0)
            {
                std::cout<<"insert succeed! "<<" messagecode = "<<message<<" cointype = "<<coinType<<" state = "<<state<<" uid = "<<uid<<" address = "<<address<<std::endl;
            }
            else
            {
                std::cout<<"insert failed! "<<" messagecode = "<<message<<" cointype = "<<coinType<<" state = "<<state<<" uid = "<<uid<<" address = "<<address<<std::endl;
            }
        }
        else if(ResponseDepositInfo == message)
        {
            const int coinType = pMsg->readByte();
            const int state = pMsg->readByte();
            const int uid = pMsg->readInt();
            const int depositid = pMsg->readInt();
            const double amount = pMsg->readDouble();
            const int timestamp = pMsg->readLong();
            const std::string address = pMsg->readString();

            std::vector<std::string> columns = {"messagecode", "cointype", "state", "uid", "depositid", "amount", "address"};
            std::vector<std::string> values;
            const std::string deposit_table_name = "deposit_test";

            values.push_back(std::to_string(message));
            values.push_back(std::to_string(coinType));
            values.push_back(std::to_string(state));
            values.push_back(std::to_string(uid));
            values.push_back(std::to_string(depositid));
            values.push_back(std::to_string(amount));
            values.push_back("'" + address + "'");

            if(sqlHelper.insert(deposit_table_name, columns, values) > 0)
            {
                std::cout<<"insert succeed! "<<" messagecode = "<<message<<" cointype = "<<coinType<<" state = "<<state<<" uid = "<<uid<<" depositid = "<<depositid<<" amount = "<<amount<<" address = "<<address<<std::endl;
            }
            else
            {
                std::cout<<"insert failed! "<<" messagecode = "<<message<<" cointype = "<<coinType<<" state = "<<state<<" uid = "<<uid<<" depositid = "<<depositid<<" amount = "<<amount<<" address = "<<address<<std::endl;
            }
        }
        else if(RequestWalletBalance == message)
        {
            const int coinType = pMsg->readByte();
            const double amount = pMsg->readDouble();

            std::vector<std::string> columns = {"messagecode", "cointype", "amount"};
            std::vector<std::string> values;
            const std::string deposit_table_name = "balance_test";

            values.push_back(std::to_string(message));
            values.push_back(std::to_string(coinType));
            values.push_back(std::to_string(amount));

            if(sqlHelper.insert(deposit_table_name, columns, values) > 0)
            {
                std::cout<<"insert succeed! "<<" messagecode = "<<message<<" cointype = "<<coinType<<" amount = "<<amount<<std::endl;
            }
            else
            {
                std::cout<<"insert failed! "<<" messagecode = "<<message<<" cointype = "<<coinType<<" amount = "<<amount<<std::endl;
            }
        }
        else
        {
            std::cout<<"messageCode Error! = "<<message<<std::endl;
        }
        pMsg->acknowledge();
    }

    consumer.close();
#endif

    activemq::library::ActiveMQCPP::shutdownLibrary();

    return 0;
}
void sigdemo(int sig)
{
    std::cout<<"stopthReceiveMsgRouting!"<<std::endl;
    exit(-1);
}

bool setMqParam(string& senduri, string &senddest, string& receiveuri, string& receivedest, string& mquser, string& mqpwd, string& sendbalancedest, string& receivebalancedest, const map<string, string> &config_map)
{
    auto itor = config_map.find("senduri");
    if(itor != config_map.end())
        senduri = itor->second;
    else
        return false;
    itor = config_map.find("senddest");
    if(itor != config_map.end())
        senddest = itor->second;
    else
        return false;
    itor = config_map.find("receiveuri");
    if(itor != config_map.end())
        receiveuri = itor->second;
    else
        return false;
    itor = config_map.find("receivedest");
    if(itor != config_map.end())
        receivedest = itor->second;
    else
        return false;
    itor = config_map.find("mquser");
    if(itor != config_map.end())
        mquser = itor->second;
    else
        return false;
    itor = config_map.find("mqpwd");
    if(itor != config_map.end())
        mqpwd = itor->second;
    else
        return false;
    itor = config_map.find("sendbalancedest");
    if(itor != config_map.end())
        sendbalancedest = itor->second;
    else
        return false;
    itor = config_map.find("receivebalancedest");
    if(itor != config_map.end())
        receivebalancedest = itor->second;
    else
        return false;
    return true;
}
bool setOtherParam(int &messageCode, int &isLoop, int &coinId, int &loopStartPos, int &loopEndPos, const map<string, string> &config_map)
{
    auto itor = config_map.find("messageCode");
    if(itor != config_map.end())
        messageCode = stoi(itor->second);
    else
        return false;
    itor = config_map.find("isLoop");
    if(itor != config_map.end())
        isLoop = stoi(itor->second);
    else
        return false;
    itor = config_map.find("coinId");
    if(itor != config_map.end())
        coinId = stoi(itor->second);
    else
        return false;
    itor = config_map.find("loopStartPos");
    if(itor != config_map.end())
        loopStartPos = stoi(itor->second);
    else
        return false;
    itor = config_map.find("loopEndPos");
    if(itor != config_map.end())
        loopEndPos = stoi(itor->second);
    else
        return false;
    return true;
}
bool setMySqlParam(string& mysqluser, string& mysqlpwd, string& mysqlip, string& mysqldb, int& mysqlport, const map<string, string> &config_map)
{
    auto itor = config_map.find("mysqluser");
    if(itor != config_map.end())
        mysqluser = itor->second;
    else
        return false;
    itor = config_map.find("mysqlpwd");
    if(itor != config_map.end())
        mysqlpwd = itor->second;
    else
        return false;
    itor = config_map.find("mysqlip");
    if(itor != config_map.end())
        mysqlip = itor->second;
    else
        return false;
    itor = config_map.find("mysqldb");
    if(itor != config_map.end())
        mysqldb = itor->second;
    else
        return false;
    itor = config_map.find("mysqlport");
    if(itor != config_map.end())
        mysqlport = stoi(itor->second);
    else
        return false;
    return true;
}
