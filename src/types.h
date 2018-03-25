#pragma once

#include <bitset>
#include <array>

using uint8 = uint8_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using bs64 = std::bitset<64>;
using bs56 = std::bitset<56>;
using bs48 = std::bitset<48>;
using bs32 = std::bitset<32>;
using bs28 = std::bitset<28>;

using Keys = std::array<bs48, 16>;
