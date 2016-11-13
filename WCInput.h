#ifndef WCINPUT_H
#define WCINPUT_H

#include <vector>
#include <string>
#include "WC.h"
using namespace std;

class WCInput
{
private:
    char _buf[MAX_MSG_LEN]; /* To store user input data */
    vector<string> _cmds;   /* To store user's commands */

    /* To split user's commands */
    void cmdSplit();

    /* To dispatch user's commands */
    void cmdDispatch();

    /* To handle user's commands */
    void cmdsend();
    void cmdlist();

public:
    WCInput();

    void loop();
};

#endif // WCINPUT_H
