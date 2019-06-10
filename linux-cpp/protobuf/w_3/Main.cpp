#include <iostream>
#include "person.pb.h"
#include <google/protobuf/descriptor.h> 

auto book_lbd = [](AddressBook& book)->void
{
    Person* person = book.add_people();
    person->set_name("test1");
    person->set_id(1);
    person->set_email("test1@.mail");
};

auto book_cout = [](const AddressBook& book)->void
{
    std::cout<<book.Utf8DebugString()<<std::endl;
};

class MsgBase
{
    public:
        virtual void onMsg(google::protobuf::Message* msg) = 0;
};

class MsgHandler
{
public:
    void Reg(const google::protobuf::Descriptor* descriptor, MsgBase* base)
    {
        process_map_[descriptor] = base;
    }

    void onMsg(const char* msg_name, const char* datas, int size)
    {
        if (!msg_name || !datas)
            return;

        const google::protobuf::Descriptor* descriptor =
            google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(msg_name);
        if (!descriptor)
            return;

        const google::protobuf::Message* prototype = 
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (!prototype)
            return;

        google::protobuf::Message* pMsg = prototype->New();
        if (!pMsg)
            return;

        if (!pMsg->ParseFromArray(datas, size))
            return;

        auto iter = process_map_.find(descriptor);
        if (iter == process_map_.end() || iter->second == nullptr)
            return;

        iter->second->onMsg(pMsg);
        delete pMsg;
    }

    static MsgHandler& Inst()
    {
        static MsgHandler msg_handler;
        return msg_handler;
    }

private:

    std::map<const google::protobuf::Descriptor*, MsgBase*>   process_map_;

};

class AddressBookMsg: public MsgBase
{
public:
    AddressBookMsg() 
    {
        MsgHandler::Inst().Reg(AddressBook::descriptor(), this);
    }

    virtual void onMsg(google::protobuf::Message* msg)
    {
        if (!msg)
            return;

        const AddressBook* book = dynamic_cast<AddressBook*>(msg);
        if (!book)
            return;

        doMsg(book);
    }
    
    void doMsg(const AddressBook* msg)
    {
        std::cout << "[AddressBookMsg] doMsg\n";         
        book_cout(*msg);
    }
};

template <typename T>
class MsgTemp : public MsgBase
{
public:
    MsgTemp()
    {
        MsgHandler::Inst().Reg(T::descriptor(), this);
    }

    virtual void onMsg(google::protobuf::Message* msg)
    {
        if (!msg)   return;
        const T* message = dynamic_cast<T*>(msg);
        if (!message)   return;

        doMsg(message);
    }

    void doMsg(const T* msg);
    //{
    //   std::cout << "[typename T] doMsg\n"; 
    //}
};

//static AddressBookMsg book_msg;
//static MsgTemp<AddressBook> book_msg_temp;
#define PROTO_MSG(TYPE) \
static MsgTemp<TYPE>    __MSG_TYPE__;   \
template <typename TYPE> void MsgTemp<TYPE>::doMsg(const TYPE* msg) 

PROTO_MSG(AddressBook)
{
    std::cout << "[typename AddressBookMsg] doMsg\n";
}

namespace ns_test_1
{
    void test1()
    {
        AddressBook book;
        book_lbd(book);
        
        int len = 1024;
        char bin[1024];
        memset(bin, 0, sizeof(bin));
    
        book.SerializeToArray(bin, len);
        MsgHandler::Inst().onMsg(book.GetTypeName().c_str(), bin, book.GetCachedSize());
        return ;
    }
}


int main()
{
    ns_test_1::test1();
    return 0;
}

