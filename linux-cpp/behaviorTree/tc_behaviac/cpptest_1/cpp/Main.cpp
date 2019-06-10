#include <iostream>
#include "Player.h"
#include <list>
#include <cstdio>
#include <string>
#include <unistd.h>
#include "behaviac_generated/types/behaviac_types.h"

using namespace std;

#if !BEHAVIAC_CCDEFINE_ANDROID
static void SetExePath()
{
#if BEHAVIAC_CCDEFINE_MSVC   
    TCHAR szCurPath[_MAX_PATH];
    GetModuleFileName(NULL, szCurPath, _MAX_PATH);
    TCHAR* p = szCurPath;
    while (_tcschr(p, L'\\'))
    {
        p = _tcschr(p, L'\\');
        p++;
    }
    *p = L'\0';
    SetCurrentDirectory(szCurPath);
#endif
}
#endif

const char* file_path = "../cpptest_1/cpp/exported/";
const char* node_name = "PlayerAction";

bool InitBehavic(const char*  path)
{
    CLOG("InitBehavic\n");
    behaviac::Workspace::GetInstance()->SetFilePath(path);
    behaviac::Workspace::GetInstance()->SetFileFormat(behaviac::Workspace::EFF_xml);
    return true;
}

PlayerAgent* InitPlayer(Player* owner, const char* nodeName)
{
    CLOG("InitPlayer\n");
    PlayerAgent* pa = behaviac::Agent::Create<PlayerAgent>();
    bool bRet = pa->btload(nodeName);
    if (!bRet)
    {
        CLOG("InitPlayer failed\n");
        return NULL;
    }

    pa->btsetcurrent(nodeName);
    pa->setOwner(owner);
    return pa;
}

void UpdateLoop(std::list<Player*>& pList)
{
    CLOG("UpdateLoop\n");
    while (1)
    {
        for (std::list<Player*>::iterator it = pList.begin();
                it != pList.end(); it++)
        {
            Player* p = *it;
            if (!p)
                continue;
            p->update();
        }
        sleep(1);
    }
}

void CleanupBehaviac()
{
    CLOG("CleanupBehaviac\n");
    behaviac::Workspace::GetInstance()->Cleanup();
}

void Test()
{
    SetExePath();
    CLOG("BEHAVIAC_CCDEFINE_NAME=%s\n", BEHAVIAC_CCDEFINE_NAME);
    InitBehavic(file_path);

    std::list<Player*> pList;
    for (int idx = 0; idx < 1; idx++)
    {
        Player* p = new Player(idx, "name_" + std::to_string(idx));
        PlayerAgent* pa = InitPlayer(p, node_name);
        p->setAgent(pa);
        pList.push_back(p);
    }

    UpdateLoop(pList);

    for (std::list<Player*>::iterator it = pList.begin();
            it != pList.end(); it++)
    {
        Player* p = *it;
        if (!p)
            continue;
        delete p;
        p = NULL;
    }
    CleanupBehaviac();
    //CLOG("Press any key to continue...");
    //int ret = getchar();    
}

int main()
{   
    Test();
    return 0;
}

