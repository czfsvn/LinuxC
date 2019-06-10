//
//  AITree.cpp
//  KPGranny2
//
//  Created by bianchx on 15/9/15.
//
//
 
#include "AITree.h"
 
#include <iostream>
#include <sstream>
#include <stdarg.h>
 
#define COMMAND_LINE 0
#define COCOS2D 1
#define AI_DEBUG 1
 
#if COCOS2D
//#include "cocos2d.h"
#endif

using namespace std;

#define getMsg(msg, msglen, pat)    \
    do \
{\
    va_list ap; \
    memset(msg, 0, msglen); \
    va_start(ap, pat);  \
    vsnprintf(msg, msglen - 1, pat, ap);    \
    va_end(ap); \
}while(false)



void CCLOG(const char* pattern, ...)
{
    char buf[4096];
    getMsg(buf, 4096, pattern);
    std::cout << buf << std::endl;
}
 
AINodeHelper * AINodeHelper::m_nodeHlper(NULL);
 
AINodeHelper * AINodeHelper::getMe()
{
    if(m_nodeHlper == NULL)
        m_nodeHlper = new AINodeHelper();
     
    return m_nodeHlper;
}
 
void AINodeHelper::registerNodeCreate(int type, BaseNodeCreate create)
{
    m_type2Create[type] = create;
}
 
void AINodeHelper::registerNodeName(int type, std::string name)
{
    m_type2Name[type] = name;
}
 
AINodeBase * AINodeHelper::CreateNode(int type)
{
    AINodeBase * nodeBase = NULL;
     
    do
    {
        map<int, BaseNodeCreate>::iterator iter = m_type2Create.find(type);
         
        if(iter == m_type2Create.end())
            break;
         
        nodeBase = (*iter).second();
         
        if(nodeBase == NULL)
            break;
         
        map<int, string>::iterator iter_name = m_type2Name.find(type);
        if(iter_name != m_type2Name.end())
        {
            string & name = (*iter_name).second;
            nodeBase->setName(name);
        }
    }while(0);
     
    return nodeBase;
}
 
AINodeBase * AINodeHelper::CreateNodeTree(vector<AINodeDescribe> des, void * host)
{
    if(des.size() == 0)
        return NULL;
     
#if COMMAND_LINE && AI_DEBUG
    cout << "CreateNodeTree all count = " << des.size() << endl;
#endif
     
#if COCOS2D && AI_DEBUG
    CCLOG("CreateNodeTree all count = %d", (int)des.size());
#endif
     
    map<int, AINodeBase *> m_type2Create;
     
    AINodeBase * rootNode = NULL;
     
    for(vector<AINodeDescribe>::iterator iter = des.begin(); iter != des.end(); ++iter)
    {
        AINodeDescribe &item = (*iter);
        AINodeBase * node = CreateNode(item.AINodeTypeId);
         
#if COMMAND_LINE && AI_DEBUG
        cout << "CreateNodeTree " << item.AINodeTypeId << endl;
#endif
         
#if COCOS2D && AI_DEBUG
        CCLOG("CreateNodeTree %d", item.AINodeTypeId);
#endif
         
        if(node == NULL)
            continue;
         
        node->m_host = host;            //注入宿主
         
        if(strlen(item.Describe) != 0)
        {
            node->setDescribe(item.Describe);
        }
         
        m_type2Create[item.Id] = node;
         
        if(item.ParentId == 0)
        {
            rootNode = node;
        }
        else
        {
            do
            {
                AINodeBase * parentNode = m_type2Create[item.ParentId];
                if(parentNode == NULL)
                    break;
                 
                AIListNode * listParentNode = dynamic_cast<AIListNode *>(parentNode);
                if(listParentNode != NULL)
                {
                    listParentNode->addChildNode(node);
                    break;
                }
                 
                AISingleNode * singleNode = dynamic_cast<AISingleNode *>(parentNode);
                if(singleNode != NULL)
                {
                    singleNode->setChildNode(node);
                    break;
                }
                 
            } while (0);
        }
    }
     
    return rootNode;
}
 
