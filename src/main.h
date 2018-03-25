#pragma once
#include "types.h"

auto toArray(std::string const& text);

template<size_t T, size_t V>
auto constexpr permute_r(std::bitset<T> const& orig, std::array<uint8, V> const& table);

template<size_t T, size_t V>
auto constexpr permute(std::bitset<T> const& orig, std::array<uint8, V> const& table);

template<size_t T>
std::tuple<size_t, size_t> constexpr divide(std::bitset<T> const& orig);

template<size_t T>
void constexpr rotate(std::bitset<T> & set, uint8 positions);

template<size_t T>
auto constexpr merge(std::bitset<T> const& left, std::bitset<T> const& right);

template<size_t T>
auto constexpr reverse(std::bitset<T> & bs);

auto keygen(bs64 const& k);

auto feistel(bs32 const& R, bs48 const& key);

auto des(bs64 block, Keys keys, bool encrypt = true);
