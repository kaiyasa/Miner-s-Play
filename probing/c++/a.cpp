

#include <iostream>
#include <vector>
#include <algorithm>


using namespace std;

int main(void) {
    std::vector<int> a(1<<20);

    cout << a.capacity() << " " << a.size() << endl;
    a.push_back(1);
    a.push_back(1);
    a.push_back(1);
    a.push_back(1);
    cout << a.capacity() << " " << a.size() << endl;
}
