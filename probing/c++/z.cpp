
#include "page"

#include <string>
#include <vector>
#include <utility>
#include <cassert>

#include <iostream>
using namespace std;

int main() {
    std::vector<Page> p;
    Page buf;
    std::string s("this is a test");
    std::copy(s.begin(), s.end(), buf.begin());
    buf.begin()[s.size()] = 0;

    auto b1 = buf.begin();

cout << "----" << endl;
    p.push_back(buf);
    auto b2 = p[0].begin();

cout << "----" << endl;
    p.push_back(std::move(buf));
    auto b3 = p[1].begin();

cout << "----" << endl;

}
