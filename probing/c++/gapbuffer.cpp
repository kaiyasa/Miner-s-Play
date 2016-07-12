
#include <algorithm>
#include <utility>

#include "gapbuffer"


using namespace std;


void GapBuffer::deallocate() {
    delete [] _mem;
    _mem  = nullptr;
    _size = _presize = _postsize = 0;
}

GapBuffer::pointer GapBuffer::allocate(size_type n) {
    _size = n > 0 ? n : 0;
    _presize = _postsize = 0;
    return _mem = (_size > 0 ? new value_type[_size] : nullptr);
}

void GapBuffer::acquire(GapBuffer& other) {
    swap(_size,     other._size);
    swap(_presize,  other._presize);
    swap(_postsize, other._postsize);
    swap(_mem,      other._mem); 
}

void GapBuffer::copy(const GapBuffer& other) {
    std::copy(other.begin(), other.end(), begin());
}

GapBuffer::GapBuffer(GapBuffer::pointer, GapBuffer::size_type n, GapBuffer::size_type size) {
    allocate(n);
}

GapBuffer::~GapBuffer() {
    deallocate();
}

GapBuffer::GapBuffer(const GapBuffer& other) {
    allocate(other._size);
    copy(other);
}

GapBuffer::GapBuffer(GapBuffer&& other) {
    allocate(0);
    acquire(other);
}

GapBuffer& GapBuffer::operator =(const GapBuffer& other) {
    if (size() != other.size()) {
        deallocate();
        allocate(other.size());
    }
    copy(other);
    return *this;
}

GapBuffer& GapBuffer::operator =(GapBuffer&& other) {
    deallocate();
    acquire(other);
    return *this;
}

GapBuffer::pointer GapBuffer::begin() const {
    return _mem;
}

GapBuffer::pointer GapBuffer::end() const {
    return _mem + _size;
}

void GapBuffer::forwardInsertion() {
    if (_postsize > 0)
        _mem[_presize++] = _mem[size() - _postsize--];
}

void GapBuffer::backwardInsertion() {
    if (_presize > 0)
        _mem[size() - ++_postsize] = _mem[--_presize];
}

void GapBuffer::moveInsertion(int distance) {
    if (distance > 0)
        for(int i = 0; i < distance; ++i)
            forwardInsertion();
    else
        for(int i = 0; i < -distance; ++i)
            backwardInsertion();
}

GapBuffer& GapBuffer::insertionAt(GapBuffer::size_type pos) {
    moveInsertion(pos - _presize);
    return *this;
}

void expand() {
}

GapBuffer& GapBuffer::insert(const GapBuffer::value_type& c) {
    if (size() == capacity())
        expand();

    _mem[_presize++] = c;
    return *this;
}

GapBuffer& GapBuffer::insert(const string& data) {
    for(const auto& c : data)
        insert(c);
    return *this;
}

GapBuffer::size_type GapBuffer::size() const {
    return _presize + _postsize;
}

GapBuffer::size_type GapBuffer::capacity() const {
    return _size;
}
