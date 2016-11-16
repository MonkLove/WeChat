#ifndef WCFILESERVER_H
#define WCFILESERVER_H
#include <sys/select.h>
#include <inttypes.h>
#include <list>
using namespace std;

class WCFileServer
{
private:
    int _listenfd;
    uint32_t _maxFds;

    char _realPath[128];
    char _relativePath[128];

    WCFileServer();

    int createSocket();
    void add2select(fd_set& fds);
    void recvData(fd_set& fds);
    void sendFile(int fd, char* path);
    void sendFileReg(int fd, char* path);
    void sendFileDir(int fd, char* path);
public:
    ~WCFileServer();
    void runFileServer();
    static WCFileServer* instance();
    void startServer();
};

#endif // WCFILESERVER_H
