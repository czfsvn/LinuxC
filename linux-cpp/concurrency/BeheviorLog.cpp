#include <boost/lexical_cast.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

std::map<uint32_t, std::string> name_map = {
    { 1, "王城普通镖车" },
    { 2, "点卡镖车" },
    { 3, "刺探" },
    { 4, "点卡刺探" },
    { 5, "国家宝藏" },
    { 6, "天空宝藏" },
    { 7, "挖宝摸金" },
    { 8, "死亡" },
    { 9, "原地复活" },
    { 10, "杀敌" },

    { 11, "消耗银子" },
    { 12, "消耗银卡" },
    { 13, "消耗征途2点卡" },
    { 14, "公共频道聊天" },
    { 15, "私人频道聊天" },
    { 16, "相应拉令" },
    { 17, "交易出售道具" },
    { 18, "交易获得道具" },
    { 19, "摆摊出售道具" },
    { 20, "摆摊获得物品" },

    { 21, "发送邮件道具" },
    { 22, "提取邮件道具" },
    { 23, "喝酒" },
    { 24, "英雄帖" },
    { 25, "吃果子" },
    { 26, "武林精英" },
    { 27, "仙侣情缘-发起" },
    { 28, "仙侣情缘-接受" },
    { 29, "圣地凶魔" },

    { 30, "家族龙舟" },
    { 31, "家族战" },
    { 32, "家族BOSS蛋" },
    { 33, "家族单挑" },
    { 34, "运镖大作战积分" },
    { 35, "家族祝福果" },
    { 36, "夺宝战场" },
    { 37, "获得银子" },
    { 38, "获得银卡" },
    { 39, "获得征途2点卡" },

    { 40, "击杀boss 次数" },
    { 41, "攻击boss次数" },
    { 42, "获得神将殿积分数量" },
    { 43, "完成五行迷阵次数" },
    { 44, "段位赛次数" },
    { 45, "钻石排位赛次数" },
    { 46, "钻石排位赛杀敌次数" },
    { 47, "九州殿完成次数" },
    { 48, "天外天完成次数" },
    { 49, "状元收菜" },

    { 50, "威名数" },
    { 51, "中立区镖车" },
    { 52, "中立区传送令" },
    { 53, "使用回城卷" },
    { 54, "进入青丘地图" },
    { 55, "进入中州地图" },
    { 56, "进入跨区家族乱武" },
    { 57, "进入3v3" },
    { 58, "进入跨区时空" },
    { 59, "获得家族军工" },

    { 60, "抓取玄兽" },
    { 61, "英雄岛杀人" },
    { 62, "消耗军票" },
    { 63, "获得军票" },
    { 64, "互通版登陆时长" },
};

std::vector<std::string> vStringSplit(const std::string& s, const std::string& delim = ",")
{
    std::vector<std::string> elems;
    size_t                   pos       = 0;
    size_t                   len       = s.length();
    size_t                   delim_len = delim.length();
    if (delim_len == 0)
        return elems;
    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}

template <typename T>
T lexical_cast(const std::string& str)
{
    using boost::lexical_cast;
    return boost::lexical_cast<T>(str.c_str());
}

namespace ns_struct
{
    struct Base
    {
        uint32_t    char_id   = 0;
        uint32_t    server    = 0;
        uint32_t    level     = 0;
        uint32_t    sec_level = 0;
        uint32_t    thi_level = 0;
        uint32_t    career    = 0;
        uint64_t    fp        = 0;
        std::string name      = "";

        std::string dump()
        {
            std::stringstream oss;
            oss << server << "," << name << "," << level << "," << sec_level << "," << thi_level;
            oss << "," << career << "," << fp;
            return oss.str();
        }
    };

    struct Details
    {
        uint32_t char_id  = 0;
        uint32_t event_id = 0;
        uint64_t value    = 0;
    };

    struct Login
    {
        uint32_t    char_id      = 0;
        uint32_t    reg_ip       = 0;
        uint32_t    login_ip     = 0;
        std::string machine_code = "";
    };

    class UserData
    {
    public:
        UserData(uint32_t cid) : char_id_(cid) {}

        void updateBase(const Base& base)
        {
            base_ = base;
        }

        void updateDetail(const Details& detail)
        {
            auto iter = detail_map_.find(detail.event_id);
            if (iter == detail_map_.end())
            {
                detail_map_.emplace(detail.event_id, std::move(detail));
            }
            else
            {
                iter->second.value += detail.value;
            }
        }

        void updateLogin(const Login& login)
        {
            reg_ip_ = login.reg_ip;
            machine_code_map_[login.machine_code] += 1;
            login_ip_map_[login.login_ip] += 1;
        }

        void dump()
        {
            std::stringstream oss;
            oss << base_.dump() << "," << reg_ip_ << "," << login_ip_map_.size();
            oss << "," << machine_code_map_.size() << ",";
            for (uint32_t idx = 1; idx < name_map.size() + 1; idx++)
            {
                auto iter = detail_map_.find(idx);
                if (iter == detail_map_.end())
                    oss << 0 << ",";
                else
                    oss << iter->second.value << ",";
            }
            std::cout << oss.str() << std::endl;
        }

