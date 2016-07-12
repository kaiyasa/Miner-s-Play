
#include "gapbuffer"

#include <iostream>
#include <string>
#include <vector>
#include <utility>


using namespace std;


std::string expected("this is a test");

int main() {
    GapBuffer buf(new char[10], 0, 10);
}
