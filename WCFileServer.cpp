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

}

WCFileServer::~WCFileServer()
{
    printf("file server desctructor\n");
}

WCFileServer* WCFileServer::instance()
{
    static WCFileServer* obj = NULL;
    if(obj == NULL){
        obj = new WCFileServer;
    }
    return obj;
}

void WCFileServer::runFileServer()
{
    pid_t pid = fork();
    if(pid == 0){
        startServer();
    }
}

void WCFileServer::startServer()
{   
    _listenfd = createSocket();

    _maxFds = _listenfd;
    while(1){
        fd_set fds;        
        FD_SET(_listenfd, &fds);
        struct timeval tv = { 2, 0};

        switch(select(_maxFds + 1, &fds, NULL, NULL, &tv)){
        case -1 :{
                if(errno == EAGAIN)
                    continue;
                perror("select");
                exit(-1);
            }
            break;
        case 0 :{
            // Time out
                continue;
            }
            break;
        default:{
                if(FD_ISSET(_listenfd, &fds))
                    add2select(fds);
                recvData(fds);
            }
        }
    }
}

void WCFileServer::recvData(fd_set& fds)
{
    char buf[MAX_MSG_LEN] = {};

    for(int fd = 3; fd < MAX_FDS + 1; ++fd){
        if(FD_ISSET(_listenfd, &fds)){
            continue;
        }
        if(FD_ISSET(fd, &fds)){
            FILE* fp = fdopen(fd, "r");
            fgets(buf, sizeof(buf), fp);
            buf[strlen(buf) - 1] = 0;

            sendFile(fd, buf);

            FILE* fptr = fdopen(fd, "w");
            if(fptr == NULL){
                perror("fdopen");
            }
            fprintf(fptr, "%s\n", WC_FILEEOF);

            fflush(fptr);

            recv(fd, buf, 1, 0);  // wait client to close the socket
            close(fd);
            FD_CLR(fd, &fds);

            printf(">>>");
            fflush(stdout);
        }
    }
}

void WCFileServer::sendFile(int fd, char* path)
{
    memset(_realPath, 0, sizeof(_realPath));
    memset(_relativePath, 0, sizeof(_relativePath));
    realpath(path, _realPath);  /* Get absolute path */

    char* pos = rindex(_realPath, '/');
    memcpy(_relativePath, pos + 1, strlen(pos + 1));

    struct stat st;
    lstat(path, &st);
    if(S_ISREG(st.st_mode)){
        sendFileReg(fd, path);
    }else if(S_ISDIR(st.st_mode)){
        sendFileDir(fd, path);
    }else if(S_ISLNK(st.st_mode)){
        // Do something;
    }else{
        printf("Unknown file type");
    }
}

void WCFileServer::sendFileReg(int fd, char *path)
{
    FILE* fp = fdopen(fd, "w");
    if(fp == NULL){
        perror("fdopen");
    }

    fprintf(fp, "%s\n", WC_SENTINEL);
    fprintf(fp, "%s\n", WC_FILE_REG);
    fprintf(fp, "%s\n", _relativePath);
    fprintf(fp, "%llu\n", (long long unsigned)WCUtil::getFileSize(_realPath));
    fflush(fp);

    char buf[MAX_MSG_LEN];
    FILE* fptr = fopen(_realPath, "r");
    if(fptr == NULL){
        perror("fopen");
    }
    while(1){
        memset(buf, 0, sizeof(buf));
        int ret = fread(buf, 1, sizeof(buf), fptr);

        if(ret <= 0){
            break;
        }
        ret = fwrite(buf, ret, 1, fp);
        if(ret != 1){
            exit(1);
        }
        fflush(fp);
    }
    fclose(fptr);
}

void WCFileServer::sendFileDir(int fd, char *path)
{
    FILE* fp = fdopen(fd, "w");
    fprintf(fp, "%s\n", WC_SENTINEL);
    fprintf(fp, "%s\n", WC_FILE_DIR);
    fprintf(fp, "%s\n", _relativePath);
    fflush(fp);

    DIR* dir = opendir(_realPath);
    struct dirent* entry;
    while(entry = readdir(dir)){
        if(string(entry->d_name) == "." ||
                string(entry->d_name) == "..")
            continue;

        sprintf(_relativePath, "%s/%s", _relativePath, entry->d_name);
        sprintf(_realPath, "%s/%s", _realPath, entry->d_name);
        if(entry->d_type == DT_DIR){
            sendFileDir(fd, _realPath);
        }else if(entry->d_type == DT_REG){
            sendFileReg(fd, _realPath);
        }
    }
    closedir(dir);
}

void WCFileServer::add2select(fd_set& fds)
{
ACCEPT:
    int connfd = accept(_listenfd, NULL, NULL);
    if(connfd > 0){
        FD_SET(connfd, &fds);
        _maxFds = _maxFds > connfd ? _maxFds : connfd;
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
    //inet_pton(AF_INET, "192.168.11.85", &addr.sin_addr);
    int err = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(err < 0){
        perror("bind");
        exit(-1);
    }
    int flag = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&flag, sizeof(flag));

    err = listen(fd, 60);

    return fd;
}
