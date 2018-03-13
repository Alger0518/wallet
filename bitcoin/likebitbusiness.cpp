#include "likebitbusiness.h"
#include <argnamedefine.h>
#include <coinmanager.h>
#include <appargs.h>
#include "dbtablename.h"
#include "activemqproxy.h"
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <jsonrpccpp/client/rpcprotocolclient.h>
#include <blocktracer.h>

LikeBitcoin* LikeBitBusiness::pBitCoin = nullptr;

void OnBeforeSendRpcMessage(jsonrpc::HttpClient* Client,struct curl_slist* Header,const std::string& Msg)
{
    static std::string RpcIp;
    static std::string RpcUser;
    static std::string RpcPwd;
    if(RpcIp.empty())
    {
        const bitstar::common::AppArgs& args= bitstar::common::AppArgs::GetGloablArgs();
        RpcIp = args.GetArgs(ArgNameDefine::RpcIP,"127.0.0.1");
        RpcUser = args.GetArgs("-rpcuser","wangyi");
        RpcPwd = args.GetArgs("-rpcpwd","123456");
    }
    Header = curl_slist_append(Header, ("Host: "+ RpcIp).c_str());
    Header = curl_slist_append(Header, "Connection: close");
    Header = curl_slist_append(Header, ("Authorization: "+ std::string("Basic ") + bitstar::common::Converter::EncodeBase64(RpcUser + ":" + RpcPwd)).c_str());
}

bool InitialCoinLoader(void* pPara)
{
    bitstar::common::LogProxy::info("Eneter liblikebitcoin::InitialCoinLoader....");
    if(!CoinDepositBusinessBase::ProductNameCheck("LikeBitBusiness::InitialCoinLoader",PRODUCT_NAME))
    {
        bitstar::common::LogProxy::fatal("CoinDepositBusinessBase::ProductNameCheck False...");
        return false;
    }
    const bitstar::common::AppArgs& args= bitstar::common::AppArgs::GetGloablArgs();
    const int BitCoinID = args.GetArgs(ArgNameDefine::CoinID,1);
    const std::string& CoinName = args.GetArgs(ArgNameDefine::CoinName,"");
    const std::string CoinCaption = args.GetArgs(ArgNameDefine::CoinEngine,"");
    std::string rpcUrl = args.GetArgs(ArgNameDefine::RpcUrl,"127.0.0.1:8545");
    if(args.Exist(ArgNameDefine::RpcIP)&&args.Exist(ArgNameDefine::RpcPort))
    {
        rpcUrl = args.GetArgs(ArgNameDefine::RpcIP,"127.0.0.1")+":"+args.GetArgs(ArgNameDefine::RpcPort,"8545");
    }
    LikeBitBusiness::pBitCoin = new LikeBitcoin(BitCoinID,CoinName,CoinCaption,rpcUrl);
    if(nullptr != LikeBitBusiness::pBitCoin)
    {
           new LikeBitBusiness(LikeBitBusiness::pBitCoin);
    }
    CoinManager::singleInstance.addCoinRpcFunction(LikeBitBusiness::pBitCoin);
    jsonrpc::HttpClient::OnBeforeSendRpcMessage = OnBeforeSendRpcMessage;
    jsonrpc::RpcProtocolClient::EnableRpc20Check = false;
    return CoinManager::singleInstance.getCoinIDMap().size()>0;
}

