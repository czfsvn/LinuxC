#include <iostream>
#include<sys/time.h>

using namespace std;

// from: https://www.toutiao.com/i6881862360945033741/

#define FUNC    std::cout << std::endl << __func__ << ", line: " << __LINE__ << std::endl;
#define BUFLEN  128

namespace ns_time_t
{
    // frunciton time_t time (time_t *t)
    // return: return the seconds form 1970.1.1 of utc time
    void main()
    {
        FUNC;
        time_t timep;
        long seconds = time(&timep);
        printf("%ld\n",seconds);
        printf("%ld\n",timep);
    }
}

namespace ns_ctime
{
    // char *ctime(const time_t *timep);
    void main()
    {
        time_t timep;

        time(&timep);
        FUNC;
        printf("ns_ctime::main: %s\n",ctime(&timep));
    }

}

namespace ns_gmtime
{
    void main()
    {
        char *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

        time_t timep;
        struct tm *p;

        time(&timep);
        p = gmtime(&timep);

        FUNC;
        printf("%d/%d/%d\n",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday);
        printf("%s %d:%d:%d\n",wday[p->tm_wday],p->tm_hour,p->tm_min,p->tm_sec);
    }
}

namespace ns_strftime
{
    void main()
    {
        time_t t = time( 0 );
        char tmpBuf[BUFLEN];

        FUNC;
        strftime(tmpBuf, BUFLEN, "%Y-%m-%d %H:%M:%S", localtime(&t)); //format date a
        printf("%s\n",tmpBuf);
    }
}

namespace ns_asctime
{
    void main()
    {
        time_t timep;

        time(&timep);
        FUNC;
        printf("%s\n",asctime(gmtime(&timep)));
    }
}

namespace ns_localtime
{
    void main()
    {
        char *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
        time_t timep;
        struct tm *p;

        time(&timep);
        p = localtime(&timep);
        FUNC;
        printf("%d/%d/%d ",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday);
        printf("%s %d:%d:%d\n",wday[p->tm_wday],p->tm_hour,p->tm_min,p->tm_sec);
    }
}

namespace ns_mktime
{
    void main()
    {
        time_t timep;
        struct tm *p;

        time(&timep);
        FUNC;
        printf("time():%ld\n",timep);
        p = localtime(&timep);
        timep = mktime(p);
        printf("time()->localtime()->mktime():%ld\n",timep);
    }
}

namespace ns_gettimeofday
{
    void main()
    {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv,&tz);
        FUNC;
        printf("tv_sec :%d\n",tv.tv_sec);
        printf("tv_usec: %d\n",tv.tv_usec);
        printf("tz_minuteswest:%d\n",tz.tz_minuteswest);
        printf("tz_dsttime:%d\n",tz.tz_dsttime);
    }
}

namespace ns_strptime
{
    void main()
    {
        std::string date_1 = "10-21 22:00:01";
        struct tm   res;
        strptime(date_1.c_str(), "%m-%d %H:%M:%S", &res);  // format date a

        time_t timep;
        time(&timep);

        time_t     now = time(nullptr);
        struct tm* p   = localtime(&now);
        timep = mktime(p);

        res.tm_year = p->tm_year;
        std::cout << "time: " << mktime(&res) << std::endl;

        return;
    }

} // namespace name


int main()
{
    std::cout << "Hello, time!\n";
    ns_time_t::main();
    ns_ctime::main();
    ns_gmtime::main();
    ns_strftime::main();
    ns_asctime::main();
    ns_localtime::main();
    ns_mktime::main();
    ns_gettimeofday::main();
    ns_strptime::main();
    return 1;
}


