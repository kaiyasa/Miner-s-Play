
#include <algorithm>
#include <utility>

#include "page"

#include <iostream>
using namespace std;

void Page::deallocate() {
    delete [] mem;
    mem  = nullptr;
    size = 0;
}

Page::pointer Page::allocate(size_type n) {
    size = n > 0 ? n : 0;
    return mem = (size > 0 ? new value_type[size] : nullptr);
}

void Page::acquire(Page& other) {
    std::swap(size, other.size);
    std::swap(mem,  other.mem); 
}

void Page::copy(const Page& other) {
    std::copy(other.begin(), other.end(), begin());
}

Page::Page(int n) {
cout << "ctor" << endl;
    allocate(n);
}

Page::~Page() {
    deallocate();
}

Page::Page(const Page& other) {
cout << "copy" << endl;
    allocate(other.size);
    copy(other);
}

Page::Page(Page&& other) {
cout << "move" << endl;
    allocate(0);
    acquire(other);
}

Page& Page::operator =(const Page& other) {
    if (size != other.size) {
        deallocate();
        allocate(other.size);
    }
    copy(other);
    return *this;
}

Page& Page::operator =(Page&& other) {
    deallocate();
    acquire(other);
    return *this;
}

Page::pointer Page::begin() const {
    return mem;
}

Page::pointer Page::end() const {
    return mem + size;
}

Page::size_type Page::limit(size_type n) {
    if (n > 0) {
        if (n < size)
            size = n;
    } else
        deallocate();
    return size;
}
