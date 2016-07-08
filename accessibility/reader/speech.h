
#ifndef __SPEECH_H__
#define __SPEECH_H__

#include <string>
#include <memory>

class Speech {
  public:
    Speech(const char *name);
    ~Speech();

    bool connect();
    bool isConnected() const;

    void say(const std::string& text) const;

  private:
    struct data;
    std::unique_ptr<data> d_ptr;
};

#endif