void AINodeHelper::printAITree(AINodeBase * node, int level)
{
    ostringstream oss;
     
    for (int i = 0; i < level; ++i)
    {
        oss << "\t";
    }
     
    oss << node->getName() << " " << node->getDescribe() << " " << node;
     
#if COMMAND_LINE
    cout << oss.str().c_str() << endl;;
#endif
     
#if COCOS2D
    CCLOG(oss.str().c_str());
#endif
     
    do
    {
        AIListNode * listNode = dynamic_cast<AIListNode *>(node);
        if(listNode != NULL)
        {
            vector<AINodeBase *> & childs = listNode->getChildNodes();
            if(childs.size() > 0)
            {
                for (std::vector<AINodeBase *>::iterator i = childs.begin(); i != childs.end(); ++i)
                {
                    printAITree(*i, level + 1);
                }
            }
             
            break;
        }
         
        AISingleNode * singleNode = dynamic_cast<AISingleNode *>(node);
        if(singleNode != NULL)
        {
            AINodeBase * child = singleNode->getChildNode();
            if(child != NULL)
            {
                printAITree(child, level + 1);
            }
        }
    } while (0);
     
     
}
 
int MemoryManagementObject::getReferenceCount()
{
    return m_mmo_referenceCount;
}
 
void MemoryManagementObject::retain()
{
    ++m_mmo_referenceCount;
}
 
void MemoryManagementObject::release()
{
    --m_mmo_referenceCount;
     
    if(m_mmo_referenceCount <= 0)
    {
        delete this;
    }
}
 
//最根层节点
void AINodeBase::destroy()
{
#if COMMAND_LINE && AI_DEBUG
    cout << "destroy " << getName() << " " << this << endl;
#endif
     
#if COCOS2D && AI_DEBUG
    CCLOG("destroy %s %p", getName().c_str(), this);
#endif
     
    release();
}
 
void AINodeBase::setDescribe(std::string describe)
{
    m_nodeDescribe = describe;
}
 
string AINodeBase::getDescribe()
{
    return m_nodeDescribe;
}
 
void AINodeBase::setName(string name)
{
    m_nodeName = name;
}
 
string AINodeBase::getName()
{
    return m_nodeName;
}
 
//列表节点
void AIListNode::addChildNode(AINodeBase * node)
{
    m_childNodes.push_back(node);
}
 
std::vector<AINodeBase *> & AIListNode::getChildNodes()
{
    return m_childNodes;
}
 
void AIListNode::destroy()
{
    if(m_childNodes.size() > 0)
    {
        for(vector<AINodeBase *>::iterator iter = m_childNodes.begin(); iter != m_childNodes.end(); ++iter)
        {
            (*iter)->destroy();
        }
    }
     
    AINodeBase::destroy();
}
 
//单条节点
void AISingleNode::setChildNode(AINodeBase * node)
{
    if(m_childNode != node)
    {
        if(m_childNode != NULL)
            m_childNode->destroy();
         
        m_childNode = node;
    }
}
 
AINodeBase * AISingleNode::getChildNode()
{
    return m_childNode;
}
 
void AISingleNode::destroy()
{
    if(m_childNode != NULL)
    {
        m_childNode->destroy();
    }
     
    AINodeBase::destroy();
}
 
//选择节点
AINodeRegister<AISelectNode> AISelectNode::reg(ANBT_SELECT, "AISelectNode");
 
bool AISelectNode::invoke(int level, bool isLog)
{
    bool success = false;
     
    do
    {
        if(m_childNodes.size() == 0)
            break;
         
        for(vector<AINodeBase *>::iterator iter = m_childNodes.begin(); iter != m_childNodes.end(); ++iter)
        {
            AINodeBase * node = (*iter);
            bool inv = node->invoke(level + 1, isLog);
             
#if (COMMAND_LINE || COCOS2D) && AI_DEBUG
            ostringstream oss;
             
            for (int i = 0; i < level; ++i) {
                oss << " ";
            }
             
            oss << node->getName() << " invoke " << inv;
#endif
             
#if COMMAND_LINE && AI_DEBUG
            if(isLog)
            {
                cout << oss.str().c_str() << endl;
            }
#endif
             
#if COCOS2D && AI_DEBUG
            if(isLog)
            {
                CCLOG("%s", oss.str().c_str());
            }
#endif
             
            if(inv)
            {
                success = true;
                break;
            }
        }
    } while (false);
     
    return success;
}
 
