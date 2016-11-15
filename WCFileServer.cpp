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
    pid_t pid = fork();
    if(pid == 0){
        startServer();
    }
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

    FD_ZERO(&_clifds);
    FD_SET(_listenfd, &_clifds);
    _maxFds = _listenfd;
    while(1){
        fd_set fds;
        struct timeval tv = { 2, 0};
       // memcpy(&fds, &_clifds, sizeof(fds));
        FD_SET(_listenfd, &fds);

        switch(select(_maxFds + 1, &fds, NULL, NULL, &tv)){
        case -1 :{
                if(errno == EAGAIN)
                    continue;
                exit(-1);
            }
            break;
        case 0 :{
//                printf("time out\n");
                continue;
            }
            break;
        default:{
                if(FD_ISSET(_listenfd, &fds))
                    add2select(fds);
                recvData(fds);
            }
        }

#if 0
        int count = select(_maxFds + 1, &fds, NULL, NULL, &tv);
        if(count > 0){
            if(FD_ISSET(_listenfd, &fds)){
                add2select(fds);
            }

            recvData(fds);

        }else{
            if(count == 0){
                continue;
            }else{
                perror("select");
                exit(-1);
            }
        }
#endif
    }

}

void WCFileServer::recvData(fd_set& fds)
{
    char buf[MAX_MSG_LEN] = {};
    //char *ret = NULL;
    for(int fd = 3; fd < MAX_FDS + 1; ++fd){
        if(FD_ISSET(_listenfd, &fds)){
            continue;
        }
        if(FD_ISSET(fd, &fds)){
            FILE* fp = fdopen(fd, "r");
            fgets(buf, sizeof(buf), fp);
            buf[strlen(buf) - 1] = 0;

            printf("recv len : %d\n", strlen(buf));
			printf("recv data path : %s\n", buf);

            //fclose(fp);   fp closed and the fd will be closed;
            FD_CLR(fd, &_clifds);

            sendFile(fd, buf);
        }
        //close(fd);
    }
    ////////////////////////////

}

void WCFileServer::sendFile(int fd, char* path)
{
    //FILE* fp = fdopen(fd, "w");

    // Change path ?
	printf("send file..\n");
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

    //fprintf(fp, "%s\n", WC_FILEEOF);
    //fflush(fp);

    close(fd);
}

void WCFileServer::sendFileReg(int fd, char *path)
{
    printf("file descriptor : %d\n", fd);
    FILE* fp = fdopen(fd, "w");
    if(fp == NULL){
        perror("fdopen");
    }

    int ret = fprintf(fp, "%s\n", WC_SENTINEL);
    printf(" fprintf ret : %d\n", ret);



    fprintf(fp, "%s\n", WC_FILE_REG);
    fprintf(fp, "%s\n", path);
    fprintf(fp, "%llu\n", (long long unsigned)WCUtil::getFileSize(path));

     fflush(fp);

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

void WCFileServer::add2select(fd_set& fds)
{
ACCEPT:
     printf("waiting to connect ...\n"); ///
    int connfd = accept(_listenfd, NULL, NULL);
    printf("connfd : %d\n", connfd);
    if(connfd > 0){
        printf("accept data ...\n"); ///
        /// \brief FD_SET
        FD_SET(connfd, &fds);
        FD_SET(connfd, &_clifds);
        _maxFds = _maxFds > connfd ? _maxFds : connfd;
//        _clifds.push_back(connfd);
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
    err = listen(fd, 60);

    //printf("start listening \n");

    return fd;
}
