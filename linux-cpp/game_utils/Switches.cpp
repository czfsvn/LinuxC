#include "Switches.h"

bool Switches::Reg(Switch *sw)
{
    if (!sw || sw->name.empty())
        return false;

    switches_map.emplace(sw->name, sw);
}
bool Switches::Set(const std::string &name, int32_t value, const char *log /* = nullptr*/)
{
    std::map<std::string, Switch *>::iterator iter = switches_map.find(name);
    if (iter == switches_map.end())
        return false;

    if (log)
    {
        std::cout << "开关： " << name << ": " << iter->second->value << " --> " << value;
    }
    iter->second->Set(value);
}

int32_t Switches::Get(const std::string &name)
{
    std::map<std::string, Switch *>::iterator iter = switches_map.find(name);
    if (iter == switches_map.end())
        return 0;

    return iter->second->value;
}

Switch *Switches::GetSwitch(const std::string &name)
{
    std::map<std::string, Switch *>::iterator iter = switches_map.find(name);
    if (iter == switches_map.end())
        return nullptr;

    return iter->second;
}

namespace sw
{
    Switch test_sw = Switch("test_sw", 0, 1);
}