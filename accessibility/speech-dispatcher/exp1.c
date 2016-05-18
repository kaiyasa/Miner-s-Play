
#include <stdio.h>
#include <stdlib.h>

#include <speech-dispatcher/libspeechd.h>

int main(int argc, char **argv) {
    SPDConnection *s = spd_open("myApp", NULL, NULL, SPD_MODE_SINGLE);
    if (s == NULL) {
        fprintf(stderr, "failed to open connection to speeech-dispatcher");
        exit(-1);
    }

    spd_say(s, SPD_TEXT, "hello");

    spd_close(s);

    return 0;
}
