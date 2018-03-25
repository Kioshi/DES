#include "main.h"
#include "types.h"
#include "tables.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <tuple>

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

template<size_t T, size_t V>
auto constexpr permute_r(std::bitset<T> const& orig, std::array<uint8, V> const& table)
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
auto constexpr permute(std::bitset<T> const& orig, std::array<uint8, V> const& table)
{
    std::bitset<V> permuted = 0;
    for (uint8 i = 0; i < table.size(); i++)
    {
        permuted[i] = orig[table[i] - 1];
    }
    return permuted;
}

template<size_t T>
std::tuple<size_t, size_t> constexpr divide(std::bitset<T> const& orig)
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
auto constexpr reverse(std::bitset<T> & bs)
{
    for (uint8 i = 0; i < bs.size() / 2; i++)
    {
        bool x = bs[i];
        bs.set(i,bs[T - i - 1]);
        bs.set(T - i - 1, x);
    }
}

auto keygen(bs64 const& k)
{
    bs64 key = k;
    reverse(key);
    auto permuted = permute(key, PC1);

    bs28 C, D;
    std::tie(C,D) = divide(permuted);

    Keys keys;
    for (uint8 i = 0; i < keys.size(); i++)
    {
        rotate(C, SHIFTS[i]);
        rotate(D, SHIFTS[i]);

        auto CD = merge(C, D);
        keys[i] = permute(CD, PC2);
    }

    return keys;
}

auto feistel(bs32 const& R, bs48 const& key)
{
    auto permuted = permute_r(R, EXPANSION);
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
    auto x = permute(out, PBOX);
    return x;
}

auto des(bs64 block, Keys keys, bool encrypt)
{
    reverse(block);
    auto permuted = permute_r(block, IP);
    bs32 L, R;
    std::tie(R, L) = divide(permuted);

    for (uint8 i = 0; i < 16; i++)
    {

        auto temp = R;
        R = L ^ feistel(R, encrypt ? keys[i] : keys[keys.size() - 1 - i]);
        L = temp;
    }

    bs64 RL = merge(L, R);
    auto x = permute_r(RL, FP);
    reverse(x);
    return x.to_ullong();
}

void main(int argc, char* argv[])
{
    std::string key = argc < 2 ? "64bitKey" : argv[1];
    std::string text = argc < 3 ? "secret text" : argv[2];

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
            std::cout << ((char*)&(x))[i - 1];

        }

    }
    std::cout << std::endl;
}
