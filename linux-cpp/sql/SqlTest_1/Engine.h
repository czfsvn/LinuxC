#ifndef __ENGING_20140504_H__
#define __ENGING_20140504_H__

#include "TypeDef.h"
#include "Common.h"
#include "MysqlCon.h"

#include <boost/serialization/singleton.hpp>
#include <boost/shared_ptr.hpp>


/*创建四个表格
 * 1：MyISAM 单条操作
 * 2: MyISAM 多条操作
 * 3：InnoDB 单条操作
 * 4: InnoDB 多条操作
 */

enum OP_Type
{
    OP_M_SINGLE     = 1,
    OP_M_COMPLEX,
    OP_I_SINGLE,
    OP_I_COMPLEX,
};

class Engine : public  boost::serialization::singleton<Engine>
{
    public:
        Engine();
        ~Engine();

        void Run(uint32 type);

    protected:
    private:
        void _Init(uint32 type);
        void _CreateTables(uint32 type);

        void _SingleInsert(uint32 type);
        void _SingleUpdate(uint32 type);

        void _ComInsert(uint32 type, uint32 maxlen);
        void _ComUpdate(uint32 type, uint32 maxlen);

    private:
        //boost::shared_ptr<MysqlCon>     m_conn;
        MysqlCon*                       m_conn;
            /*
        const static std::string IP        = "127.0.0.1";
        const static std::string USER      = "czf";
        const static std::string PWD       = "123";
        const static std::string DB        = "test";
        */
};




#endif
