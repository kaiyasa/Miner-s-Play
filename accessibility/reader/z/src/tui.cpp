
#include "tui"

#include <iostream>

#include <ncurses.h>

using namespace std;

struct Tui::data {
    WINDOW *main;
};

Tui::Tui()
  : d_ptr(new data())
{
return;
    if ((d_ptr->main = initscr()) == NULL) {
        cerr << "ncurses initization failed" << endl;
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(stdscr, TRUE);

    wtimeout(d_ptr->main, 100);
}

int Tui::getKeycode() {
    return wgetch(d_ptr->main);
}

string Tui::getKeyname(int keyCode)
{
    return string(keyname(keyCode));
}

Tui& Tui::moveTo(Point pos)
{
    return moveTo(pos.row, pos.col);
}

Tui& Tui::moveTo(int row, int col)
{
    wmove(d_ptr->main, row, col);
    return *this;
}

Tui& Tui::print(const char *text)
{
    waddstr(d_ptr->main, text);
    return *this;
}

Tui& Tui::print(const string& text)
{
    return print(text.c_str());
}

Tui::~Tui()
{
    if (d_ptr->main != NULL) {
        delwin(d_ptr->main);
        endwin();
        refresh();
    }
}

void Tui::refresh()
{
    wrefresh(d_ptr->main);
}
