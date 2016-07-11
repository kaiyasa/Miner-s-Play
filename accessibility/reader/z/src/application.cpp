
#include "application"

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <thread>


using namespace std;


Application::Application()
{
}

Application::~Application()
{
}

class Keyboard 
{
  public:
    Keyboard(Tui *in)
      : waiting(false), sio(in)
    { }

    int fetch() {
        if (!waiting)
            start();

        if (keyCode == -2)
            return -1;

        return finish();
    }

  private:
    void start()
    {
        keyCode = -2;
        waiting = true;
        worker = thread(reader, sio);
    }

    int finish()
    {
        worker.join();
        int result = keyCode;
        waiting = false;
        return result;
    }

    static void reader(Tui* sio)
    {
        keyCode = sio->getKeycode();
    }

    Tui *sio;
    bool waiting = false;
    thread worker;

    static volatile int keyCode;
};
volatile int Keyboard::keyCode = -2;

bool preload(FILE *inFile) { return false; }

void display() {}


class Loader {
  public:
    bool load() {
        char *b, *p, end = rbuf + sizeof(rbuf);

        for(b = rbuf; b != end; b = p + 1) {
            for(p = b; p != end; ++p) {
                if (*p != '\n' && *p != '\r') 
                    ignore = false;
                    continue;
                }

                if (ignore && *p == '\n')
                    ignore = false;
                else {
                  save(b);
                  ignore = (*p == '\r');
                }
                break;
            }
        }
    }

    char rbuf[8192];
    bool ignore = false;
}

        int nr = fread(rbuf, 1, sizeof(rbuf), input));
        if (nr == 0)
            return true;

        while (b != end) {
            for(p = b; p != end; ++p) {
                if (*p == '\n' && !ignore) {
                    result.push_back(b);
                    break;
                } else if (*p == '\r') {
                    ignore = true;
                    result.push_back(b);
                    break;
                } else {
                    ignore = false;
                }
            }
            b = ++p;
        }
    }

    FILE *input;
    char rbuf[8192];
    string current;
}

void Application::run(FILE *inFile)
{
    Keyboard keyboard(&sio);
    Point cursor(2, 0), col(0, 1);
    char rbuf[8192];
    size_t nr, count = 0;

    bool isRunning, finished = preload(inFile);
    display();

    while (isRunning)
    {
        if (!finished)
            finished = loadMore();

        int ch = keyboard.fetch(!finished);
        auto action = findAction(ch);
        isRunning = action(context);
    }

    while ((nr = fread(rbuf, 1, sizeof(rbuf), inFile)) > 0) {
        count += nr;
        int ch = keyboard.fetch();
        if (ch > -1) {
        }
    }
}
