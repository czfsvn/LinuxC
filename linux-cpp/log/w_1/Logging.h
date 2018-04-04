#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <sstream>
#include <iostream>

class LogMessage;

class LogFinisher
{
    public:
        void operator = (LogMessage& other);
};

class LogMessage 
{
    public:
        LogMessage(int level);
        ~LogMessage();

        /*
        LogMessage& operator<<(const std::string& value);
        LogMessage& operator<<(const char* value);
        LogMessage& operator<<(char value);
        LogMessage& operator<<(int value);
        //LogMessage& operator<<(uint value);
        LogMessage& operator<<(long value);
        LogMessage& operator<<(unsigned long value);
        LogMessage& operator<<(long long value);
        LogMessage& operator<<(unsigned long long value);
        LogMessage& operator<<(double value);
        LogMessage& operator<<(void* value);
        //LogMessage& operator<<(const StringPiece& value);
        //LogMessage& operator<<(const ::google::protobuf::util::Status& status);
        //LogMessage& operator<<(const uint128& value);
        */

        template <typename T>
            LogMessage& operator<<(T val)
            {
                oss << val;
                return *this;
            }


    private:
        friend class LogFinisher;
        void Finish();

        int level_;
        std::string message_;

        std::ostringstream oss;
};


#endif
