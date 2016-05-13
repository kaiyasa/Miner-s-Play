

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    if (argc > 1)
        cout << "hello " << argv[1] << endl;
    else
        cout << "hello world" << endl;
    return 0;
}
