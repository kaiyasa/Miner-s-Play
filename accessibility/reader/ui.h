
#ifndef __UI_H__
#define __UI_H__

#include <functional>

#include "view.h"

enum KeyCode {
    CNTL_KEY_RIGHT = 01057
};


// forward decl
struct Context;

struct KeyAction {
    typedef std::function<int(Context&)> lambda;

    int keyCode;
    lambda action;
};


void display(View &view);

void moveDown(View &view);
void moveUp(View &view);

#endif
