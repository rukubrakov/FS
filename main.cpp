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


	int answ = is_equal_with_print(utf32_v,new_utf32_v,true);



	int utf8_pos = 0;
    int utf8_len = 0;
    cout << "For each letter let's see uint8 and uint32 versions for more tests:" << endl;
    for(int i =0; i <str_to_convert.length(); i++)
    {
		cout << "UTF32:"<< endl;
        cout << bitset<32>(utf32_v[i]) << endl;
        cout << "UTF32 -> UTF8:"<< endl;
        for(utf8_len = 1; (int)(utf8_v[utf8_pos] >> (8-utf8_len)) == 1; utf8_len++)
        {
        }
        for(int i = 0; i < utf8_len; i++)
        {
            cout << bitset<8>(utf8_v[utf8_pos + i]) << " ";
        }
        utf8_pos += utf8_len;
        printf("\n");
    }
    return 0;
}
