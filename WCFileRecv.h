#ifndef WCFILERECV_H
#define WCFILERECV_H

#include <string>
#include <inttypes.h>
#include "WC.h"
using namespace std;

class WCFileRecv
{
private:
    string _path;
    uint32_t _peerip;
    char _buf[MAX_MSG_LEN];

    static void* recvHandle(void* arg);
    void start();
    void recvFile(uint32_t);
    char* getLine(FILE*);
public:
    WCFileRecv(string path, uint32_t peerip);
};

#endif // WCFILERECV_H
