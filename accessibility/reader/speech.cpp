
#include "speech.h"

#include <speech-dispatcher/libspeechd.h>

using namespace std;

 struct Speech::data {
    const char *appName;
    SPDConnection *channel;
};


Speech::Speech(const char *name)
  : d_ptr(new data({name, NULL}))
{
}

Speech::~Speech()
{
    spd_close(d_ptr->channel);
}

bool Speech::connect() {
    if (!isConnected())
        d_ptr->channel = spd_open(d_ptr->appName, NULL, NULL, SPD_MODE_SINGLE);

    return isConnected();
}

bool Speech::isConnected() const {
    return d_ptr->channel != NULL;
}

void Speech::say(const string& text) const {
    if (!isConnected())
        return;

    const char *data = text.empty() ? "blank" : text.c_str();
    spd_say(d_ptr->channel, SPD_TEXT, data);
}
