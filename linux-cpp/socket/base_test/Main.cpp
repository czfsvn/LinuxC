#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <arpa/inet.h>
#include <functional>
#include <algorithm>
#include <string.h>
#include <sstream>

using namespace std;

typedef unsigned long long  uint64;

/*
<gm ip="210.13.74.218" mask="255.255.255.255"/>
<gm ip="218.80.198.234" mask="255.255.255.255"/>
<gm ip="222.66.162.218" mask="255.255.255.255"/>
<gm ip="116.228.152.107" mask="255.255.255.255"/>
<gm ip="222.66.162.220" mask="255.255.255.255"/>
<gm ip="218.80.242.254" mask="255.255.255.255"/>
<gm ip="218.80.198.254" mask="255.255.255.255"/>
<gm ip="210.13.74.222" mask="255.255.255.255"/>
<gm ip="222.68.182.171" mask="255.255.255.255"/>
<gm ip="222.68.182.172" mask="255.255.255.255"/>
<gm ip="210.22.188.19" mask="255.255.255.255"/>
<gm ip="210.22.188.22" mask="255.255.255.255"/>
<gm ip="180.168.71.162" mask="255.255.255.255"/>
<gm ip="180.168.126.122" mask="255.255.255.255"/>
<gm ip="180.168.126.117" mask="255.255.255.255"/>
<gm ip="180.168.126.244" mask="255.255.255.255"/>
<gm ip="210.13.74.220" mask="255.255.255.255"/>
<gm ip="218.80.198.250" mask="255.255.255.255"/>
<gm ip="222.66.162.222" mask="255.255.255.255"/>

<gm ip="210.22.188.16" mask="255.255.255.248"/>
<gm ip="220.248.104.160" mask="255.255.255.248"/>
<gm ip="210.13.74.216" mask="255.255.255.248"/>

<gm ip="180.168.210.0" mask="255.255.255.240"/>
<gm ip="180.168.126.112" mask="255.255.255.240"/>
<gm ip="180.168.126.176" mask="255.255.255.240"/>
<gm ip="180.168.126.240" mask="255.255.255.240"/>
<gm ip="180.168.170.0" mask="255.255.255.240"/>
<gm ip="180.168.197.96" mask="255.255.255.240"/>
<gm ip="180.168.197.80" mask="255.255.255.240"/>
<gm ip="58.216.216.106" mask="255.255.255.240"/>
<gm ip="180.168.210.16" mask="255.255.255.240"/>
*/

std::map<std::string, std::string> ip_map = 
{
    {"210.13.74.218", "255.255.255.255"},
    {"218.80.198.234", "255.255.255.255"},
    {"222.66.162.218", "255.255.255.255"},
    {"116.228.152.107", "255.255.255.255"},
    {"222.66.162.220", "255.255.255.255"},
    {"218.80.242.254", "255.255.255.255"},
    {"218.80.198.254", "255.255.255.255"},
    {"210.13.74.222", "255.255.255.255"},
    {"222.68.182.171", "255.255.255.255"},
    {"222.68.182.172", "255.255.255.255"},
    {"210.22.188.19", "255.255.255.255"},
    {"210.22.188.22", "255.255.255.255"},
    {"180.168.71.162", "255.255.255.255"},
    {"180.168.126.122", "255.255.255.255"},
    {"180.168.126.117", "255.255.255.255"},
    {"180.168.126.244", "255.255.255.255"},
    {"210.13.74.220", "255.255.255.255"},
    {"218.80.198.250", "255.255.255.255"},
    {"222.66.162.222", "255.255.255.255"},

    {"210.22.188.16", "255.255.255.248"},
    {"220.248.104.160", "255.255.255.248"},
    {"210.13.74.216", "255.255.255.248"},

    {"180.168.210.0", "255.255.255.240"},
    {"180.168.126.112", "255.255.255.240"},
    {"180.168.126.176", "255.255.255.240"},
    {"180.168.126.240", "255.255.255.240"},
    {"180.168.170.0", "255.255.255.240"},
    {"180.168.197.96", "255.255.255.240"},
    {"180.168.197.80", "255.255.255.240"},
    {"58.216.216.106", "255.255.255.240"},
    {"180.168.210.16", "255.255.255.240"},

};


