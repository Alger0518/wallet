#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#define COMMENT_CHAR '#'

class Config
{
public:
    Config() = delete;
    static bool ReadConfig(const std::string & filename, std::map<std::string, std::string>& m);
    static void PrintConfig(const std::map<std::string, std::string> &m);
private:
    static bool IsSpace(char c);
    static bool IsCommentChar(char c);
    static void Trim(std::string & str);
    static bool AnalyseLine(const std::string & line, std::string & key, std::string & value);
};

#endif // CONFIG_H
