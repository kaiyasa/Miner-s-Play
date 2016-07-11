

#include "view.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <ncurses.h>

using namespace std;

View::View()
  : top(0), pos(0), xpos(0), y(0), rows(24), cols(80)
{
}

string& View::currentLine()
{
    return buffer[location()];
}

const string& View::currentLine() const
{
    return buffer[location()];
}

char View::currentChar() const
{
    return currentLine()[xpos];
}

int View::location() const
{
    return top + pos;
}

View& View::print(const string& text)
{
    waddstr(stdscr, text.c_str());
    return *this;
}

View& View::print(int r, int c, const string& text)
{
    mvwaddstr(stdscr, y + r, c, text.c_str());
    return *this;
}

View& View::printf(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    print(sprintf(va, fmt));
    va_end(va);
    return *this;
}

View& View::printf(int r, int c, const char *fmt, ...) {
    va_list va;

    va_start(va, fmt);
    print(r, c, sprintf(va, fmt));
    va_end(va);
    return *this;
}

string View::sprintf(va_list va, const char *fmt) {
    char buffer[2048];

    vsnprintf(buffer, sizeof(buffer), fmt, va);
    return string(buffer);
}

View& View::moveTo()
{
    int last = currentLine().empty() ? 0 : currentLine().size() - 1;
    return moveTo(Point(pos, min(xpos, last)));
}

View& View::moveTo(int row, int col)
{
    return moveTo(Point(row, col));
}

View& View::moveTo(const Point &pos)
{
    cursor = bound.top() + pos;
    wmove(stdscr, cursor.row, cursor.col);
    return *this;
}

View& View::insln()
{
    winsertln(stdscr);
    return *this;
}

View& View::delln()
{
    wdeleteln(stdscr);
    return *this;
}
