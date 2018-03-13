#include "mysqlhelper.h"

using namespace std;
CMySqlHelper::CMySqlHelper()
{
    user = "root";
    pswd = "123456";
    host = "localhost";
    schema = "walletdb";
    port = 3306;
    bConnected = false;
}

CMySqlHelper::CMySqlHelper(const string &username, const string &password, const string &hostip, const string &schemaname, int iport)
{
    setArgs(username,password,hostip,schemaname,iport);
}

void CMySqlHelper::setArgs(const string &username, const string &password, const string &hostip, const string &schemaname, int iport)
{
    user = username;
    pswd = password;
    host = hostip;
    schema = schemaname;
    port = iport;
    bConnected = false;
}
CMySqlHelper::~CMySqlHelper()
{
    mysql_close(&m_Mysql);
}

bool CMySqlHelper::connect()
{
    mysql_init(&m_Mysql);
    MYSQL* pMysql = mysql_real_connect(&m_Mysql, host.c_str(), user.c_str(), pswd.c_str(), schema.c_str(), port, NULL, 0);
    if (pMysql)
    {
        bConnected = true;
        return true;
    }
    else
    {
        bConnected = false;
        return false;
    }
}

bool CMySqlHelper::connect(const string &username, const string &password, const string &hostip, const string &schemaname, int iport)
{
    user = username;
    pswd = password;
    host = hostip;
    schema = schemaname;
    port = iport;
    bConnected = false;
    mysql_init(&m_Mysql);
    MYSQL* pMysql = mysql_real_connect(&m_Mysql, host.c_str(), user.c_str(), pswd.c_str(), schema.c_str(), port, NULL, 0);
    if (pMysql)
    {
        bConnected = true;
        return true;
    }
    else
    {
        bConnected = false;
        return false;
    }
}

DataTable CMySqlHelper::select(const string &tablename, const vector<std::string>& columns, string condition)
{
    static bool isReconnected = false;
    DataTable result_dt;

    if (!bConnected)
    {
        return result_dt;
    }

    string strQuery = "select ";
    size_t columns_num = columns.size();
    if (columns_num < 1)
    {
        return result_dt;
    }
    for (size_t i = 0; i < columns_num - 1; i++)
    {
        strQuery += columns[i] + ",";
    }
    strQuery = strQuery + columns[columns_num -1] + " from " + tablename;
    if (condition != "")
    {
        strQuery += " " + condition;
    }

    int res = mysql_query(&m_Mysql, strQuery.c_str());
    if (0 == res)
    {
        isReconnected = false;
        MYSQL_RES *result = nullptr;
        MYSQL_ROW sql_row;
        result = mysql_store_result(&m_Mysql);
        if (result)
        {
            while ((sql_row = mysql_fetch_row(result)) != nullptr)
            {
                vector<string> row;
                for (size_t i = 0; i < columns.size(); i++)
                {
                    row.push_back(sql_row[i]);
                }
                result_dt.push_back(row);
            }
            mysql_free_result(result);
        }
    }
    else
    {
        if((!isReconnected)&&isNeedReConnect())
        {
            isReconnected = true;
            mysql_close(&m_Mysql);
            connect();
            return select(tablename, columns, condition);
        }
    }
    return result_dt;
}

int CMySqlHelper::insert(const string &tablename, const vector<string>& columns, const vector<string>& data)
{
    if (!bConnected)
    {
        return -1;
    }

    if ((columns.size() != data.size()) || (columns.size() < 1))
    {
        return -2;
    }

    string query = "insert into " + tablename + "(" + columns[0];
    for (size_t i = 1; i < columns.size(); i++)
    {
        query += "," + columns[i];
    }
    query += ") values(" + data[0];
    for (size_t i = 1; i < data.size(); i++)
    {
        query += "," + data[i];
    }
    query += ")";
    int res = mysql_query(&m_Mysql, query.c_str());
    if (0 == res)
    {
        return (int)mysql_affected_rows(&m_Mysql);
    }
    else
    {
        return -res;
    }
}

int CMySqlHelper::replace(const string &tablename, const vector<string>& columns, const vector<string>& data)
{
    if (!bConnected)
    {
        return -1;
    }

    if ((columns.size() != data.size()) || (columns.size() < 1))
    {
        return -1;
    }
    string query = "replace into " + tablename + "(" + columns[0];
    for (size_t i = 1; i < columns.size(); i++)
    {
        query += "," + columns[i];
    }
    query += ") values(" + data[0];
    for (size_t i = 1; i < data.size(); i++)
    {
        query += "," + data[i];
    }
    query += ")";
    int res = mysql_query(&m_Mysql, query.c_str());
    if (0 == res)
    {
        return (int)mysql_affected_rows(&m_Mysql);
    }
    else
    {
        return res;
    }
}

int CMySqlHelper::query(const string &querystr)
{
    if (!bConnected)
    {
        return -1;
    }

    int res = mysql_query(&m_Mysql, querystr.c_str());

    if (0 == res)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

string CMySqlHelper::getValue(const string &tablename, const string &column, string condition)
{
    if (!bConnected)
    {
        return "";
    }
    string querystr = "select " + column + " from " + tablename;
    if (condition != "")
    {
        querystr += " " + condition;
    }

    return getValueSql(querystr);
}

string CMySqlHelper::getValueSql(const string& sql) {
    if (!bConnected)
    {
        return "";
    }

    string strValue("");
    int res = mysql_query(&m_Mysql, sql.c_str());
    if (0 == res)
    {
        MYSQL_RES *result = nullptr;
        MYSQL_ROW sql_row;
        result = mysql_store_result(&m_Mysql);
        if (result)
        {
            sql_row = mysql_fetch_row(result);
            if (sql_row)
            {
                if (*sql_row != nullptr)
                {
                    strValue = sql_row[0];
                }
            }
        }
    }
    return strValue;
}

int CMySqlHelper::delete_sql(const std::string &tablename, std::string condition)
{
    if (!bConnected)
    {
        return -1;
    }
    string query = "delete from " + tablename + " " + condition;
    int res = mysql_query(&m_Mysql, query.c_str());
    if (0 == res)
    {
        return (int)mysql_affected_rows(&m_Mysql);
    }
    else
    {
        return -res;
    }
}