LikeBitBusiness::LikeBitBusiness(ICoinRpcFunction* pCoinDeposit):CoinDepositBusinessBase(pCoinDeposit)
{

}
int LikeBitBusiness::doScanBlcokDeposit(const long BlockIndex, CMySqlHelper& helper,BlockTracer& blockTracer)const
{
    const std::vector<TransactionContext> Trans = this->getBlockTransactionContext(BlockIndex);
    if(Trans.size() ==0)
    {
        //bitstar::common::LogProxy::infof("LikeBitBusiness::doScanBlcokDeposit Trans is 0...");
        blockTracer.Add(BlockIndex,"","");
        return 0;
    }
    blockTracer.Add(BlockIndex,"","");
    for(int i=0;i<Trans.size();i++)
    {
        const TransactionContext& Context = Trans[i];
        std::string UserID = this->doIsToAddrInPool(Context,helper);
        if(UserID.empty())
        {
            bitstar::common::LogProxy::errorf("UserID is empty!continue...");
            continue;
        }
        if(!this->doIsTransactionInDeposit(Context,helper,pCoinDeposit))
        {
            this->doInsertDeposit(Context,helper,UserID);
        }
    }
    return 1;
}
std::vector<TransactionContext> LikeBitBusiness::getBlockTransactionContext(const long BlockIndex) const
{
    std::string hash;
    Json::Value tx;
    std::vector<TransactionContext> contexts;
    if(pBitCoin->GetBlockHash(BlockIndex, hash))
    {
        Json::Value block_tx;
        if(!pBitCoin->GetBlock(&block_tx, hash))
        {
            bitstar::common::LogProxy::errorf("LikeBitBusiness::getBlockTransactionContext GetBlock error sleep for 60s");
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        for(auto t=0;t<block_tx["tx"].size();t++)
        {
            std::string transactionId = block_tx["tx"][t].asCString();
            Json::Value transaction = pBitCoin->GetTransaction(transactionId);
            if(transaction.isNull() || RpcCall::IsJsonContainsException(transaction))
            {
                continue;
            }
            if(transaction["confirmations"].asInt() < transableConfirmation)
            {
                bitstar::common::LogProxy::warnf("LikeBitBusiness::getBlockTransactionContext confirmations=%d < transableConfirmation=%d, continue...",transaction["confirmations"].asInt(),transableConfirmation);
                continue;
            }
            for(auto i = 0; i < transaction["details"].size(); i++)
            {
                Json::Value details = transaction["details"][i];
                const std::string category = details["category"].asCString();
                if(category == "receive")
                {
                    const std::string account = details["account"].asCString();
                    if(account == "BitStarChange")
                    {
                        bitstar::common::LogProxy::warnf("LikeBitBusiness::getBlockTransactionContext account == BitStarChange, continue...");
                        continue;
                    }
                    if(details["address"].isNull()) // 取交易数据"to"项的数据，其为充值支付地址
                    {
                        const std::string& strTransaction = RpcCall::Json2Str(details);
                        bitstar::common::LogProxy::warnf("Transaction:%s Content Success,but no to address, run continue...",strTransaction.c_str());
                        continue;
                    }
                    TransactionContext context;
                    if(pBitCoin->GetTransactionContext(details,context))
                    {
                        context.TransHash = transactionId;
                        context.Blocktime = transaction["blocktime"].asInt();
                        context.TransJson = transaction;
                        contexts.push_back(context);
                    }
                }
            }
        }
    }
    else
    {
        bitstar::common::LogProxy::errorf("LikeBitBusiness::getBlockTransactionContext GetBlockHash error BlockIndex=%d",BlockIndex);
    }
    return contexts;
}
std::string LikeBitBusiness::doIsToAddrInPool(const TransactionContext& Context, CMySqlHelper& helper)const
{
    if(CoinDepositBusinessBase::IsSystemAccount(Context.FromAddr))
    {
        return ArgNameDefine::EmptyStr;
    }

    const std::string adddr_pool_table_name = DBTableName::GetAddressTableName(Context.CoinName);

    const std::string UserIDStr = helper.getValue(adddr_pool_table_name, "uid", "where address='" + Context.ToAddr + "' and used=1");
    if("" == UserIDStr)
    {
        bitstar::common::LogProxy::errorf("address = %s is not exist in the pool",Context.ToAddr.c_str());
        return ArgNameDefine::EmptyStr;;
    }
    return UserIDStr;
}
int LikeBitBusiness::doIsTransactionInDeposit(const TransactionContext& Context, CMySqlHelper& helper,ICoinRpcFunction* pCoinRpc)const
{
    std::vector<std::string> columns{"id"};

    const std::string deposit_table_name = DBTableName::GetDepositTableName(Context.CoinName);

    std::string where = "where confirmstate<2 and txid='" + Context.TransHash + "' and vout=" + std::to_string(Context.Vout);
    DataTable dt = helper.select(deposit_table_name, columns, where);
    if(dt.size() > 0)
    {
        bitstar::common::LogProxy::warnf("the transaction is in the deposit, txid=%s,vout=%d",(Context.TransHash).c_str(),Context.Vout);
        return 1;
    }
    return 0;
}
int LikeBitBusiness::doInsertDeposit(const TransactionContext& Context, CMySqlHelper& helper,const std::string& UserIDStr)const
{
    std::vector<std::string> columns = {"txid", "vout", "address", "uid", "amount", "confirmstate"};
    std::vector<std::string> values;
    const std::string deposit_table_name = DBTableName::GetDepositTableName(Context.CoinName);

    values.push_back("'" + Context.TransHash + "'");
    values.push_back(std::to_string(Context.Vout));
    values.push_back("'" + Context.ToAddr + "'");
    values.push_back("'" + UserIDStr + "'");
    values.push_back(std::to_string(Context.DBBalance));
    values.push_back("0");

    //update mysql
    if(helper.insert(deposit_table_name, columns, values) > 0)
    {
        bitstar::common::LogProxy::infof("insert sucessed: uid=%s, address=%s, vout=%d, amount=%f",UserIDStr.c_str(),Context.ToAddr.c_str(),Context.Vout,Context.DBBalance);
    }
    else
    {
        bitstar::common::LogProxy::errorf("insert failed: uid=%s, address=%s, vout=%d, amount=%f",UserIDStr.c_str(),Context.ToAddr.c_str(),Context.Vout,Context.DBBalance);
        return -1;
    }

    const std::string DepostitID = helper.getValue(deposit_table_name, "id", "where txid='" + Context.TransHash + "' and vout=" + std::to_string(Context.Vout));
    if("" == DepostitID)
    {
        bitstar::common::LogProxy::errorf("id is not exist in the deposit table, txid=%s, vout=%d.",Context.TransHash.c_str(),Context.Vout);
        return -2;
    }

    const long blocktime = time(NULL);
#ifndef REMOVE_MQ
    if(!ActiveMQProxy::singleInstance.sendDepositMessage(this->pCoinDeposit->getCoinType(),
                                                                  0,
                                                                  bitstar::common::Converter::atoi32(UserIDStr),
                                                                  bitstar::common::Converter::atoi32(DepostitID),
                                                                  Context.DBBalance,
                                                                  blocktime,
                                                                  Context.ToAddr))
     {
        bitstar::common::LogProxy::errorf("Send Deposit Mq Message Failed,uid=%s,txid=%s,address=%s,amount=%f",UserIDStr.c_str(),Context.TransHash.c_str(),Context.ToAddr.c_str(),Context.DBBalance);
        return -3;
     }
#endif
  return 1;
}

TDBBalance LikeBitBusiness::GetWalletBalanceFromDB(CMySqlHelper& helper,const int CoinType)const
{
    Json::Value addr_balance;
    if(pBitCoin->ListAddressGroupings(addr_balance))
    {
        try
        {
            TDBBalance balance = 0;
            for(int i = 0; i < (int)addr_balance.size(); i++)
            {
                const Json::Value& group = addr_balance[i];
                for(int j = 0; j < (int)group.size(); j++)
                {
                    const Json::Value& item = group[j];
                    balance += item[1].asDouble();
                }
            }
            return balance;
        }
        catch(...)
        {
            return -3;
        }
    }
    else
    {
        return -2;
    }
}

const TStrVector LikeBitBusiness::getUnusedAddress(CMySqlHelper& helper,const int CoinType,const int UserID)
{
    static std::string tableName = DBTableName::GetAddressTableName(this->pCoinDeposit->getCoinName());
    static const std::vector<std::string> columns = {"id", "address"};
    DataTable dtExist = helper.select(tableName, columns, bitstar::common::StringFormat::strfmt("where uid=%d",UserID));
    if(dtExist.size()>0)
    {
        return dtExist[0];
    }
    DataTable dt = helper.select(tableName, columns, "where used=0 limit 1");
    if(dt.size() > 0)
    {
        return dt[0];
    }

    return EmptyStrVector;
}

TDBBalance LikeBitBusiness::GetWalletBalanceFromChain(CMySqlHelper& helper,const int CoinType,bool Update2DB)const
{
    TDBBalance total = GetWalletBalanceFromDB(helper, CoinType);
    return total > 0 ? total : 0;
}
