
#include "ui.h"
#include "misc.h"

#include <ncurses.h>

void display(View &view) {
    clear();
    view.moveTo(0, 0);

    int last = min(view.buffer.size(), view.top + view.rows);
    for(auto row = view.top; row < last; ++row)
        view.print(row - view.top, view.xpos, view.buffer[row]);
    view.moveTo();
    refresh();
}

void moveDown(View &view) {
    if (view.location() == view.buffer.size() - 1)
        return;

    if (view.pos == view.rows - 1) {
        ++view.top;
        view.moveTo(0, 0)
            .delln()
            .moveTo(view.pos, 0)
            .insln()
            .print(view.currentLine())
            .moveTo();
    } else
        ++view.pos;
}

void moveUp(View &view) {
    if (view.location() == 0)
        return;

    if (view.pos > 0)
        --view.pos;
    else {
        --view.top;

        view.moveTo(view.rows - 1, 0)
            .delln()
            .moveTo(view.pos, 0)
            .insln()
            .print(view.currentLine())
            .moveTo();
    }
}
