
#include <ncurses.h>

#include "application.h"
#include "io.h"
#include "misc.h"


using namespace std;

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
    Application app(context, setup());
    auto actions = setup();

    vector<string> lines;
    if (argc < 2) {
        lines = text(stdin);
        freopen("/dev/tty", "r", stdin);
    } else
        lines = text(read(argv[1]));

    app.run(context, lines);
    return EXIT_SUCCESS;
}
