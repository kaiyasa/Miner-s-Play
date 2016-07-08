
#include "application.h"


#include <cstdio>
#include <algorithm>

#include <ncurses.h>


using namespace std;

Application::Application(Context& context, const vector<KeyAction>& act)
  : actions(act)
{
    if (!context.speech.connect()) {
        fprintf(stderr, "failed to open connection to speeech-dispatcher");
        exit(EXIT_FAILURE);
    }

    if (NULL == initscr()) {
        fprintf(stderr, "failed to initialize curses");
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    context.view.y    = 0;
    context.view.rows = rows - 1;
    context.view.cols = cols;

    context.status.y    = context.view.rows;
    context.status.rows = 1;
    context.status.cols = cols;

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
        color_set(1, NULL);
    }

}

Application::~Application()
{
    delwin(stdscr);
    endwin();
    refresh();
}

void Application::run(Context& context, vector<string>& lines)
{
    context.view.buffer = lines;
    display(context.view);

    context.speak = describe(context.view.location(), context.view.currentLine());
    context.speech.say(context.speak.all);

    int ch = -1;
    bool isRunning = true;
    while (isRunning) {
        context.status.moveTo(0, 0)
              .delln().insln()
              .printf(0, 40, "Ln %d (%d:%d), key = 0%03o (%s)",
                 context.view.location() + 1, context.view.top, context.view.pos, ch, keyname(ch));

        context.view.moveTo();

        wrefresh(stdscr);
        ch = wgetch(stdscr);

        auto item = find_if(actions.begin(), actions.end(), [ch](KeyAction action) {
            return action.keyCode == ch;
        });

        if (item != actions.end()) {
            int code = item->action(context);
            if (code == 2)
                isRunning = false;
            if (code == 1) {
                if (context.speak.location != context.view.location())
                    context.speak = describe(context.view.location(), context.view.currentLine());
                context.speech.say(context.speak.all);
            }
        }
    }
}


string char2str(char ch) {
    char text[2] = { 0, 0 };

    text[0] = ch;
    switch (ch) {
        case '(':   return string("open paren");
        case '{':   return string("open curly");
        case ')':   return string("close paren");
        case '}':   return string("close curly");
        case '<':   return string("greater than");
        case '>':   return string("less than");
        case '.':   return string("dot");
        case ';':   return string("semi colon");
        case ' ':   return string("space");
        case '\t':  return string("tab");
        default:    return string(text);
    }
    return string("undefined");
}

Description describe(int location, const string& text) {
    Description result = { location, text };

    int pos = 0;
    for(auto i = text.begin(); i != text.end(); ++i) {
        char ch = *i;
        int size = 1;

        if (ch == '\t')
            size = TABSIZE - (pos % TABSIZE);
        Description::Marker mark = { size, char2str(ch) };
        result.text.push_back(mark);
    }
    return result;
}
