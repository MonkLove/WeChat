#include <unistd.h>
#include "WCFileServer.h"

WCFileServer::WCFileServer()
{

}

WCFileServer* WCFileServer::instance()
{
    static WCFileServer* obj = NULL;
    if(obj == NULL){
        obj = new WCFileServer;
    }
    return obj;
}

void WCFileServer::startServer()
{
   // int fd = socket(AF_INET, )
}
