#ifndef WCFILESERVER_H
#define WCFILESERVER_H


class WCFileServer
{
private:
    WCFileServer();

public:
    WCFileServer* instance();
    void startServer();
};

#endif // WCFILESERVER_H
