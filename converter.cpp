#include "converter.hpp"


uint8_t n_last_bits(uint32_t x, int n)//func returns last n digits as uint8_t
{
	assert(n == 6);
	return 0b10000000 + (uint8_t)(x & 0b00111111u);
}

bool is_equal_with_print(std::vector<uint32_t> x, std::vector<uint32_t> y,bool is_print)//Checking whether 2 uint32 vectors are the same
{
	cout << "utf32_v vs new_utf32_v" << endl;
	int i;
	if (is_print)
	{
		for (i = 0; i < x.size(); ++i)
		{
			cout << "old "<< bitset<32>(x[i]) << "\n"<< "new " << bitset<32>(y[i]) << endl;
			printf("\n");
		}
	}
    if (x != y)
    {
		if (is_print)
			cout << "Vectors are not equal" << endl;
        return false;
    }
    else
    {
		if (is_print)
			cout << endl << "Vectors equal" << endl;   
		return true;
	}
}

void utf8_from_utf32(std::uint32_t utf32, std::vector<uint8_t> &acceptor)//converter from uint32_t to uint8_t 
{
    if (utf32 <= (uint32_t)0x7f) //it means that the number can be coded as 0XXXXXXX (we just push last 7 bits instead of 'X's )
        acceptor.push_back((uint8_t) utf32);
        
    else if (utf32 <= (uint32_t)0x7ff) //it means that the number can be coded as 110XXXXX 10XXXXXX(we just push 11 bits instead of 'X's:)

    {
        acceptor.push_back((uint8_t)(0b11000000 + (uint8_t)(utf32 >> 6u))); //110XXXXX 10XXXXXX 
        acceptor.push_back(n_last_bits(utf32,6));
    }
    else if (utf32 <= (uint32_t)0xffff)
    {
        acceptor.push_back((uint8_t)(0b11100000 + (uint8_t)(utf32 >> 12u))); //1110XXXX 10XXXXXX 10XXXXXX
        acceptor.push_back(n_last_bits(utf32 >> 6u,6));
        acceptor.push_back(n_last_bits(utf32,6));
    }
    else if (utf32 <= (uint32_t)0x1fffff)
    {
        acceptor.push_back((uint8_t)(0b11110000 + (uint8_t)(utf32 >> 18u))); //11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
        acceptor.push_back(n_last_bits(utf32 >> 12u,6));
        acceptor.push_back(n_last_bits(utf32 >> 6u,6));
        acceptor.push_back(n_last_bits(utf32,6));
    }
    else if (utf32 <= (uint32_t)0x3ffffff)//111110XX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX
    {
        acceptor.push_back((uint8_t)(0b11111000 + (uint8_t)(utf32 >> 24u)));
        acceptor.push_back(n_last_bits(utf32 >> 18u,6));
        acceptor.push_back(n_last_bits(utf32 >> 12u,6));
        acceptor.push_back(n_last_bits(utf32 >> 6u,6));
        acceptor.push_back(n_last_bits(utf32,6));
    }
    else if (utf32 <= (uint32_t)0x7fffffff)//1111110X 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX
    {
        acceptor.push_back((uint8_t)(0b11111000 + (uint8_t)(utf32 >> 30u)));
        acceptor.push_back(n_last_bits(utf32 >> 24u,6));
        acceptor.push_back(n_last_bits(utf32 >> 18u,6));
        acceptor.push_back(n_last_bits(utf32 >> 12u,6));
        acceptor.push_back(n_last_bits(utf32 >> 6u,6));
        acceptor.push_back(n_last_bits(utf32,6));
    }
}


std::vector<uint8_t>to_utf8(const std::vector<uint32_t> &utf32_str)// converts string(vector<uint32_t>) of utf32 to vector<uint8_t>
{
    std::vector<uint8_t> out;
    int i;
    for (i = 0; i < utf32_str.size(); ++i)
    {
        utf8_from_utf32(utf32_str[i], out);
    }
    return out;
}

std::vector<uint32_t>to_utf32(const std::vector<uint8_t> &utf8_string)//converts vector<uint8_t> to string(vector<uint32_t>) of utf32

{
    std::vector<uint32_t> out;
    uint32_t tmp = 0;
	int i,j;
    for (i = 0; i < utf8_string.size();)
    {
		if ((utf8_string[i] >> 1u) == 0b01111110u)
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00000111u) << 30u;
            for (j = 0; j < 5; ++j)
            {
                if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u)) 
                    tmp += ((uint32_t)utf8_string[i++] & 0b00111111u)<< (4 - j)*6u;
                else{
                    throw std::runtime_error("Eight (byte) has a wrong begining (not 10)!");
                }
            }
        }
		if ((utf8_string[i] >> 2u) == 0b00111110u)//this case means that there starts the parsing of group with 5 eights and the first one start with 111110
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00000111u) << 24u; //shifting bits of 5th eight by 24, to set the bits of another eights
            for (j = 0; j < 4; ++j)
            {
                if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u)) //moving through another 4 eights, if first 2 bits are 10, then put another 6 bits of the eight to tmp, shifting by 18,12,6 and 0 bits 
                    tmp += ((uint32_t)utf8_string[i++] & 0b00111111u)<< (3 - j)*6u;
                else{
                    throw std::runtime_error("Eight (byte) has a wrong begining (not 10)!");
                }
            }
        }
        if ((utf8_string[i] >> 3u) == 0b00011110u)//this case means that there starts the parsing of group with 4 eights and the first one start with 11110
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00000111u) << 18u; //shifting bits of 4th eight by 18, to set the bits of another eights
            for (j = 0; j < 3; ++j)
            {
                if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u)) //moving through another 3 eights, if first 2 bits are 10, then put another 6 bits of the eight to tmp, shifting by 12,6 and 0 bits 
                    tmp += ((uint32_t)utf8_string[i++] & 0b00111111u)<< (2 - j)*6u;
                else{
                    throw std::runtime_error("Eight (byte) has a wrong begining (not 10)!");
                }
            }
        }
        else if ((utf8_string[i] >> 4u) == 0b0001110u)
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00001111u) << 12u;

            for (j = 0; j < 2; ++j) {
                if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u))
                    tmp += ((uint32_t)utf8_string[i++] & 0b00111111u)
                            << (1 - j)*6u;
                else 
                    throw std::runtime_error("Eight (byte) has a wrong begining (not 10)!");
            }
        }
        else if ((utf8_string[i] >> 5u) == 0b000110u)
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00011111u) << 6u;
            if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u))
                tmp += ((uint32_t)utf8_string[i++] & 0b00111111u);
            else 
                throw std::runtime_error("Eight (byte) has a wrong begining (not 10)!");
        }
        else
            tmp += (uint32_t)utf8_string[i++];
        out.push_back(tmp);
        tmp = 0;
    }

    return out;
}
