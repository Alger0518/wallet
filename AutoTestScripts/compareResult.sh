#!/bin/bash

export mysqlUser=root
export mysqlPwd=123456
export mysqlHost=192.168.199.217
export mysqlDB=walletdb2
export mySQL_path=`which mysql`
export resetScanPos=516821
export resetScanEndPos=516821
export resetCoin=bch,ltc

export btcCli=/root/wallet_client/bitcoin-0.15.1/bin/bitcoin-cli
export bchCli=/root/wallet_client/bitcoin-abc-0.16.2/bin/bitcoin-cli
export ltcCli=/root/wallet_client/litecoin-0.13.2/bin/litecoin-cli

export rpcuser=bitstar
export rpcpassword=bitstar@2018
export bchport=7332
export ltcport=9332
#!/usr/bin/expect -f

set var [lindex $argv 0]

spawn scp -r root@192.168.199.217:/opt/$var'_SQL_result' /usr/local/wallet/reset/
expect "*password:" { send "123456\n" }
expect eof

spawn ssh root@192.168.199.217 "rm -rf /opt/$var'_SQL_result'"
expect "*password:" { send "123456\n" }
expect eof
#!/bin/bash

source ./config.sh

#function getResutFromSQL()
#{
	$mySQL_path -h$mysqlHost -u$mysqlUser -p$mysqlPwd $mysqlDB -e"
	select txid,amount,address,vout from $1_deposit into outfile '/opt/$1_SQL_result'"
#}

#arr=$(echo $resetCoin|tr "," "\n")

#for line in ${arr[@]}
#do
#	getResutFromSQL $line
#	./cpResult.sh $line
#done

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
#!/bin/bash

source ./config.sh

function reset()
{
	num=`ps -ef|grep "ScanDeposit --name=$1"|grep -v grep|grep -v PPID|awk '{ print $2}'|wc -l`
	if [ $num == 1 ];
	then
		processId=`ps -ef|grep "ScanDeposit --name=$1"|grep -v grep|grep -v PPID|awk '{ print $2}'`
		kill -9 $processId
	fi

	echo $resetScanPos > /usr/local/wallet/deposit/$1history.data

	mySQL_path=`which mysql`
	$mySQL_path -h$mysqlHost -u$mysqlUser -p$mysqlPwd -e"
	SET FOREIGN_KEY_CHECKS=0;
	use $mysqlDB;
	delete from $1_deposit; 
	update $1_address_pool set uid=0,used=0;
	SET FOREIGN_KEY_CHECKS=1;"

	cd /usr/local/wallet/deposit/

	datetime=`date +%Y%m%d%H%M%S`
	screen_name=$1$datetime
	screen -dmS $screen_name

	screen -x -S $screen_name -p 0 -X stuff "./ScanDeposit --name=$1 -daemon=0 -consolelog=1>>$1.log" 
	screen -x -S $screen_name -p 0 -X stuff "\n" 
	
	echo "compelete!"
}

arr=$(echo $resetCoin|tr "," "\n")

for line in ${arr[@]}
do
  reset $line
done
�据
	./cpResult.sh $line       ###将数据从数据库环境拷贝到本地
	isFileExist $line
        if [ $? == 1 ];
        then
		((successNum++));
		arrSuccess[successNum]=$line
		compareNum $line
		compareContent $line $?	
	else
		echo "$line need more files !!!"
	fi
done

for lineSuc in ${arrSuccess[@]}
do
	printResult $lineSuc
	mvFile $lineSuc
	rm -rf *wallet*
	rm -rf *result*
done
