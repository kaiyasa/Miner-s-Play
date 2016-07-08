
#include <cstdio>

#include "io.h"

using namespace std;


FILE* read(const string& fname) {
    FILE *fp;

    if ((fp = fopen(fname.c_str(), "r")) == NULL) {
        perror(fname.c_str());
        exit(EXIT_FAILURE);
    }

    return fp;
}

vector<string> text(FILE *in) {
    auto read = [in]() { return fgetc(in); };

    vector<string> result;
    string data;

    for(auto ch = read(); ch != -1; ch = read()) {
        if (ch == '\r' || ch == '\n') {
            if (ch == '\r') {
                ch = read();
                if (ch != '\n')
                    ungetc(ch, in);
            }
            result.push_back(data);
            data.clear();
        } else
            data += ch;
    }

    if (!data.empty())
        result.push_back(data);

    return result;
}
