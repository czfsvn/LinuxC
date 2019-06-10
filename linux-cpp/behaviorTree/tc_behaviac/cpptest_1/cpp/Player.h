#ifndef __PLAYER_20180326_H__
#define __PLAYER_20180326_H__

#include <iostream>
#include <stdarg.h>
#include <string.h>


namespace
{
#if 0
#define getMsg(msg, msglen, pat)    \
    do \
    {   \
        va_list ap; \
        memset(msg, 0, msglen); \
        va_start(ap, pat);  \
        va_end(ap); \
    } while(false)

    void CLOG(const char* pattern, ...)
    {
        char buf[4096];
        getMsg(buf, 4096, pattern);
        std::cout << buf << std::endl;
    }
#endif
#define CLOG    printf
}

class PlayerAgent;

class Player
{
public:
    Player(int, std::string);
    ~Player();

    void    setAgent(PlayerAgent* pa);
    void    initAgent(const char* nodeName);
    void    update();

    void    autoMove();
    void    autoFight();
private:
    int     id_;
    std::string name_;
    PlayerAgent* playerAgent_;
};

#endif
