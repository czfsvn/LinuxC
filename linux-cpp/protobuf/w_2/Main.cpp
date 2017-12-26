#include <iostream>
#include "person.pb.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace std;

auto book_lbd = [](AddressBook& book)->void
{
	Person* person = book.add_people();
	person->set_name("test1");
	person->set_id(1);
	person->set_email("test1@.mail");
	std::cout<<book.Utf8DebugString()<<std::endl;
};

void Test_1()
{
	AddressBook book;
	book_lbd(book);
}

void Test_2()
{
	// const google::protobuf::Descriptor* 
	// 先获得类型的Descriptor .
	auto descriptor =
	google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName("AddressBook");
	if (nullptr == descriptor)
		return;

	// google::protobuf::Message *
	// 利用Descriptor拿到类型注册的instance. 这个是不可修改的.
	auto prototype =
	google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
	if (nullptr == prototype)
		return;

	// 构造一个可用的消息.
	// google::protobuf::Message *
	auto message = prototype->New();
	if (message == nullptr)
		return;

	auto book = dynamic_cast<AddressBook*>(message);
	// 直接调用message的具体接口
	// 其实这些接口是语法糖接口.所以并没有对应的反射机制来对应调用.
	// 反射机制实现了的Set/Get XXX系列接口，是属于Reflection的接口，接收Message作为参数.
	book_lbd(*book);
}

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>

void Test_3()
{
	// 准备配置好文件系统
	google::protobuf::compiler::DiskSourceTree sourceTree;
	// 将当前路径映射为项目根目录 ， project_root 仅仅是个名字，你可以你想要的合法名字.
	sourceTree.MapPath("project_root", "../");
	// 配置动态编译器.
	google::protobuf::compiler::Importer importer(&sourceTree, NULL);
	// 动态编译proto源文件。 源文件在./source/proto/test.proto .
	importer.Import("project_root/person.proto");
	// 现在可以从编译器中提取类型的描述信息.
	auto descriptor = importer.pool()->FindMessageTypeByName("Person");
	if (nullptr == descriptor)
		return;

	// 创建一个动态的消息工厂.
	google::protobuf::DynamicMessageFactory factory;
	// 从消息工厂中创建出一个类型原型.
	auto protol = factory.GetPrototype(descriptor);
	if (nullptr == protol)
		return;
	// 构造一个可用的消息.
	auto message = protol->New();
	if (nullptr == message)
		return;

	// 下面是通过反射接口给字段赋值.
	auto reflection = message->GetReflection();
	auto field = descriptor->FindFieldByName("id");
	reflection->SetUInt32(message, field, 1);
	std::cout << "Person: " << message->DebugString() << std::endl;
	delete message;
}

void Test_4()
{
	// 拿到一个对象，不在乎怎么拿到，可以是通过反射拿到。
	// 这里简单直接的创建一个.
	Person person;
	// 拿到对象的描述包.
	auto descriptor = person.GetDescriptor();
	// 拿到对象的反射配置.
	auto reflector = person.GetReflection();
	// 拿到属性的描述包.
	auto field = descriptor->FindFieldByName("id");
	// 设置属性的值.
	reflector->SetUInt32(&person, field, 5);
	// 获取属性的值.
	std::cout << reflector->GetUInt32(person, field) << std::endl;
}

int main()
{
		Test_1();
		Test_2();
		Test_3();
		Test_4();
		std::cout<<"Hello, World!"<<std::endl;
		return 0;
}
