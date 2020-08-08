#ifndef __SWITCHES_H__
#define __SWITCHES_H__

#include <iostream>

#include <map>

#include "Singleton.h"

class Switch;

class Switches : public Singleton<Switches>
{
public:
    bool Reg(Switch *sw);
    bool Set(const std::string &name, int32_t value, const char *log = nullptr);

    int32_t Get(const std::string &name);

    Switch *GetSwitch(const std::string &name);

private:
    //friend class SinglegonFactory<Switches>;
    //Switches() {}
    std::map<std::string, Switch *> switches_map = {};
};

class Switch
{
public:
    Switch(const char *n, int32_t v, uint32_t f) : name(n), value(v), flag(f)
    {
        Switches::getMe().Reg(this);
    }

    int32_t Set(int32_t v)
    {
        value = v;
        return value;
    }

    int32_t operator()() const { return value; }

    uint32_t flag = 0;
    int32_t value = 0;
    std::string name = "";
};

namespace sw
{
    extern Switch test_sw;
}

#endif