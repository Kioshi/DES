#include <iostream>
#include <bitset>
#include <tuple>
#include <array>
#include <vector>
#include <iomanip>


using uint8 = uint8_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using bs64 = std::bitset<64>;
using bs56 = std::bitset<56>;
using bs48 = std::bitset<48>;
using bs32 = std::bitset<32>;
using bs28 = std::bitset<28>;

using Keys = std::array<bs48, 16>;

auto toArray(std::string const& text)
{
    uint64 size = text.size() / 8 + !!(text.size() % 8);
    std::vector<uint64> textArr(size);

    for (uint64 i = 0; i < size; i++)
    {
        textArr[i] = 0;
        for (uint8 j = 0; j < 8; j++)
        {
            textArr[i] <<= 8;
            if (text.size() > i * 8 + j)
            {
                textArr[i] |= text[i * 8 + j];
            }
        }
    }
    return textArr;
}

static const std::array<uint8,56> PC1 =
{
    57, 49, 41, 33, 25, 17,  9,
    1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,

    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};
static const std::array<uint8, 48> PC2 =
{
    14, 17, 11, 24,  1,  5,
    3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};
static const std::array<uint8, 16> SHIFTS =
{
    1,  1,  2,  2,  2,  2,  2,  2,  1,  2,  2,  2,  2,  2,  2,  1
};

static const std::array<uint8, 64> IP =
{
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};


static const std::array<uint8, 48> EXPANSION =
{
    32,  1,  2,  3,  4,  5,
    4,  5,  6,  7,  8,  9,
    8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

static const std::array<uint8,64> SBOX[8] =
{
    {
        // S1
        14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
        0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
        4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
        15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
    },
    {
        // S2
        15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
        3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
        0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
        13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
    },
    {
        // S3
        10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
        13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
        13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
        1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
    },
    {
        // S4
        7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
        13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
        10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
        3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
    },
    {
        // S5
        2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
        14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
        4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
        11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
    },
    {
        // S6
        12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
        10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
        9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
        4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
    },
    {
        // S7
        4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
        13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
        1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
        6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
    },
    {
        // S8
        13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
        1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
        7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
        2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
    }
};

static const std::array<uint8, 32> PBOX =
{
    16,  7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
};

static const std::array<uint8, 64> FP =
{
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};

template<size_t T, size_t V>
auto constexpr permute(std::bitset<T> const& orig, std::array<uint8, V> const& table)
{
    std::bitset<V> permuted = 0;
    for (uint8 i = 0; i < table.size(); i++)
    {
        if (orig[orig.size() - table[i]])
            permuted.set(permuted.size() - i - 1);
    }
    return permuted;
}

template<size_t T, size_t V>
auto constexpr permute2(std::bitset<T> const& orig, std::array<uint8, V> const& table)
{
    std::bitset<V> permuted = 0;
    for (uint8 i = 0; i < table.size(); i++)
    {
        permuted[i] = orig[table[i] - 1];
    }
    return permuted;
}

template<size_t T>
std::tuple<uint64,uint64> constexpr divide(std::bitset<T> const& orig)
{
    std::bitset<T> divider(uint64(-1) >> T / 2);
    size_t rightKey = (orig & divider).to_ullong();
    size_t leftKey = ((orig >> (T / 2)) & divider).to_ullong();
    return { leftKey, rightKey };
}

template<size_t T>
void constexpr rotate(std::bitset<T> & set, uint8 positions)
{
    for (uint8 i = 0; i < positions; i++)
    {
        bool x = set[0];
        set >>= 1;
        set.set(set.size() - 1, x);
    }
}

template<size_t T>
auto constexpr merge(std::bitset<T> const& left, std::bitset<T> const& right)
{
    return std::bitset<T * 2>((left.to_ullong() << right.size()) | right.to_ullong());
}

template<size_t T>
auto reverse(std::bitset<T> & bs)
{
    for (uint8 i = 0; i < bs.size() / 2; i++)
    {
        bool x = bs[i];
        bs.set(i,bs[T - i - 1]);
        bs.set(T - i - 1, x);
    }
}

auto keygen(bs64 const& key)
{
    auto permuted = permute(key, PC1);
    reverse(permuted);
    bs28 C, D;
    std::tie(C, D) = divide(permuted);

    Keys keys;
    for (uint8 i = 0; i < keys.size(); i++)
    {
        rotate(C, SHIFTS[i]);
        rotate(D, SHIFTS[i]);

        auto CD = merge(C, D);
        reverse(CD);
        keys[i] = permute(CD, PC2);
        reverse(keys[i]);
    }

    return keys;
}


auto feistel(bs32 const& R, bs48 const& key)
{
    auto permuted = permute(R, EXPANSION);
    // XOR
    permuted = permuted ^ key;
    reverse(permuted);
    bs32 out(0);
    for (uint8 i = 0; i < 8; i++)
    {
        // First and last bit of 6bit block make row
        auto row = (uint8)(permuted[permuted.size() - 1 - (i * 6)] << 1) | (uint8)permuted[permuted.size() - 1 - 5 - (i * 6)];
        // Middle four bits make column
        uint8 column = 0;
        for (uint8 j = 0; j < 4; j++)
            column |= permuted[permuted.size() - 1 - 1 - j - (i * 6)] << (3 - j);


        out <<= 4;
        out |= SBOX[i][16 * row + column];
    }

    reverse(out);
    auto x = permute2(out, PBOX);
    return x;
}

auto des(bs64 block, Keys keys, bool encrypt = true)
{
    reverse(block);
    auto permuted = permute(block, IP);
    bs32 L, R;
    std::tie(R, L) = divide(permuted);



    for (uint8 i = 0; i < 16; i++)
    {

        auto temp = R;
        R = L ^ feistel(R, encrypt ? keys[i] : keys[keys.size() - 1 - i]);
        L = temp;
    }

    bs64 RL = merge(L, R);

    auto x = permute(RL, FP);
    reverse(x);
    return x.to_ullong();
}

void main()
{
    std::string key = "64bitKey";
    std::string text = "secret text";

    auto keyArr = toArray(key);
    auto textArr = toArray(text);

    auto keys = keygen(keyArr[0]);
    std::vector<uint64> crypted;

    for (auto const& block : textArr)
    {
        uint64 x = des(block, keys);
        crypted.push_back(x);
        for (uint8 i = 8; i > 0; i--)
        {
            int xx = static_cast<int>(((uint8*)&(x))[i - 1]);
            std::cout << std::setfill('0') << std::setw(2) << std::hex << xx;
        }

    }
    std::cout << std::endl;

    for (auto block : crypted)
    {
        uint64 x = des(block, keys, false);
        for (uint8 i = 8; i > 0; i--)
        {
            std::cout << ((char*)&(x))[i-1];

        }

    }
    std::cout << std::endl;
}
