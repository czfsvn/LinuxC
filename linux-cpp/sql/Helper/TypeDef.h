#ifndef ___TYPE_DEF_20110309_H___
#define ___TYPE_DEF_20110309_H___

#include <string>
#include <list>
#include <iostream>

#include <boost/assert.hpp>
//#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

//#define BOOST_ENABLE_ASSERT_HANDLER
#define MAX_ID_GAP					100000

#ifdef linux
typedef signed long     int64;
typedef signed int      int32;
typedef signed short    int16;
typedef signed char     int8;
typedef unsigned long   uint64;
typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;
#else
typedef __int64   int64;
typedef __int32   int32;
typedef __int16   int16;
typedef __int8    int8;
typedef unsigned __int64  uint64;
typedef unsigned __int32  uint32;
typedef unsigned __int16  uint16;
typedef unsigned __int8   uint8;
#endif

typedef const char*		CCharPtr;

const static std::string CONST_SEP = ",";



std::string Int2Str(int32 num);
std::string Int2Str(uint32 num);
std::string Int2Str(int64 num);
std::string Int2Str(uint64 num);
uint16 Str2UInt16(const char* str);
int16 Str2Int16(const char* str);
uint32 Str2UInt(const char* str);
int32 Str2Int(const char* str);
int64 Str2Int64(const char* str);
uint64 Str2UInt64(const char* str);



template<typename T1, typename T2> inline T1 Boost_Lexical(T1& des, T2& res)
{
	return boost::lexical_cast<T1>(res);
}

std::string IntBStr(uint16 num);
std::string IntBStr(uint32 num);
std::string IntBStr(uint64 num);
std::string IntBStr(int16 num);
std::string IntBStr(int32 num);
std::string IntBStr(int64 num);
uint32 StrBUInt(std::string res);
uint16 StrBUInt16(std::string res);
uint64 StrBUInt64(std::string res);
uint8 StrBUInt8(std::string res);
int32 StrBInt(std::string res);
int16 StrBInt16(std::string res);
int64 StrBInt64(std::string res);
int8 StrBInt8(std::string res);


uint32 StrBUInt(CCharPtr res);
uint16 StrBUInt16(CCharPtr res);
uint64 StrBUInt64(CCharPtr res);
uint8 StrBUInt8(CCharPtr res);
int32 StrBInt(CCharPtr res);
int16 StrBInt16(CCharPtr res);
int64 StrBInt64(CCharPtr res);
int8 StrBInt8(CCharPtr res);

float StrBFloat(std::string res);
float StrBFloat(CCharPtr res);




bool MakeStr(std::list<uint32>& uList, std::string& des, std::string sep = CONST_SEP);
bool MakeStr(std::list<std::string>& uList, std::string& des, std::string sep = CONST_SEP);
bool MakeAddStr(std::string& res, std::string& des, uint32 add, bool zeroVoid);
bool MakeAddStr(const char* res, std::string& des, uint32 add, bool zeroVoid);

bool StrSplit(std::string& res, std::list<std::string>& strList, std::string sep = CONST_SEP);
bool StrSplit(std::string& res, std::list<uint32>& uList, std::string sep = CONST_SEP);

bool StrSplit(CCharPtr src, std::list<uint32>& strList, std::string sep = CONST_SEP);
bool StrSplit(CCharPtr src, std::list<std::string>& strList, std::string sep = CONST_SEP);

//namespace boost
//{
//	void assertion_failed(char const * expr, char const * function, char const * file, long line)
//	{
//		boost::format fmt("Assertion failed!\n Expression: %s\nFunciont:%s\nFile:%s\nLine:%d\n\n");
//		fmt%expr%function%file%line;
//		std::cout<<fmt;
//		//Log::get_mutable_instance().LogText(fmt);
//	}
//}


#endif
