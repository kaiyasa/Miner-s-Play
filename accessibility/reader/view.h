
#ifndef __VIEW_H__
#define __VIEW_H__

#include <string>
#include <vector>

#include "viewport.h"

typedef std::vector<std::string> Lines;

class View {
  public:
    struct Locator {
        int offset;
        Point locate;

        int lineIndex() const { return offset + locate.row; }
    };

    int top, pos, xpos, y, rows, cols;
    Lines buffer;

    Viewport bound;
    Point cursor;
    Locator text;

//    Viewport bound(Point(), Point());

    View();

    std::string& currentLine();
    const std::string& currentLine() const;
    char currentChar() const;
    int location() const;

    View& print(const std::string& text);
    View& print(int r, int c, const std::string& text);
    View& printf(const char *fmt, ...);
    View& printf(int r, int c, const char *fmt, ...);

    std::string sprintf(va_list va, const char *fmt);

    View& moveTo();
    View& moveTo(int row, int col);
    View& moveTo(const Point &pos);

    View& insln();
    View& delln();

};

#endif
