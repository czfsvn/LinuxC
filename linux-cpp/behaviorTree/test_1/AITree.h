//
//  AITree.h
//  KPGranny2
//
//  Created by bianchx on 15/9/15.
//
//
 
#ifndef __KPGranny2__AITree__
#define __KPGranny2__AITree__
 
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <string.h>
 
//#include "AITreeNodeType.h"
 
#pragma mark =============== public Helper Action ==================
 
class AINodeBase;
 
typedef AINodeBase * (* BaseNodeCreate)();
 
struct AINodeDescribe
{
    AINodeDescribe()
    {
        memset(this, 0, sizeof(AINodeDescribe));
    }
     
    AINodeDescribe(int id, int pId, int typeId, const char * describe = NULL)
        :Id(id)
         ,ParentId(pId)
         ,AINodeTypeId(typeId)
    {
        memset(Describe, 0, sizeof(Describe));

        if(describe != NULL && strlen(describe) < sizeof(Describe) / sizeof(char))
        {
            strcpy(Describe, describe);
        }
    }
     
    int Id;             //当期节点Id
    int ParentId;       //父节点Id
    int AINodeTypeId;   //智能节点类型Id
    char Describe[256];   //节点名称
};
 
class AINodeHelper
{
private:
    static AINodeHelper * m_nodeHlper;
     
    std::map<int, BaseNodeCreate> m_type2Create;
    std::map<int, std::string> m_type2Name;
     
public:
    static AINodeHelper * getMe();
     
    void registerNodeCreate(int type, BaseNodeCreate create);
    void registerNodeName(int type, std::string name);
     
    AINodeBase * CreateNode(int type);          //创建节点
     
    AINodeBase * CreateNodeTree(std::vector<AINodeDescribe> des, void * host = NULL);
     
    void printAITree(AINodeBase * node, int level = 0);
};
 
template <class T>
class AINodeRegister
{
public:
    static AINodeBase * CreateT()
    {
        return new T();
    }
     
    AINodeRegister(int type, std::string name = "")
    {
        AINodeHelper * helper = AINodeHelper::getMe();
         
        helper->registerNodeCreate(type, &AINodeRegister::CreateT);
         
        if(name != "")
            helper->registerNodeName(type, name);
    }
};
 
#pragma mark ================== 具体的内容 =================
 
enum AINodeBaseType
{
    ANBT_SELECT,        //选择节点
    ANBT_SEQUENCE,      //顺序节点
    ANBT_NOT,           //取反节点
};

enum AINodeType
{
    ANT_RELEASE_SKILL,
    ANT_PICKING_UP_COINS,
};
 
class MemoryManagementObject
{
private:
    int m_mmo_referenceCount;
     
public:
    MemoryManagementObject()
    :m_mmo_referenceCount(1)
    {
    }
     
    virtual ~MemoryManagementObject()
    {
         
    }
     
    int getReferenceCount();
    void retain();
    void release();
};
 
class AINodeBase : public MemoryManagementObject
{
protected:
    std::string m_nodeName;
    std::string m_nodeDescribe;
     
public:
    AINodeBase()
    :m_host(NULL)
    ,m_nodeName("AINodeBase")
    ,m_nodeDescribe("")
    {
    }
     
    virtual ~AINodeBase() { }
     
    void * m_host;      //AI的宿主
     
    virtual bool invoke(int level = 0, bool isLog = false) { return false; }
     
    virtual void destroy();
     
    virtual void setDescribe(std::string describe);
    virtual std::string getDescribe();
     
    virtual void setName(std::string name);
    virtual std::string getName();
};
 
//列表节点
class AIListNode : public AINodeBase
{
protected:
    std::vector<AINodeBase *> m_childNodes;
     
public:
    virtual void addChildNode(AINodeBase * node);
    virtual std::vector<AINodeBase *> & getChildNodes();
    virtual void destroy();
};
 
//单条节点
class AISingleNode : public AINodeBase
{
protected:
    AINodeBase * m_childNode;
     
public:
    AISingleNode()
    :m_childNode(NULL)
    { }
     
    virtual void setChildNode(AINodeBase * node);
    virtual AINodeBase * getChildNode();
    virtual void destroy();
};
 
//选择节点
class AISelectNode : public AIListNode
{
private:
    static AINodeRegister<AISelectNode> reg;
     
public:
    virtual bool invoke(int level = 0, bool isLog = false);
};
 
//顺序节点
class AISequenceNode : public AIListNode
{
private:
    static AINodeRegister<AISequenceNode> reg;
     
public:
    virtual bool invoke(int level = 0, bool isLog = false);
};
 
//取反节点
class AINotNode : public AISingleNode
{
private:
    static AINodeRegister<AINotNode> reg;
     
public:
    virtual bool invoke(int level = 0, bool isLog = false);
};
 
#endif /* defined(__KPGranny2__AITree__) */
