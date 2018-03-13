#!/bin/bash

source ./config.sh

datetime=`date +%Y%m%d%H%M%S`
var=$1'port'
port=`eval echo '$'"$var"`
var=$1'Cli'
cli=`eval echo '$'"$var"`

function processAddr()
{
	for line in $(cat $1_tmp2_$datetime)
	do 
		tmpStr=${line#*'"'}
		echo ${tmpStr%'"'*} >> $1_address_$datetime
	done
	rm -rf $1_tmp2_$datetime
}
function getTx()
{
	for line in $(seq $resetScanPos $resetScanEndPos)
	do
		blockhash=`$cli -rpcuser=$rpcuser -rpcpassword=$rpcpassword -rpcport=$port getblockhash $line`
		$cli -rpcuser=$rpcuser -rpcpassword=$rpcpassword -rpcport=$port getblock $blockhash>>$1_tmp_$datetime
		cat $1_tmp_$datetime |jq '.tx[]'>>$1_tmp2_$datetime
		rm -rf $1_tmp_$datetime
	done
}
function getTxDatil()
{
	for line in $(cat $1_address_$datetime)
	do
		$cli -rpcuser=$rpcuser -rpcpassword=$rpcpassword -rpcport=$port gettransaction $line true 1>$1_transaction_tmp 2>/dev/null
		if [ $? == 0 ];
		then
		        amount=`cat $1_transaction_tmp | jq '.amount'`
			txid=`cat $1_transaction_tmp | jq '.txid'`
			address=`cat $1_transaction_tmp | jq '.details[].address'`
			vout=`cat $1_transaction_tmp |jq '.details[].vout'`
			item=$txid'|'$amount'|'$address'|'$vout
			echo $item 1>>$1_wallet_data
		fi
	done
	rm -rf $1_transaction_tmp
	rm -rf $1_address_$datetime
}

getTx $1
processAddr $1
getTxDatil $1
