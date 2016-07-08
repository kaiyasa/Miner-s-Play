
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

#include <unistd.h>

#include <ncurses.h>

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "speech.h"

typedef std::vector<std::string> Lines;

using namespace std;

enum KeyCode {
    CNTL_KEY_RIGHT = 01057
};

struct Description {
    struct Marker {
        int size;
        string utter;
    };

    int location;
    string all;
    vector<Marker> text;
};

struct View {
    View()
      : top(0), pos(0), xpos(0), y(0), rows(24), cols(80) { }

    int top, pos, xpos, y, rows, cols;
    Lines buffer;

    string& currentLine() {
        return buffer[location()];
    }

    char currentChar() {
        return currentLine()[xpos];
    }

    View& print(const string& text) {
        waddstr(stdscr, text.c_str());
        return *this;
    }

    View& print(int r, int c, const string& text) {
        mvwaddstr(stdscr, y + r, c, text.c_str());
        return *this;
    }

    View& printf(const char *fmt, ...) {
        va_list va;

        va_start(va, fmt);
        print(sprintf(va, fmt));
        va_end(va);
        return *this;
    }

    View& printf(int r, int c, const char *fmt, ...) {
        va_list va;

        va_start(va, fmt);
        print(r, c, sprintf(va, fmt));
        va_end(va);
        return *this;
    }

    int location() {
        return top + pos;
    }

    View& moveTo() {
        int last = currentLine().empty() ? 0 : currentLine().size() - 1;
        return moveTo(pos, min(xpos, last));
    }

    View& moveTo(int r, int c) {
        wmove(stdscr, y + r, c);
        return *this;
    }

    View& insln() {
        winsertln(stdscr);
        return *this;
    }

    View& delln() {
        wdeleteln(stdscr);
        return *this;
    }

    string sprintf(va_list va, const char *fmt) {
        char buffer[2048];

        vsnprintf(buffer, sizeof(buffer), fmt, va);
        return string(buffer);
    }
};

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
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

struct Context {
    View view;
    View status;
    Speech speech;
    Description speak;

    Context() : speech("q") {}
};

void init(Context& context) {
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

FILE* read(string fname) {
    FILE *fp;

    if ((fp = fopen(fname.c_str(), "r")) == NULL) {
        perror(fname.c_str());
        exit(EXIT_FAILURE);
    }

    return fp;
}

string char2str(char ch);

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

struct KeyAction {
    typedef function<int(Context&)> lambda;

    int keyCode;
    lambda action;
};

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

vector<KeyAction> setup() {
    vector<KeyAction> result;

    result.push_back( {KEY_UP, [](Context &context) {
        moveUp(context.view);
        return 1;
    }});

    result.push_back( {KEY_DOWN, [](Context &context) {
        moveDown(context.view);
        return 1;
    }});

    result.push_back( {KEY_RESIZE, [](Context &context) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        context.view.rows = rows - 1;
        context.status.y = context.view.rows;
        context.view.cols = context.status.cols = cols;

        if (context.view.pos >= context.view.rows)
            context.view.pos = context.view.rows - 1;
        display(context.view);
        return 1;
    }});

    result.push_back( {KEY_RIGHT, [](Context &context) {
        string text;

        if (context.view.xpos >= context.view.currentLine().size() - 1)
            text = string("end");
        else {
            Description::Marker &mark = context.speak.text[context.view.xpos];
            context.view.xpos += mark.size;
            text = mark.utter;
        }

        context.speech.say(text);
        return 0;
    }});

    result.push_back( {KEY_LEFT, [](Context &context) {
        string text;

        if (context.view.xpos == 0)
            text = string("start");
        else {
            int prev = max(0, min(context.view.xpos - 1, context.view.currentLine().size() - 2));
            context.view.xpos = prev;
            text = char2str(context.view.currentLine()[prev]);
        }

        context.speech.say(text);
        return 0;
    }});

    result.push_back( {CNTL_KEY_RIGHT, [](Context &context) {
        context.speech.say("stub");
        return 0;
    }});

    result.push_back( {' ', [](Context &context) {
        if (context.view.location() < context.view.buffer.size() - context.view.rows) {
            context.view.top += context.view.rows - 1;
            if (context.view.location() >= context.view.buffer.size())
                context.view.pos = context.view.buffer.size() - 1 - context.view.top;
            display(context.view);
        } else {
            context.speech.say("no more text");
            return 0;
        }
        return 1;
    }});

    result.push_back( {'q', [](Context &context) {
        return 2;
    }});

    return result;
}

int main(int argc, char *argv[]) {
    Context context;

    if (argc < 2) {
        context.view.buffer = text(stdin);
        freopen("/dev/tty", "r", stdin);
    } else
        context.view.buffer = text(read(argv[1]));


    if (!context.speech.connect()) {
        fprintf(stderr, "failed to open connection to speeech-dispatcher");
        exit(EXIT_FAILURE);
    }

    init(context);
    wtimeout(stdscr, 100);
    display(context.view);

    context.speak = describe(context.view.location(), context.view.currentLine());
    context.speech.say(context.speak.all);

    context.status.moveTo(0, 0)
          .delln().insln()
          .printf(0, 40, "Ln %d", context.view.location() + 1);
    context.view.moveTo();

    int ch;
    auto actions = setup();
    bool isRunning = true;
    while (isRunning) {
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

        context.status.moveTo(0, 0)
              .delln().insln()
              .printf(0, 40, "Ln %d (%d:%d), key = 0%03o (%s)",
                 context.view.location() + 1, context.view.top, context.view.pos, ch, keyname(ch));

        context.view.moveTo();
        refresh();
    }

    delwin(stdscr);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}

