#include <iostream>
#include <cstdio>
#include "Logging.h"

using namespace std;

#define GOOGLE_LOG(LEVEL)                                                 \
        LogFinisher() =                           \
        LogMessage(LEVEL)                            

#define DECLARE_STREAM_OPERATOR(TYPE)                   \
    LogMessage& LogMessage::operator<<(TYPE value)  {   \
        oss << value;                                   \
        return *this;                                }


/*
#define DECLARE_STREAM_OPERATOR(TYPE, FORMAT)           \
        LogMessage& LogMessage::operator<<(TYPE value) {                  \
            char buffer[128];                                               \
            snprintf(buffer, sizeof(buffer), FORMAT, value);                \
            buffer[sizeof(buffer)-1] = '\0';                                \
            message_ += buffer;                                             \
            return *this;                                                   \
        }


DECLARE_STREAM_OPERATOR(char         , "%c" )
DECLARE_STREAM_OPERATOR(int          , "%d" )
DECLARE_STREAM_OPERATOR(long         , "%ld")
DECLARE_STREAM_OPERATOR(unsigned long, "%lu")
DECLARE_STREAM_OPERATOR(double       , "%g" )
DECLARE_STREAM_OPERATOR(void*        , "%p" )
DECLARE_STREAM_OPERATOR(unsigned int , "%u" )

LogMessage& LogMessage::operator<<(const string& value) {
    message_ += value;
    return *this;
}

LogMessage& LogMessage::operator<<(const char* value) {
    message_ += value;
    return *this;
}

*/
/*
LogMessage& LogMessage::operator<<(const StringPiece& value) {
    message_ += value.ToString();
    return *this;
}

LogMessage& LogMessage::operator<<(
        const ::google::protobuf::util::Status& status) {
    message_ += status.ToString();
    return *this;
}

LogMessage& LogMessage::operator<<(const uint128& value) {
    std::ostringstream str;
    str << value;
    message_ += str.str();
    return *this;
}

DECLARE_STREAM_OPERATOR(const std::string&);
DECLARE_STREAM_OPERATOR(const char*);
DECLARE_STREAM_OPERATOR(char);
DECLARE_STREAM_OPERATOR(int);
DECLARE_STREAM_OPERATOR(long);
DECLARE_STREAM_OPERATOR(unsigned long);
DECLARE_STREAM_OPERATOR(double);
*/

LogMessage::LogMessage(int lv) : level_(lv) 
{

}
LogMessage::~LogMessage() 
{
}

void LogMessage::Finish() 
{
    std::cout << "level: " << level_ << ", msg: " << oss.str() << std::endl;
}

void LogFinisher::operator = (LogMessage& other)
{   
    other.Finish();
}

void Test_Log()
{
    GOOGLE_LOG(1) << 2;
    GOOGLE_LOG(1) << "test" << 123 << "";
    GOOGLE_LOG(2) << "test" << 223 << "";
    GOOGLE_LOG(3) << "test" << 323 << "";
    GOOGLE_LOG(4) << "test" << 423 << "";
}

int main()
{
    std::cout << "Hello, World!" << std::endl;
    Test_Log();
    return 0;
}
