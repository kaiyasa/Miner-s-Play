
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <ncurses.h>

#include <string>
#include <vector>

#include <speech-dispatcher/libspeechd.h>

typedef std::vector<std::string> Lines;

using namespace std;

Lines create() {
    char line[100];
    Lines result;

    for(int i = 0; i < 100; ++i) {
        snprintf(line, 100, "line %d", i + 1);
        result.push_back(string(line));
    }

    return result;
}

typedef struct View {
    View() : top(0), pos(0), rows(24)
    {}

    int top, pos, rows;
    Lines buffer;
} View;

void display(const View &view) {
    for(auto row = view.top; row < view.top + view.rows - 1; ++row)
        mvaddstr(row, 0, view.buffer[row].c_str());
    move(view.top + view.pos, 0);
    refresh();
}

void moveDown(View &view) {
    if (view.top + view.pos == view.buffer.size() - 1)
        return;

    if (view.pos < view.rows - 2)
        ++view.pos;
    else {
        ++view.top;
        move(view.rows - 1, 0);
        deleteln();

        move(0, 0);
        insdelln(-1);

        mvaddstr(view.pos, 0, view.buffer[view.top + view.pos].c_str());
    }
}

void moveUp(View &view) {
    if (view.top + view.pos == 0)
        return;

    if (view.pos > 0)
        --view.pos;
    else
        if (view.top > 0) {
            --view.top;

            move(0, 0);
            insertln();

            mvaddstr(view.pos, 0, view.buffer[view.top + view.pos].c_str());

            move(view.rows - 1, 0);
            deleteln();
        }
}

void init() {
    if (NULL == initscr()) {
        fprintf(stderr, "failed to initialize curses");
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(stdscr, TRUE);
}

int main(int argc, char *argv[]) {
    View view;

    init();
    SPDConnection *s = spd_open("myApp", NULL, NULL, SPD_MODE_SINGLE);
    if (s == NULL) {
        fprintf(stderr, "failed to open connection to speeech-dispatcher");
        exit(EXIT_FAILURE);
    }

    view.buffer = create();



    display(view);

    int ch;
    spd_say(s, SPD_TEXT, view.buffer[view.top + view.pos].c_str());
    mvprintw(view.rows - 1, 40, "Ln %d", view.pos + 1);
    move(view.pos, 0);
    while ('q' != (ch = getch())) {
        if (ch == KEY_UP)
            moveUp(view);

        if (ch == KEY_DOWN)
            moveDown(view);

        spd_say(s, SPD_TEXT, view.buffer[view.top + view.pos].c_str());

        mvprintw(view.rows - 1, 40, "Ln %d, key = 0%03o", view.top + view.pos + 1, ch);
        move(view.pos, 0);
        refresh();
    }

    delwin(stdscr);
    endwin();
    refresh();
    spd_close(s);

    return EXIT_SUCCESS;
}
