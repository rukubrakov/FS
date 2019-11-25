#include "converter.hpp"


uint8_t n_last_bits(uint32_t x, int n)//преобразуем последние n цифр октавы к формату uint8_t
{
	if (n == 6) 
		return 0b10000000 + (uint8_t)(x & 0b00111111u);
}

bool is_equal(std::vector<uint32_t> x, std::vector<uint32_t> y)//Проверка 2 векторов uint32 на эквивалентность
{
	cout << "utf32_v vs new_utf32_v" << endl;
	int i;
    for (i = 0; i < x.size(); ++i)
    {
        cout << "old "<< bitset<32>(x[i]) << "\n"<< "new " << bitset<32>(y[i]) << endl;
        printf("\n");
    }

    if (x != y)
    {
        cout << "Vectors are not equal" << endl;
        return false;
    }
    else
    {
        cout << endl << "Vectors equal" << endl;   
		return true;
	}
}

void utf8_from_utf32(std::uint32_t utf32, std::vector<uint8_t> &acceptor)//Фактически функция конвертации из uint32_t в uint8_t
{
    if (utf32 <= (uint32_t)0x7f) //выполнение этого условия, согласно википедии, означает, что число удастся закодировать 7 битами, или 1 октавой с учетом первого символа 0
        acceptor.push_back((uint8_t) utf32);
        
    else if (utf32 <= (uint32_t)0x7ff) //невыполнение предыдущего, но выполнение этого означает, что 11 бит нужно для кодировки числа или 2 октавы

    {
        acceptor.push_back((uint8_t)(0b11000000 + (uint8_t)(utf32 >> 6u))); //по договоренности первые 3 символа первой октавы - 110 к ним прибавляем кусок, который вылазит за 6 бит
        acceptor.push_back(n_last_bits(utf32,6));
    }
    else if (utf32 <= (uint32_t)0xffff)
    {
        acceptor.push_back((uint8_t)(0b11100000 + (uint8_t)(utf32 >> 12u))); //аналогично, только теперь первые биты - 1110
        acceptor.push_back(n_last_bits(utf32 >> 6u,6));
        acceptor.push_back(n_last_bits(utf32,6));
    }
    else if (utf32 <= (uint32_t)0x10ffff)
    {
        acceptor.push_back((uint8_t)(0b11110000 + (uint8_t)(utf32 >> 18u))); //аналогично, только теперь первые биты - 1110
        acceptor.push_back(n_last_bits(utf32 >> 12u,6));
        acceptor.push_back(n_last_bits(utf32 >> 6u,6));
        acceptor.push_back(n_last_bits(utf32,6));
    }
}


std::vector<uint8_t>to_utf8(const std::vector<uint32_t> &utf32_str)// конвертируем строку <uint32_t> в vector<uint8_t>
{
    std::vector<uint8_t> out;
    int i;
    for (i = 0; i < utf32_str.size(); ++i)
    {
        utf8_from_utf32(utf32_str[i], out);
    }
    return out;
}

std::vector<uint32_t>to_utf32(const std::vector<uint8_t> &utf8_string)//преобразование в обратную сторону

{
    std::vector<uint32_t> out;
    uint32_t tmp = 0;
	int i,j;
    for (i = 0; i < utf8_string.size();)
    {
        if ((utf8_string[i] >> 3u) == 0b00011110u)//выполнение будет означать .что перед нами первая из 4 октав, так как именно она начинается с 11110
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00000111u) << 18u; //сдвигаем значащие биты 4 октавы на 18, чтобы установить биты других октав
            for (j = 0u; j < 3u; ++j)
            {
                if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u)) //движемся по другим 3 октавам, если первые 2 бита 10, то записываем остальные 6 в tmp, сдвигая на 12, 6 и 0 битов соответсвенно
                    tmp += ((uint32_t)utf8_string[i++] & 0b00111111u)<< (2 - j)*6u;
                else{
                    throw std::runtime_error("No 10");//если октавы имеют неправильную структуру - не начинаются с 10, будет ошибка
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
                    throw std::runtime_error("No 10");
            }
        }
        else if ((utf8_string[i] >> 5u) == 0b000110u)
        {
            tmp += ((uint32_t)utf8_string[i++] & 0b00011111u) << 6u;
            if ((utf8_string[i] & 128u) && !(utf8_string[i] & 64u))
                tmp += ((uint32_t)utf8_string[i++] & 0b00111111u);
            else 
                throw std::runtime_error("No 10");
        }
        else
            tmp += (uint32_t)utf8_string[i++];
        out.push_back(tmp);
        tmp = 0;
    }

    return out;
}
