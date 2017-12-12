#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
    std::string char_to_hex(unsigned char chr)
    {
        auto f = [](unsigned char c) {
            if (c < 10)
                return c + '0';
            else
                return c - 10 + 'a';
        };

        std::string res;
        res += f(chr / 16);
        res += f(chr % 16);

        return res;
    }
}

template <typename TContainer>
class circular_iterator
{
    typename TContainer::iterator it;
    int distance;
    int max;

public:

    circular_iterator(TContainer& con) :
        it(con.begin()),
        distance(0),
        max(con.size())
    {
    }

    void operator+=(int offset)
    {
        const int new_position{ (distance + offset) % max };
        std::advance(it, new_position - distance);
        distance = new_position;
    }

    circular_iterator& operator++()
    {
        *this += 1;
        return *this;
    }

    circular_iterator operator++(int)
    {
        circular_iterator copy{ *this };
        ++*this;
        return copy;
    }

    void operator-=(int offset)
    {
        const int new_position{ ((distance - offset) + (((distance - offset) < 0) ? max : 0 )) % max };
        std::advance(it, new_position - distance);
        distance = new_position;
    }

    circular_iterator& operator--()
    {
        *this -= 1;
        return *this;
    }
    
    circular_iterator operator--(int)
    {
        circular_iterator copy{ *this };
        --*this;
        return copy;
    }

    bool operator==(const circular_iterator<TContainer>& rhs)
    {
        typename TContainer::iterator
            l_copy{ it },
            r_copy{ rhs.it };

        l_copy -= distance;
        r_copy -= rhs.distance;

        return l_copy == r_copy;
    }

    typename TContainer::value_type& operator*()
    {
        return *it;
    }

    typename TContainer::value_type* operator->()
    {
        return &*it;
    }

};

template <int LIST_SIZE>
class wrinkling_list
{
    std::array<int, LIST_SIZE> list;
    circular_iterator<std::array<int, LIST_SIZE>> current_position;
    int skip_size;

public:

    wrinkling_list() : 
        current_position(list),
        skip_size(0)
    {
        auto it{ current_position };
        for (int i = 0; i < LIST_SIZE; ++it, ++i)
        {
            *it = i;
        }
    }

    void wrinkle(int length)
    {
        circular_iterator<std::array<int, LIST_SIZE>>
            r_begin{ current_position },
            r_end{ current_position };

        r_end += length - 1;
        for (int i = 0; i < length / 2; ++i, ++r_begin, --r_end)
        {
            int temp = *r_begin;
            *r_begin = *r_end;
            *r_end = temp;
         }

        current_position += length + skip_size;
        ++skip_size;
    }

    int first_two_product()
    {
        return *list.begin() * *std::next(list.begin());
    }

    std::string produce_hash();
};

std::string wrinkling_list<256>::produce_hash()
{
    std::vector<int> xored;

    auto it{ list.begin() };
    for (int i = 0; i < 16; ++i)
    {
        int xord{ 0 };
        for (int j = 0; j < 16; ++j, ++it)
        {
            xord ^= *it;
        }
        xored.push_back(xord);
    }

    std::string hash;
    for (const auto xord : xored)
    {
        hash += char_to_hex(xord);
    }

    return hash;
}

std::string test(const std::string& filename)
{
    std::ifstream input_file{ filename };
    std::string line;
    std::getline(input_file, line);

    wrinkling_list<256> list;

    for (int i = 0; i < 64; ++i)
    {
        for (const auto c : line)
        {
            list.wrinkle(c);
        }

        list.wrinkle(17);
        list.wrinkle(31);
        list.wrinkle(73);
        list.wrinkle(47);
        list.wrinkle(23);
    }

    return list.produce_hash();
}

int main()
{
    std::cout << test("input.txt");
    system("PAUSE > nul");
}
