#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <streambuf>

using Letters = int;

int main(int ac, char** av)
{
    std::string const name = (ac == 1) ? "/usr/share/dict/words" : av[1];
    std::ifstream fs;
    std::istream& in = (name != "-") ? (fs.open(name), fs) : std::cin;
    if (!in) {
        std::cerr << "file open failed, " << name << '\n';
        return 1;
    }

    std::vector<Letters> words;
    std::set<Letters, std::greater<Letters>> sevens;
    std::istream_iterator<std::string> it(in), end;

    std::for_each(it, end, [&](auto&& word) {
        if (word.size() >= 5) {
            Letters letters = std::accumulate(word.begin(), word.end(), 0,
                [](Letters a, char b) {
                    return (b < 'a' || b > 'z') ? -1 : a | (1 << ('z' - b));
                });
            if (letters > 0) {
                words.push_back(letters);
                if (__builtin_popcountl(letters) == 7)
                    sevens.insert(letters);
            }}});

    char buf[] = "aaaaaaa\n";
    for (Letters seven : sevens) {
        auto for_each_letter = [&seven](auto op) {
            int pos = 0;
            for (Letters rest = seven; rest != 0; ++pos, rest &= ~-rest)
                op(rest & -rest, pos);
        };
        int points[7] = { 0, };
        for (Letters word : words)
            if ((word & ~seven) == 0)
                for_each_letter([&](Letters letter, int rvpos) {
                    if (word & letter)
                        points[6 - rvpos] += (word == seven) ? 3 : 1;
                });
        bool any = false, mid;
        for_each_letter([&](Letters letter, int rvpos) {
            int pos = 6 - rvpos;
            any |= mid = (points[pos] > 20 && points[pos] < 33);
            buf[pos] = (mid? 'Z' : 'z') - __builtin_popcountl(letter - 1);
        });
        if (any)
            std::cout.rdbuf()->sputn(buf, 8);
    }
}
