
#include "ui.h"
#include "misc.h"

#include <ncurses.h>

void display(View &view) {
    clear();
    view.moveTo(0, 0);

    int offset = view.locate.offset;
    int last = min(view.buffer.size(), offset + view.bound.height());
    for(auto row = offset; row < last; ++row)
        view.print(row - offset, 0, view.buffer[offset + row]);
    view.moveTo();
}

void moveDown(View &view) {
    if (view.locate.lineIndex() == view.buffer.size() - 1)
        return;

    if (view.locate.row == view.bound.height() - 1) {
        ++view.locate.offset;

        view.moveTo(0, 0)
            .delln()
            .moveTo(view.locate.row, 0)
            .insln()
            .print(view.currentLine())
            .moveTo();
    } else {
        ++view.locate.row;
        ++view.cursor.row;
    }
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
