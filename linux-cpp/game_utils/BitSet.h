
#include <iostream>

using namespace std;

namespace ns_bit_sets
{
#define MAX_STATE 20
    class BitSets
    {
    private:
        /* data */
    public:
        BitSets(/* args */);
        ~BitSets();

        bool isSetBit(const uint32_t state) const;
        bool setBit(const uint32_t state);
        bool clearBit(const uint32_t state);
        void clearAll();

    private:
        byte bystates[(MAX_STATE + 7) / 8];
    };

    BitSets::BitSets(/* args */)
    {
        clearAll();
    }

    BitSets::~BitSets() {}

    void BitSets::clearAll()
    {
        bzero(bystates, sizeof(bystates));
    }

    bool BitSets::isSetBit(const uint32_t state) const
    {
        return 0 != (bystates[state / 8] & (0xff & (1 << (state % 8))));
    }

    bool BitSets::setBit(const uint32_t state)
    {
        bystates[state / 8] |= (0xff & (1 << (state % 8)));
    }
    bool BitSets::clearBit(const uint32_t state)
    {
        bystates[state / 8] &= (0xff & (~(1 << (state % 8))));
    }

    void main() {}
}  // namespace ns_bit_sets
