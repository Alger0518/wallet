#ifndef LIKEBITBUSINESS_H
#define LIKEBITBUSINESS_H

#include <coindepositbusinessbase.h>
#include "likebitcoin.h"
#include "likeusdtcoin.h"
#include "converter.h"


extern "C" bool InitialCoinLoader(void* pPara);



class LikeBitBusiness:public CoinDepositBusinessBase
{
public:
    LikeBitBusiness(ICoinRpcFunction* pCoinDeposit);

    virtual int doScanBlcokDeposit(const long BlockIndex, CMySqlHelper& helper,BlockTracer& blockTracer)const override;

    virtual std::string doIsToAddrInPool(const TransactionContext& Context, CMySqlHelper& helper)const override;

    virtual int doInsertDeposit(const TransactionContext& Context, CMySqlHelper& helper,const std::string& UserIDStr)const override;

    virtual int doIsTransactionInDeposit(const TransactionContext& Context, CMySqlHelper& helper,ICoinRpcFunction* pCoinRpc)const override;

    virtual TDBBalance GetWalletBalanceFromDB(CMySqlHelper& helper,const int CoinType)const override;

    virtual const TStrVector getUnusedAddress(CMySqlHelper& helper,const int CoinType,const int UserID) override;

    virtual TDBBalance GetWalletBalanceFromChain(CMySqlHelper& helper,const int CoinType,bool Update2DB = false)const override;
private:
    friend bool InitialCoinLoader(void* pPara);
    static LikeBitcoin* pBitCoin;
    virtual std::vector<TransactionContext> getBlockTransactionContext(const long BlockIndex) const;
};

#endif // LIKEBITBUSINESS_H
