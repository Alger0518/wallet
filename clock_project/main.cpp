#include <iostream>
#include <sys/time.h>
#include <unistd.h>
using namespace std;



class MyTimer
{
private:
    struct timeval m_start, m_end;
    double m_timeCost;
    std::string m_str;
public:
    MyTimer(const std::string& str):m_str(str), m_timeCost(0) {}
    ~MyTimer(){};
private:
    MyTimer(const MyTimer&);
    MyTimer& operator = (const MyTimer&);
public:
    void start(const std::string& str = "")
    {
        if(str.length())
            m_str = str;
        gettimeofday(&m_start, NULL);
    }
    void end()
    {
        gettimeofday(&m_end, NULL);
        m_timeCost = m_end.tv_sec - m_start.tv_sec + (m_end.tv_usec - m_start.tv_usec) / 1000000.0;
        cout.setf(ios::fixed);
        std::cout<<m_str<<" cost : "<<m_timeCost<<" sec."<<std::endl;
    }
    void reset()
    {
        m_timeCost = 0;
    }
};

int main()
{
    MyTimer t("forloop1");
    int j;
    t.start("123");
    sleep(1);
    for(int i =1;i<100000000;i++)
        j += i;
    t.end();
}
