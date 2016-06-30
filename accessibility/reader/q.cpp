
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include <ncurses.h>

#include <string>
#include <vector>

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

    bool isConnected() {
        return channel != NULL;
    }

    void say(const string& text) {
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
      : top(0), pos(0), rows(24) { }

    int top, pos, rows, cols;
    Lines buffer;
};

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

void display(const View &view) {
    clear();
    move(0, 0);
    for(auto row = view.top; row < view.top + view.rows - 1; ++row)
        mvwaddstr(stdscr, row, 0, view.buffer[row].c_str());
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

        mvwaddstr(stdscr, view.pos, 0, view.buffer[view.top + view.pos].c_str());
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

            mvwaddstr(stdscr, view.pos, 0, view.buffer[view.top + view.pos].c_str());

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

Lines read(string fname) {
    FILE *fp;

    if ((fp = fopen(fname.c_str(), "r")) == NULL) {
        perror(fname.c_str());
        exit(EXIT_FAILURE);
    }

    return text(fp);
}

int main(int argc, char *argv[]) {
    View view;
    Speech speech("q");

    view.buffer = read(argv[1]);
    if (!speech.connect()) {
        fprintf(stderr, "failed to open connection to speeech-dispatcher");
        exit(EXIT_FAILURE);
    }

    init();
    getmaxyx(stdscr, view.rows, view.cols);
    display(view);

    int ch;
    speech.say(view.buffer[view.top + view.pos]);

    mvwprintw(stdscr, view.rows - 1, 40, "Ln %d", view.pos + 1);
    move(view.pos, 0);
    while ('q' != (ch = getch())) {
        if (ch == KEY_UP)
            moveUp(view);

        if (ch == KEY_DOWN)
            moveDown(view);

        if (ch == KEY_RESIZE) {
            getmaxyx(stdscr, view.rows, view.cols);
            if (view.pos > view.rows - 2)
                view.pos = view.rows - 2;
            display(view);
        }
        speech.say(view.buffer[view.top + view.pos]);

        mvwprintw(stdscr, view.rows - 1, 40, "Ln %d, key = 0%03o", view.top + view.pos + 1, ch);
        move(view.pos, 0);
        refresh();
    }

    delwin(stdscr);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}

