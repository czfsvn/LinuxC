#include "workspace/exported/behaviac_generated/types/behaviac_types.h"
#include <unistd.h>

#define LOGI printf

void Test()
{
    //const char* path = "../tutorials/tutorial_1/cpp/exported";
    const char* path = "../cpp_agent/workspace/exported";
    const char* agant_name = "cpp_agent_bt";
    LOGI("InitBehavic\n");
    behaviac::Workspace::GetInstance()->SetFilePath(path);
    behaviac::Workspace::GetInstance()->SetFileFormat(behaviac::Workspace::EFF_xml);

    LOGI("InitAgent\n");
    cpp_agent* agent = behaviac::Agent::Create<cpp_agent>();
    if (agent->btload(agant_name) == false)
    {
        LOGI("InitAgent faield\n");
        return;
    }
    agent->btsetcurrent(agant_name);

    LOGI("Update\n");

    while (1)
    {
        behaviac::EBTStatus status = behaviac::BT_RUNNING;
        while (status == behaviac::BT_RUNNING)
        {
            status = agent->btexec();
        }
        sleep(2);
    }

    LOGI("cleanup\n");
    behaviac::Agent::Destroy(agent);
    behaviac::Workspace::GetInstance()->Cleanup();
}

int main()
{
    Test();
    return 0;
}