template<typename Target, typename Source>
Target cast(Source arg)
{
    Target target = Target();
    std::stringstream ss;
    ss << arg;
    ss >> target;
    return target;
}

template <> std::string cast<std::string, const char*>(const char* arg)
{
    return std::string(arg);
}

template <> std::string cast<std::string, char*>(char* arg)
{
    return std::string(arg);
}
template <> std::string cast<std::string, const std::string>(const std::string arg)
{
    return arg;
}
template <> std::string cast<std::string, std::string>(std::string arg)
{
    return arg;
}
template <> std::string cast<std::string, const std::string&>(const std::string& arg)
{
    return arg;
}

template<typename T>
void getAllNum(const char* s, std::vector<T>& data)
{
    data.clear();
    if (s == nullptr)
        return;

    bool preIsD = false;
    for (size_t i = 0; i < strlen(s); i++)
    {
        if (isdigit(*(s + i)))
        {
            if (!preIsD)
            {
                data.push_back(cast<T>(s + i));
            }
            preIsD = true;
        }
        else
        {
            preIsD = false;
        }
    }
}

std::map<unsigned int, unsigned int> filter_map;

/*
unsigned int get_ip(const std::string& ip)
{
    return inet_addr(ip.c_str());
}
*/

uint64 get_ip(const std::string& ip)
{
    std::vector<uint64> uVec;
    getAllNum(ip.c_str(), uVec);
    if (uVec.size() != 4)
        return 0;

    uint64 a =  uVec[3] << 24;
    uint64 b =  uVec[2] << 16;
    uint64 c =  uVec[1] << 8;
    uint64 d = uVec[0];
    return a + b + c + d;
}

void transfer()
{
    filter_map.clear();
    for (auto& kp : ip_map)
    {
        filter_map.insert(std::make_pair(get_ip(kp.first), get_ip(kp.second)));
    }
}

bool check(const std::string& ipAddr)
{
    unsigned int ip = get_ip(ipAddr);
    for (auto& kp : filter_map)
    {
        if ((kp.first & kp.second) == (ip & kp.second))
        {
            std::cout << kp.first << std::endl; 
            return true;
        }
    }
    return false;
}

namespace ns_test_1
{
    struct ip_infos
    {
        std::string ip, mask;
        unsigned int ip_int, mast_int;
        void operator()()
        {
            std::cout << "ip: " << ip << "(" << ip_int << "), mask: "<< mask << "(" << mast_int << ")\n";
        }
    };

    std::list<ip_infos> test_transfer()
    {
        std::list<ip_infos> info_list;
        for (auto& kp : ip_map)
        {
            ip_infos info;
            info.ip         =   kp.first;
            info.mask       =   kp.second;
            info.ip_int     =   get_ip(kp.first);
            info.mast_int   =   get_ip(kp.second);

            //info();
            info_list.push_back(info);
        }
        return info_list;
    }
    void test_1()
    {
        for (auto& kp : ip_map)
        {
            std::cout << kp.first << " : " << kp.second << std::endl;
        }

        for (auto& kp : filter_map)
        {
            std::cout << kp.first << " : " << kp.second << std::endl;
        }

        std::vector<std::string> ip_vec = 
        {
            "180.168.197.99",
            "222.73.33.156"
        };

        for (auto& item : ip_vec)
        {
            std::cout << item << " is " << (check(item) ? "in" : "not in") << " filterlist\n";
        }
    }

    void test_2()
    {
        std::list<ip_infos> infos = test_transfer();
        std::for_each(infos.begin(), infos.end(), 
                std::bind(&ip_infos::operator(), std::placeholders::_1));
    }
};

int main()
{
    transfer();
    ns_test_1::test_1();
    //ns_test_1::test_2();
    return 0;
}