//顺序节点
AINodeRegister<AISequenceNode> AISequenceNode::reg(ANBT_SEQUENCE, "AISequenceNode");
 
bool AISequenceNode::invoke(int level, bool isLog)
{
    bool success = true;
     
    do
    {
        for(vector<AINodeBase *>::iterator iter = m_childNodes.begin(); iter != m_childNodes.end(); ++iter)
        {
            AINodeBase * node = (*iter);
            bool inv = node->invoke(level + 1, isLog);
             
#if (COMMAND_LINE || COCOS2D) && AI_DEBUG
            ostringstream oss;
             
            for (int i = 0; i < level; ++i) {
                oss << " ";
            }
             
            oss << node->getName() << " invoke " << inv;
#endif
             
#if COMMAND_LINE && AI_DEBUG
            if(isLog)
            {
                cout << oss.str() << endl;
            }
#endif
             
#if COCOS2D && AI_DEBUG
            if(isLog)
            {
                CCLOG("%s", oss.str().c_str());
            }
#endif
             
            if(inv == false)
            {
                success = false;
                break;
            }
        }
    } while (false);
     
    return success;
}
 
//取反节点
AINodeRegister<AINotNode> AINotNode::reg(ANBT_NOT, "AINotNode");
 
bool AINotNode::invoke(int level, bool isLog)
{
    bool success = false;
     
    do
    {
        if(m_childNode == NULL)
            break;
         
        success = !(m_childNode->invoke(level + 1, isLog));
    } while (false);
     
#if (COMMAND_LINE || COCOS2D) && AI_DEBUG
    ostringstream oss;
     
    for (int i = 0; i < level; ++i) {
        oss << " ";
    }
     
    if(m_childNode != NULL)
    {
        oss << m_childNode->getName() << " invoke " << !success;
    }
    else
    {
        oss << "no child";
    }
#endif
     
#if COMMAND_LINE && AI_DEBUG
    if(isLog)
    {
        cout << oss.str() << endl;
    }
#endif
     
#if COCOS2D && AI_DEBUG
    if(isLog)
    {
        CCLOG("no child");
    }
#endif
     
    return success;
}


void Test()
{
    AINodeDescribe des[] = {
        AINodeDescribe(1, 0, ANBT_SELECT, "root node"),

        AINodeDescribe(2, 1, ANBT_SEQUENCE, "condition node: if pick coins "),
        AINodeDescribe(5, 2, ANT_RELEASE_SKILL, "condition node: if exists coins"),
        AINodeDescribe(6, 2, ANT_PICKING_UP_COINS, "node: pick coins"),

        AINodeDescribe(3, 1, ANBT_SEQUENCE, "condition node: if return to owner"),
        AINodeDescribe(7, 3, ANT_RELEASE_SKILL, "condition node: no see coins long time"),
        AINodeDescribe(8, 3, ANT_PICKING_UP_COINS, "condition node: no see owner long time"),
        AINodeDescribe(9, 3, ANT_PICKING_UP_COINS, "node: return owner"),

        AINodeDescribe(4, 1, ANT_PICKING_UP_COINS, "no thing todo"),
    };

    int desCount = sizeof(des) / sizeof(AINodeDescribe);

    vector<AINodeDescribe> des_vtr;
    for (int i = 0; i < desCount; ++i)
    {
        des_vtr.push_back(des[i]);
    }

    AINodeBase * rootNode = AINodeHelper::getMe()->CreateNodeTree(des_vtr); 

    cout << "\nState organization chart \n" << endl;
    AINodeHelper::getMe()->printAITree(rootNode);
    cout << "\nState organization chart \n" << endl;

    for (int i = 0; i < 10; ++i)
    {
        cout << "tree call begin ==================" << endl;

        rootNode->invoke(0, true);

        cout << "tree call end ====================" << endl;
    }
}

int main()
{
    std::cout << "Hello, AITree\n";
    Test();
    return 0;
}
