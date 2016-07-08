
#ifndef __DESCRIPTION_H__
#define __DESCRIPTION_H__

#include <string>

#include <vector>

struct Description {
    struct Marker {
        int size;
        std::string utter;
    };

    int location;
    std::string all;
    std::vector<Marker> text;
};

#endif
