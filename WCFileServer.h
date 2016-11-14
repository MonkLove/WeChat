#ifndef WCFILESERVER_H
#define WCFILESERVER_H
#include <sys/select.h>
#include <inttypes.h>

class WCFileServer
{
private:
    int _listenfd;
    fd_set _clifds;
    uint16_t _clinum;

    WCFileServer();

    int createSocket();
    void add2select(fd_set& fds, int fd);
    void recvData(fd_set& fds);
    void sendFile(int fd, char* path);
    void sendFileReg(int fd, char* path);
    void sendFileDir(int fd, char* path);
public:
    WCFileServer* instance();
    void startServer();
};

#endif // WCFILESERVER_H
