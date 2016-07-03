
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

#include <unistd.h>

#include <ncurses.h>

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include <speech-dispatcher/libspeechd.h>

typedef std::vector<std::string> Lines;

using namespace std;

class Speech {
  public:
    bool connect() {
        if (!isConnected())
            channel = spd_open(appName, NULL, NULL, SPD_MODE_SINGLE);

        return isConnected();
    }

    bool isConnected() const {
        return channel != NULL;
    }

    void say(const string& text) const {
        if (!isConnected())
            return;

        const char *data = text.empty() ? "blank" : text.c_str();
        spd_say(channel, SPD_TEXT, data);
    }

    Speech(const char *name)
      : appName(name) { }

    ~Speech() {
        spd_close(channel);
    }

  private:
    const char *appName;
    SPDConnection *channel = NULL;
};

struct View {
    View()
      : top(0), pos(0), y(0), rows(24), cols(80) { }

    int top, pos, y, rows, cols;
    Lines buffer;

    string& currentLine() {
        return buffer[location()];
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
        return moveTo(pos, 0);
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
        view.print(row - view.top, 0, view.buffer[row]);
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
            .moveTo()
            .insln()
            .print(view.currentLine());
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
            .moveTo()
            .insln()
            .print(view.currentLine());
    }
}

void init(View& view, View& status) {
    if (NULL == initscr()) {
        fprintf(stderr, "failed to initialize curses");
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    view.y    = 0;
    view.rows = rows - 1;
    view.cols = cols;

    status.y    = view.rows;
    status.rows = 1;
    status.cols = cols;

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

struct KeyAction {
    typedef function<int(View&, Speech&)> lambda;

    int keyCode;
    lambda action;
};

vector<KeyAction> setup() {
    vector<KeyAction> result;

    result.push_back( {KEY_UP, [](View &view, Speech &speech) {
        moveUp(view);
        return 1;
    }});

    result.push_back( {KEY_DOWN, [](View &view, Speech &speech) {
        moveDown(view);
        return 1;
    }});

    result.push_back( {KEY_RESIZE, [](View &view, Speech &speech) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        view.rows = rows - 1;
        //status.y = view.rows;
        view.cols = /*status.cols =*/ cols;

        if (view.pos >= view.rows)
            view.pos = view.rows - 1;
        display(view);
        return 1;
    }});

    result.push_back( {' ', [](View &view, Speech &speech) {
        if (view.location() < view.buffer.size() - view.rows) {
            view.top += view.rows - 1;
            if (view.location() >= view.buffer.size())
                view.pos = view.buffer.size() - 1 - view.top;
            display(view);
        } else {
            speech.say("no more text");
            return 0;
        }
        return 1;
    }});

    result.push_back( {'q', [](View &view, Speech &speech) {
        return 2;
    }});
    return result;
}

int main(int argc, char *argv[]) {
    View view, status;
    Speech speech("q");

    if (argc < 2) {
        view.buffer = text(stdin);
        freopen("/dev/tty", "r", stdin);
    } else
        view.buffer = text(read(argv[1]));


    if (!speech.connect()) {
        fprintf(stderr, "failed to open connection to speeech-dispatcher");
        exit(EXIT_FAILURE);
    }

    init(view, status);
    display(view);

    speech.say(view.currentLine());

    status.moveTo(0, 0)
          .delln().insln()
          .printf(0, 40, "Ln %d", view.location() + 1);
    view.moveTo();

    int ch;
    auto actions = setup();
    bool isRunning = true;
    while (isRunning) {
        ch = wgetch(stdscr);
        auto item = find_if(actions.begin(), actions.end(), [ch](KeyAction action) {
            return action.keyCode == ch;
        });

        if (item != actions.end()) {
            int code = item->action(view, speech);
            if (code == 2)
                isRunning = false;
            if (code == 1)
                speech.say(view.currentLine());
        }

        status.moveTo(0, 0)
              .delln().insln()
              .printf(0, 40, "Ln %d (%d:%d), key = 0%03o (%s)",
                 view.location() + 1, view.top, view.pos, ch, keyname(ch));

        view.moveTo();
        refresh();
    }

    delwin(stdscr);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}