    private:
        uint32_t                        char_id_ = 0;
        uint32_t                        reg_ip_  = 0;
        Base                            base_;
        std::map<uint32_t, Details>     detail_map_       = {};
        std::map<std::string, uint32_t> machine_code_map_ = {};
        std::map<uint32_t, uint32_t>    login_ip_map_     = {};
    };

    class DataManager
    {
    public:
        DataManager() {}

        ~DataManager() {}

        void run()
        {
            std::string path = "/home/cn/work/log/eventscenesserver21.log.200511-04";
            readFile(path);
        }

        void dump()
        {
            for (const auto& item : data_map_)
            {
                item.second->dump();
            }
        }

        void readFile(const std::string& file)
        {
            ifstream infile;
            infile.open(file.data());  //将文件流对象与文件连接起来
            assert(infile.is_open());  //若失败,则输出错误消息,并终止程序运行

            string s;
            while (getline(infile, s))
            {
                // cout << s << endl;
                parseLine(s);
            }
            infile.close();
        }

        void parseLine(const std::string& line)
        {
            std::string preffix = "[BEHAVIOR_";
            auto        pre_idx = line.find(preffix);
            if (string::npos == pre_idx)
                return;

            std::string base_suffix = "_BASE]";
            auto        base_idx    = line.find(base_suffix);
            if (base_idx != string::npos)
            {
                parseBase(line);
                return;
            }

            std::string detail_suffix = "_DETAIL]";
            auto        detail_idx    = line.find(detail_suffix);
            if (detail_idx != string::npos)
            {
                parseDetail(line);
                return;
            }

            std::string login_suffix = "_LOGIN]";
            auto        login_idx    = line.find(login_suffix);
            if (login_idx != string::npos)
            {
                parseLogin(line);
                return;
            }
        }

        void parseBase(const std::string& line)
        {
            const std::vector<std::string>& result = vStringSplit(line, ",");
            if (result.size() != 9)
                return;

            // std::cout << "base: " << line << std::endl;
            Base base;
            base.char_id   = lexical_cast<uint32_t>(result[3]);
            base.server    = lexical_cast<uint32_t>(result[1]);
            base.level     = lexical_cast<uint32_t>(result[4]);
            base.sec_level = lexical_cast<uint32_t>(result[5]);
            base.thi_level = lexical_cast<uint32_t>(result[6]);
            base.career    = lexical_cast<uint32_t>(result[7]);
            base.fp        = lexical_cast<uint64_t>(result[8]);
            base.name      = result[2];
            addBase(base);
        }

        void parseLogin(const std::string& line)
        {
            const std::vector<std::string>& result = vStringSplit(line, ",");
            if (result.size() != 5)
                return;

            // std::cout << "login: " << line << std::endl;

            Login login;
            login.char_id      = lexical_cast<uint32_t>(result[1]);
            login.reg_ip       = lexical_cast<uint32_t>(result[2]);
            login.login_ip     = lexical_cast<uint32_t>(result[3]);
            login.machine_code = result[4];
            addLogin(login);
        }

        void parseDetail(const std::string& line)
        {
            const std::vector<std::string>& result = vStringSplit(line, ",");
            if (result.size() != 4)
                return;

            // std::cout << "detail: " << line << std::endl;
            Details detail;
            detail.char_id  = lexical_cast<uint32_t>(result[1]);
            detail.event_id = lexical_cast<uint32_t>(result[2]);
            detail.value    = lexical_cast<uint64_t>(result[3]);
            addDetail(detail);
        }

        void addDetail(const Details& detail)
        {
            auto iter = data_map_.find(detail.char_id);
            if (iter != data_map_.end())
            {
                iter->second->updateDetail(detail);
            }
            else
            {
                UserData* user = new UserData(detail.char_id);
                user->updateDetail(detail);
                data_map_.emplace(detail.char_id, user);
            }
        }

        void addLogin(const Login& login)
        {
            auto iter = data_map_.find(login.char_id);
            if (iter != data_map_.end())
            {
                iter->second->updateLogin(login);
            }
            else
            {
                UserData* user = new UserData(login.char_id);
                user->updateLogin(login);
                data_map_.emplace(login.char_id, user);
            }
        }

        void addBase(const Base& base)
        {
            auto iter = data_map_.find(base.char_id);
            if (iter != data_map_.end())
            {
                iter->second->updateBase(base);
            }
            else
            {
                UserData* user = new UserData(base.char_id);
                user->updateBase(base);
                data_map_.emplace(base.char_id, user);
            }
        }

    private:
        std::map<uint32_t, UserData*> data_map_ = {};
    };

    void run()
    {
        DataManager dataM;
        dataM.run();
        dataM.dump();
    }

}  // namespace ns_struct

int main()
{
    ns_struct::run();
}
