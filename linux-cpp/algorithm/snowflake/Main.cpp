#include <chrono>
#include <iostream>
#include <thread>
using namespace std;

namespace ns_snow
{
#ifdef SNOWFLAKE_ID_WORKER_NO_LOCK
    typedef std::atomic<uint64_t> AtomicUInt64;
#else
    typedef uint64_t AtomicUInt64;
#endif

    using namespace std::chrono;
    // 1  bit  符号位
    // 41 bit  时间戳
    // 5  bit  机房
    // 5  bit  机器ID
    // 12 bit  序列号-不重复
    class SnowFlake
    {
    public:
        SnowFlake(const uint64_t &host, const uint64_t &work, const uint64_t &sq) : host_id(host), work_id(work), sequence(sq)
        {
        }

        AtomicUInt64 GetTimeStamp() const
        {
            return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        }

        AtomicUInt64 TilNextMills(const uint64_t last_time)
        {
            uint64_t timestamp = GetTimeStamp();
            while (timestamp <= lastTimestamp)
                timestamp = GetTimeStamp();
            return timestamp;
        }

        AtomicUInt64 NextID()
        {
#ifdef SNOWFLAKE_ID_WORKER_NO_LOCK
            std::unique_lock<std::mutex> lock{mtx};
            static AtomicUInt64 now{0};
#else
            static AtomicUInt64 now{0};
#endif

            now = GetTimeStamp();
            if (now < lastTimestamp)
            {
                return 0;
            }

            if (now == lastTimestamp)
            {
                sequence = (sequence + 1) & sequenceMask;
                if (sequence == 0)
                {
                    now = TilNextMills(lastTimestamp);
                }
            }
            else
            {
                sequence = 0;
            }
#ifdef SNOWFLAKE_ID_WORKER_NO_LOCK
            lastTimestamp = now.load();
#else
            lastTimestamp = now;
#endif
            return ((now - twepoch) << timestampLeftShift) | (host_id << hostIdShift) | (work_id << workerIdShift) | sequence;
        }

    private:
        // 开始时间截
        const static AtomicUInt64 twepoch = 1288834974657L;

        // worker 所占的位数
        const static AtomicUInt64 workerIdBits = 5L;
        // host 所占位数
        const static AtomicUInt64 hostIdBits = 5L;
        // 序列所占的位数
        const static AtomicUInt64 sequenceBits = 12L;
        // 时间戳所占位数
        const static AtomicUInt64 timeBits = 41L;
        // worker 最大值
        const static AtomicUInt64 maxWorkerId = -1L ^ (-1L << workerIdBits);
        // data 最大值
        const static AtomicUInt64 maxHostId = -1L ^ (-1L << hostIdBits);
        // 序列号最大值
        const static AtomicUInt64 sequenceMask = -1L ^ (-1L << sequenceBits);

        // 工作id需要左移的位数，12位
        const static AtomicUInt64 workerIdShift = sequenceBits;
        // 数据id需要左移位数 12 + 5 = 17位
        const static AtomicUInt64 hostIdShift = sequenceBits + workerIdBits;
        // 时间戳需要左移位数 12+5+5=22位
        const static AtomicUInt64 timestampLeftShift = sequenceBits + workerIdBits + hostIdBits;

        AtomicUInt64 lastTimestamp = 0L;
        AtomicUInt64 sequence{0};

        uint64_t host_id{0};
        uint64_t work_id{0};

#ifdef SNOWFLAKE_ID_WORKER_NO_LOCK
        std::mutex mtx;
#endif
    };

    void test(uint32_t host, uint32_t work, uint32_t sq)
    {
        SnowFlake flake(host, work, sq);
        for (uint16_t idx = 0; idx < 10; idx++)
        {
            std::cout << "idx: " << idx << ", value: " << flake.NextID() << std::endl;
            std::this_thread::sleep_for(1ms);
        }

        std::cout << std::endl;
    }
    void main()
    {
        test(2, 2, 2);
        test(2, 3, 1);
    }
} // namespace ns_snow

namespace ns_test1
{
    using namespace std::chrono;
    void main()
    {
        time_point tp = system_clock::now();
        //uint64_t micro = std::chrono::duration_cast<std::chrono::microseconds>(tp);
        uint64_t now = system_clock::to_time_t(tp);
        std::cout << "now : " << now << std::endl;

        std::cout << "now : " << duration_cast<milliseconds>(tp.time_since_epoch()).count() << std::endl;
        std::cout << "now : " << duration_cast<microseconds>(tp.time_since_epoch()).count() << std::endl;
    }
} // namespace ns_test1

int main()
{
    ns_test1::main();
    ns_snow::main();
    std::cout << "Hello, snowflake\n";
    return 0;
}