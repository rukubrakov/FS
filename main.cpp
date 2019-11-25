#include <bitset>
#include <iconv.h>
#include <iostream>
#include <stdexcept>
#include "converter.hpp"


using namespace std;

std::u32string str_to_convert = U"^ФС1LOVE";


int main()
{
    std::vector<uint32_t> utf32_v;
    int i;
    for (i = 0; i < str_to_convert.length(); i++)
    {
        utf32_v.push_back(static_cast<uint32_t>(str_to_convert[i]));
    }


    std::vector<uint8_t> utf8_v = to_utf8(utf32_v);
    std::vector<uint32_t> new_utf32_v = to_utf32(utf8_v);


	int answ = is_equal(utf32_v,new_utf32_v);

    return 0;
}
