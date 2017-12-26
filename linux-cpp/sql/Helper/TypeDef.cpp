#include "TypeDef.h"
#include <sstream>
#include <boost/foreach.hpp>

std::string Int2Str(int32 num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

std::string Int2Str(uint32 num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

std::string Int2Str(int64 num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

std::string Int2Str(uint64 num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

uint16 Str2UInt16(const char* str)
{
	return str != NULL ? atoi(str) : 0;
}

int16 Str2Int16(const char* str)
{
	return str != NULL ? atoi(str) : 0;
}

uint32 Str2UInt(const char* str)
{
#ifdef __WINDOWS
	return str != NULL ? uint32(_atoi64(str)) : 0;
#else
	return str != NULL ? uint32(atol(str)) : 0;
#endif
}

int32 Str2Int(const char* str)
{
	return str != NULL ? atoi(str) : 0;
}

int64 Str2Int64(const char* str)
{
#ifdef __WINDOWS
	return str != NULL ? _atoi64(str) : 0;
#else
	return str != NULL ? atol(str) : 0;
#endif
}

uint64 Str2UInt64(const char* str)
{
#ifdef __WINDOWS
	return str != NULL ? _strtoui64(str, NULL, 10) : 0;
#else
	return str != NULL ? strtol(str, NULL, 10) : 0;
#endif
}

std::string IntBStr(uint16 num)
{
	std::string res;
	return Boost_Lexical(res, num);
}

std::string IntBStr(uint32 num)
{
	std::string res;
	return Boost_Lexical(res, num);
}

std::string IntBStr(uint64 num)
{
	std::string res;
	return  Boost_Lexical(res, num);
}

std::string IntBStr(int16 num)
{
	std::string res;
	return Boost_Lexical(res, num);
}

std::string IntBStr(int32 num)
{
	std::string res;
	return Boost_Lexical(res, num);
}

std::string IntBStr(int64 num)
{
	std::string res;
	return Boost_Lexical(res, num);
}

uint32 StrBUInt(std::string res)
{
	uint32 des = 0;
	return Boost_Lexical(des, res);
}

uint16 StrBUInt16(std::string res)
{
	uint16 des = 0;
	return Boost_Lexical(des, res);
}

uint8 StrBUInt8(std::string res)
{
	uint8 des = 0;
	return Boost_Lexical(des, res);
}

uint64 StrBUInt64(std::string res)
{
	uint64 des = 0;
	return Boost_Lexical(des, res);
}

int32 StrBInt(std::string res)
{
	int32 des = 0;
	return Boost_Lexical(des, res);
}

int16 StrBInt16(std::string res)
{
	int16 des = 0;
	return Boost_Lexical(des, res);
}

int8 StrBInt8(std::string res)
{
	int8 des = 0;
	return Boost_Lexical(des, res);
}

int64 StrBInt64(std::string res)
{
	int64 des = 0;
	return Boost_Lexical(des, res);
}

int32 StrBInt(CCharPtr res)
{
	return res ? StrBInt((std::string)res) : 0;
}

int16 StrBInt16(CCharPtr res)
{
	return res ? StrBInt16((std::string)res) : 0;
}

int64 StrBInt64(CCharPtr res)
{
	return res ? StrBInt64((std::string)res) : 0;
}

int8 StrBInt8(CCharPtr res)
{
	return res ? StrBInt8((std::string)res) : 0;
}

uint32 StrBUInt(CCharPtr res)
{
	return res ? StrBUInt((std::string)res) : 0;
}

uint16 StrBUInt16(CCharPtr res)
{
	return res ? StrBUInt16((std::string)res) : 0;
}

uint64 StrBUInt64(CCharPtr res)
{
	return res ? StrBUInt64((std::string)res) : 0;
}

uint8 StrBUInt8(CCharPtr res)
{
	return res ? StrBUInt8((std::string)res) : 0;
}

float StrBFloat(std::string res)
{
	float des = 0.0f;
	return Boost_Lexical(des, res);
}

float StrBFloat(CCharPtr res)
{
	return res ? StrBFloat((std::string)res) : 0.0f;
}

bool StrSplit(std::string& src, std::list<std::string>& strList, std::string sep)
{
	std::string s;
	for (std::string::const_iterator i = src.begin(); i != src.end(); i++)
	{
		if (sep.find(*i) != std::string::npos)
		{
			if (s.length()) strList.push_back(s);
			s = "";
		}
		else
		{
			s += *i;
		}
	}
	if (s.length()) strList.push_back(s);
	return true;
}

bool StrSplit(std::string& src, std::list<uint32>& strList, std::string sep)
{
	std::string s;
	for (std::string::const_iterator i = src.begin(); i != src.end(); i++)
	{
		if (sep.find(*i) != std::string::npos)
		{
			if (s.length()) strList.push_back(Str2UInt(s.c_str()));
			s = "";
		}
		else
		{
			s += *i;
		}
	}
	if (s.length()) strList.push_back(Str2UInt(s.c_str()));
	return true;
}

bool StrSplit(CCharPtr src, std::list<std::string>& strList, std::string sep)
{
	std::string sr = src;
	return StrSplit(sr, strList, sep);
}

bool StrSplit(CCharPtr src, std::list<uint32>& strList, std::string sep)
{
	std::string sr = src;
	return StrSplit(sr, strList, sep);
}

bool MakeStr(std::list<uint32>& uList, std::string& des, std::string sep)
{
	BOOST_FOREACH(uint32 val, uList)
	{
		des += Int2Str(val);
		des += CONST_SEP;
	}
	return true;
}

bool MakeStr(std::list<std::string>& uList, std::string& des, std::string sep)
{
	BOOST_FOREACH(std::string& val, uList)
	{
		des += val;
		des += CONST_SEP;
	}
	return true;
}

bool MakeAddStr(std::string& res, std::string& des, uint32 add, bool zeroVoid)
{
	std::list<uint32> uList;
	StrSplit(res, uList);
	if (zeroVoid)
	{
		BOOST_FOREACH(uint32 val, uList)
		{
			if (val != 0)
			{
				des += Int2Str(val + add);
				des += CONST_SEP;
			}
		}
	}
	else
	{
		BOOST_FOREACH(uint32 val, uList)
		{
			des += Int2Str(val + add);
			des += CONST_SEP;
		}
	}

	return true;
}

bool MakeAddStr(const char* res, std::string& des, uint32 add, bool zeroVoid)
{
	if (!res)
	{
		return false;
	}

	std::string temp = res;
	std::list<uint32> uList;
	return MakeAddStr(temp, des, add, zeroVoid);
}
