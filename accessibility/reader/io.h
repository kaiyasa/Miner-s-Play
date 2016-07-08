
#ifndef __IO_H__
#define __IO_H__

#include <string>
#include <vector>

FILE* read(const std::string& fname);
std::vector<std::string> text(FILE *in);

#endif
