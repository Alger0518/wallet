#ifndef MYSQLHELPER_H
#define MYSQLHELPER_H

#include <mysql/mysql.h>
#include <vector>
#include <map>
#include <string>
//#include"../bitstarcommon/bitstarcommon_global.h"

typedef std::vector<std::string> DataRow;
typedef std::vector<std::string> DataCol;
typedef std::vector<DataRow> DataTable;
#define MySQL_SERVER_HAS_GONE_AWAY   2006

class CMySqlHelper
{
private:
    std::string user;
    std::string pswd;
    std::string host;
    std::string schema;
    int port;
    MYSQL m_Mysql;
    bool bConnected;
public:
    CMySqlHelper();
    CMySqlHelper(const std::string &username, const std::string &password, const std::string &hostip, const std::string &schemaname, int iport);
    ~CMySqlHelper();

    void setArgs(const std::string &username, const std::string &password, const std::string &hostip, const std::string &schemaname, int iport);

    DataTable select(const std::string &tablename, const std::vector<std::string>& columns, std::string condition="");
    bool isConnect()const {return bConnected;}
    bool connect();
    bool connect(const std::string &username, const std::string &password, const std::string &hostip, const std::string &schemaname, int iport);
    bool GetConnectState() { return bConnected; }

    int insert(const std::string &tablename, const std::vector<std::string>& columns, const std::vector<std::string>& data);

    int replace(const std::string &tablename, const std::vector<std::string>& columns, const std::vector<std::string>& data);
    int delete_sql(const std::string &tablename, std::string condition = "");
    int query(const std::string &querystr);
    std::string getValue(const std::string &tablename, const std::string &column, std::string condition = "");
    std::string getValueSql(const std::string& sql);
    bool isNeedReConnect() { return MySQL_SERVER_HAS_GONE_AWAY == mysql_errno(&m_Mysql); }
};

#endif // MYSQLHELPER_H
