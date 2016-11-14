#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h>
#include <sys/select.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include "WCFileServer.h"
#include "WCUtil.h"
#include "WC.h"

WCFileServer::WCFileServer()
{
    _clinum = 0;
    FD_ZERO(&_clifds);
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
    _listenfd = createSocket();
    fd_set fds;


    struct timeval tv = {2, 0};
    while(1){
        FD_ZERO(&fds);
        if(_clinum > 0){
            memcpy(&fds, &_clifds, sizeof(_clifds));
        }

        FD_SET(_listenfd, &fds);
        int count = select(MAX_FDS + 1, &fds, NULL, NULL, &tv);
        if(count > 0){
            if(FD_ISSET(_listenfd, &fds)){
                add2select(fds, _listenfd);
#if 0
ACCEPT:
                connfd = accept(listenfd, NULL, NULL);
                if(connfd > 0){
                    FD_SET(connfd, &fds);
                    FD_SET(connfd, &_clifds);
                    ++_clinum;
                }else{
                    if(errno == EAGAIN){
                        goto ACCEPT;
                    }else{
                    perror("accept");
                    exit(-1);
                    }
                }
#endif
            }else{
                // read from socket
                recvData(fds);
            }
        }else{
            if(count == 0){
                continue;
            }else{
                perror("select");
                exit(-1);
            }
        }
    }

}

void WCFileServer::recvData(fd_set& fds)
{
    char buf[MAX_MSG_LEN] = {};
    int ret = 0;
    for(int fd = 3; fd < MAX_FDS + 1; ++fd){
        if(FD_ISSET(fd, &fds)){
            FILE* fp = fdopen(fd, "r");
            fgets(buf, sizeof(buf), fp);
            buf[strlen(buf) - 1] = 0;
            fclose(fp);

            sendFile(fd, buf);
        }
        close(fd);
    }
    ////////////////////////////

}

void WCFileServer::sendFile(int fd, char* path)
{
    // Change path ?
    struct stat st;
    lstat(path, &st);
    if(S_ISREG(st.st_mode)){
        sendFileReg(fd, path);
    }else if(S_ISDIR(st.st_mode)){
        sendFileDir(fd, path);
    }else if(S_ISLNK(st.st_mode)){
        //
    }else{
        printf("Unknown file type");
    }

}

void WCFileServer::sendFileReg(int fd, char *path)
{
    FILE* fp = fdopen(fd, "w");
    fprintf(fp, "%s\n", WC_SENTINEL);
    fprintf(fp, "%s\n", WC_FILE_REG);
    fprintf(fp, "%s\n", path);
    fprintf(fp, "%llu\n", (long long unsigned)WCUtil::getFileSize(path));

    char* buf[MAX_MSG_LEN];
    FILE* f = fopen(path, "r");
    while(1){
        int ret = fread(buf, 1, sizeof(buf), f);
        if(ret <= 0){
            break;
        }
        ret = fwrite(buf, ret, 1, fp);
        if(ret != 1){
            exit(1);
        }
    }
    fclose(f);
}

void WCFileServer::sendFileDir(int fd, char *path)
{
    FILE* fp = fdopen(fd, "w");
    fprintf(fp, "%s\n", WC_SENTINEL);
    fprintf(fp, "%s\n", WC_FILE_DIR);
    fprintf(fp, "%s\n", path);

    DIR* dir = opendir(path);
    struct dirent* entry;
    char newpath[128] = {};
    while(entry = readdir(dir)){
        if(string(entry->d_name) == "." ||
                string(entry->d_name) == "..")
            continue;

        sprintf(newpath, "%s/%s", path,entry->d_name);
        if(entry->d_type == DT_DIR){
            sendFileDir(fd, newpath);
        }else if(entry->d_type == DT_REG){
            sendFileReg(fd, newpath);
        }
    }
    closedir(dir);
}

void WCFileServer::add2select(fd_set& fds, int listenfd)
{
ACCEPT:
    int connfd = accept(listenfd, NULL, NULL);
    if(connfd > 0){
        FD_SET(connfd, &fds);
        FD_SET(connfd, &_clifds);
        ++_clinum;
    }else{
        if(errno == EAGAIN){
            goto ACCEPT;
        }else{
            perror("accept");
            exit(-1);
        }
    }
}

int WCFileServer::createSocket()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    int err = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(err < 0){
        perror("bind");
        exit(-1);
    }
    err = listen(fd, 60);

    return fd;
}
