
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "speech.h"
#include "ui.h"
#include "description.h"

struct Context {
    View view;
    View status;
    Speech speech;
    Description speak;

    Context() : speech("q") {}
};


class Application {
  public:
    Application(Context& context, const std::vector<KeyAction>& act);
    ~Application();

    void run(Context& context, std::vector<std::string>& lines);

  private:
    std::vector<KeyAction> actions;
};

std::string char2str(char ch);
Description describe(int location, const std::string& text);


#endif
