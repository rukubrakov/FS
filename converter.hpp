#include <cstdint>
#include <bitset>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <assert.h>
using namespace std;

bool is_equal_with_print(std::vector<uint32_t> x, std::vector<uint32_t> y,bool is_print);
std::vector<uint8_t>to_utf8(const std::vector<uint32_t> &x);
std::vector<uint32_t>to_utf32(const std::vector<uint8_t> &x);
