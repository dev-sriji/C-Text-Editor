#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

#define CTRL(key) (key & 0x1f)

struct termios orig_termios;

void refreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void handle_err(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1){
        handle_err("tcsetattr");
    };
}
void enableRawMode() {
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) handle_err("tsgetattr");
    atexit(disableRawMode);
    struct termios copy_termios = orig_termios;
    copy_termios.c_cflag |= (CS8);
    copy_termios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    copy_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    copy_termios.c_oflag &= ~(OPOST);
    copy_termios.c_cc[VMIN] = 0;
    copy_termios.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &copy_termios) == -1) handle_err("tcsetattr");
}


int readKeyPress() {
    char c;
    int key_read;
    while(read(key_read = STDIN_FILENO, &c, 1) == -1){
        if(key_read == -1 && errno != EAGAIN)
            handle_err("read");
    }
    return c;
}
void processKeyPress() {
    char c = readKeyPress();
    switch (c) {
        case CTRL('q'):
        refreshScreen();
        exit(0);
        break;
    }
}

int main() {
    enableRawMode();
    while(1){
        processKeyPress();
    };
  return 0;
}
