#include "msg.pb.h"
#include <iostream>

using namespace std;

int main()
{
    demo::msg msg_test;
    msg_test.set_msgtype(1);
    msg_test.set_msginfo("hello");
    msg_test.set_msgfrom("localhost");

    string in_data;
    msg_test.SerializeToString(&in_data);
    std::cout<<"format : "<<in_data<<std::endl;

    demo::msg msg_encoding;
    msg_encoding.ParseFromString(in_data);
    std::cout<<"msg type : "<<msg_encoding.msgtype()<<std::endl;
    std::cout<<"msg info : "<<msg_encoding.msginfo()<<std::endl;
    std::cout<<"msg from : "<<msg_encoding.msgfrom()<<std::endl;

    return 0;
}
