
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>

/*
    How text lines could flow over buffers.
  
         in-text - complete within buffer
       span-text - starts in buffer N and ends in N+j (j > 0)
 */

template <typename T>
class nth_iterator {
    int pos;
    T   *data;

    size_t translate(int n) const
      { return n >= 0 ? n : data->size() + n + 1; }

  public:
    typedef nth_iterator<T>       self_type;

    typedef typename T::reference reference;

    nth_iterator(int n, T *container)
      : pos(n), data(container)
    { }

    self_type& operator ++()
      { ++pos; return *this; }

    self_type  operator ++(int)
      { return self_type(pos + 1, data); }

    self_type& operator --()
      { --pos; return *this; }

    self_type  operator --(int)
      { return self_type(pos - 1, data); }

    bool operator !=(const self_type& that) const
      { return translate(pos) != translate(that.pos); }

    reference operator *()
      { return data->operator[](pos); }
};


class gapBuffer {
  public:
    typedef char               value_type;
    typedef value_type&        reference;
    typedef const value_type&  const_reference;
    typedef value_type*        pointer;
    typedef const value_type*  const_pointer;

    typedef nth_iterator<gapBuffer> iterator;

  protected:
    unsigned int presize, postsize;
    std::vector<value_type> text;

    void moveInsertionForward(int n);
    void moveInsertionBackward(int n);

    void expand(int suggest);

  public:
    iterator begin() { return iterator(0, this); }
    iterator end()   { return iterator(-1, this); }

    gapBuffer(size_t n);

    void reset(size_t n);
    reference operator [](size_t n);

    size_t size()     const { return presize + postsize; }
    size_t capacity() const { return text.size(); }
    size_t gap()      const { return capacity() - size(); }

    void moveInsertion(size_t pos);

    void insert(value_type ch);
    void insert(const std::string& s);
    void insert(const_pointer s);
    void insert(const_pointer s, size_t n);
};

typedef nth_iterator<gapBuffer> myIterator;

namespace std {
  template<>
  struct iterator_traits<myIterator> {
      typedef char                        value_type;
      typedef bidirectional_iterator_tag  iterator_category;
  };
};

gapBuffer::gapBuffer(size_t n) : presize(0), postsize(0), text(n)
{
}

void gapBuffer::reset(size_t n) {
    text.resize(n);
    presize = n;
    postsize = 0;
}

gapBuffer::reference gapBuffer::operator [](size_t n) {
    return text[n + (n < presize ? 0 : gap())];
}

void gapBuffer::expand(int suggest) {
    const int factor = 2048;

    // shift insertion to end of buffer
    auto current = presize;
    moveInsertion(size());

    // round total size to next sizing
    suggest += text.capacity();
    auto r = suggest % factor;
    suggest += (r == 0 ? 0 : factor - r);

    text.resize(suggest);

    // restore insertion point
    moveInsertion(current);
}

void gapBuffer::moveInsertion(size_t pos) {
    int distance = pos - presize;

    if (distance > 0)
        moveInsertionForward(distance);
    else
        moveInsertionBackward(-distance);
}

void gapBuffer::moveInsertionForward(int n) {
    /*
       pre: 4      post: 1
           1234^-5
               | |
               +-+
     */
    if (postsize == 0)
        return;

    for(int i = 0; i < n; ++i)
        text[presize++] = text[capacity() - postsize--];
}

void gapBuffer::moveInsertionBackward(int n) {
    /*
       pre: 1      post: 4
           1-^2345
           | |
           +-+
     */
    if (presize == 0)
        return;

    for(int i = 0; i < n; ++i)
        text[capacity() - ++postsize] = text[--presize];
}

void gapBuffer::insert(value_type ch) {
    if (size() == capacity())
        expand(1);

    text[presize++] = ch;
}

void gapBuffer::insert(const std::string& s) {
    insert(s.c_str(), s.size());
}

void gapBuffer::insert(const_pointer s) {
    size_t count = 0;
    while (s[count] != 0)
        ++count;
    insert(s, count);
}

void gapBuffer::insert(const_pointer s, size_t n) {
    if (size() + n > capacity())
        expand(n);

    for(int i = 0; i < n; ++i)
        insert(s[i]);
}

struct segment : gapBuffer {
    unsigned int first, last;         // line numbers

    segment() : gapBuffer(100), first(0), last(0)
    {}

    pointer c_buffer() { return &(*text.begin()); }
};

struct buffer {
    std::string name;
    std::vector<segment> seg;
};

class Loader {
    std::string name;
    std::fstream in;
    buffer buf;

  public:
    Loader(const std::string &fname) : name(fname), in(name)
    {}

    bool fetch() {
        if (!in)
            return false;

        segment seg;
        int nr = in.readsome(seg.c_buffer(), seg.capacity());
        if (!in || nr == 0)
            return false;

        seg.reset(nr);
        buf.seg.push_back(seg);
std::cout << seg.size() << std::endl;
        return true;
    }
};

using namespace std;

template <typename T>
T basename(T name) {
    T p = name;
    for(;              *p != 0  ; ++p);
    for(; p != name && *p != '/'; --p);
    return p;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "usage:\n\t" << basename(argv[0])
             << " filename" << endl;
        return EXIT_FAILURE;
    }

    string fname(argv[1]);
    Loader loader(fname);

    while (loader.fetch());
}

std::string toString(gapBuffer::iterator first, const gapBuffer::iterator& last) {
    std::string result;

    std::copy(first, last, std::back_inserter(result));
    return result;
}

template <class Container>
class gap_insert_iterator
  : public iterator<output_iterator_tag, void, void, void, void>
{
  protected:
    Container* container;

  public:
    typedef gap_insert_iterator<Container> self_type;
    typedef Container container_type;

    explicit gap_insert_iterator (Container& x)
      : container(&x)
    { }

    self_type& operator= (typename Container::const_reference value)
      { container->insert(value); return *this; }

    self_type& operator* ()
      { return *this; }

    self_type& operator++ ()
      { return *this; }

    self_type& operator++ (int)
      { return *this; }
};

template <typename T>
gap_insert_iterator<T> inserter(T& x) {
    return gap_insert_iterator<T>(x);
}

#if 0
int main() {
    string s("12345");

    for(int i = 0; i < s.size(); ++i) {
        gapBuffer buf(6);
        buf.insert(s);
        buf.moveInsertion(i);

        string r = toString(buf.begin(), buf.end());

        cout << s << " -- " << r << endl;
        assert(s == r);
    }

    for(int i = 0; i <= s.size(); ++i) {
        string t(s);
        t.insert(i, 1, '6');

        gapBuffer buf(6);
        buf.insert(s);
        buf.moveInsertion(i);
        buf.insert('6');

        string r = toString(buf.begin(), buf.end());

cout << t << " -- " << r << endl;
        assert(t == r);
    }

    {
        gapBuffer buf(0);
        copy(s.begin(), s.end(), inserter(buf));
        assert(buf.size() == s.size());
        string r = toString(buf.begin(), buf.end());
        cout << buf.capacity() << " " << r << endl;
    }
}
#endif
