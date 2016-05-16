#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

struct termios oldt, newt;
void init_scr()
{
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VTIME] = 0;
    newt.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ioctl(STDIN_FILENO, KDSKBMODE, K_MEDIUMRAW);
    printf("\033c");
}

/*--- Reverse the screen to earlier settings*/
void finish_scr()
{
    printf("\n");
    ioctl(STDIN_FILENO, KDSKBMODE, K_RAW);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main(int argc, char *argv[])
{
    init_scr();
    int key;
    do {
        key = getchar(); /* consume the character */
        printf("%d\n", key);
    } while(key != 113);
    finish_scr();
}
