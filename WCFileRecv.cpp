#include "WCFileRecv.h"
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

WCFileRecv::WCFileRecv(string path, uint32_t peerip)
{
    _path = path;
    _peerip = peerip;
    pthread_t tid;
    pthread_create(&tid, NULL, recvHandle, (void*)this);
}

void* WCFileRecv::recvHandle(void* arg)
{
    pthread_detach(pthread_self());

    WCFileRecv* obj = (WCFileRecv*)arg;
    obj->start();
    delete obj;
}

void WCFileRecv::start()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.s_addr = _peerip;
    int err = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(err < 0){
        perror("connect");
        return;
    }

    err = write(fd, _path.c_str(), strlen(_path.c_str()));

    printf("ack success -- %s\n", _path.c_str());

    err = write(fd, "\n", 1);


    printf("ret : %d\n", err);

    recvFile(fd);
}

char* WCFileRecv::getLine(FILE* fp)
{
   char* err = fgets(_buf, sizeof(_buf), fp);
   if(err == NULL){
       perror("fgets");
   }
   _buf[strlen(_buf) - 1] = 0;
   return _buf;
}

void WCFileRecv::recvFile(uint32_t fd)
{
    printf("start recv file ...\n");
    FILE* fp = fdopen(fd, "r");
    if(fp == NULL){
        perror("fdopen");
    }
    char buf[MAX_MSG_LEN] = {};
    char* p = NULL;
    while(1){
        p = getLine(fp);
        if(strcmp(p, WC_FILEEOF) == 0)
            break;

        if(strcmp(p, WC_SENTINEL) != 0)       // 1 sentinel
            return;

        p = getLine(fp);
        if(strcmp(p, WC_FILE_DIR) == 0){
            p = getLine(fp);
            mkdir(p, 0755);
        }else if(strcmp(p, WC_FILE_REG) == 0){    // 2 file type
            p = getLine(fp);                     // 3 file name
            FILE* fptr = fopen(p, "w");

            uint64_t filelen = atoll(getLine(fp));  // 4 file length
            while(filelen > 0){
                int readlen = filelen > sizeof(buf) ? sizeof(buf) : filelen;
                int ret = fread(buf, 1, readlen, fp);
                if(ret > 0){
                    printf("recv file content : %s\n", buf);
                }
                fwrite(buf, ret, 1, fptr);
                filelen -= ret;
            }
            fclose(fptr);
            break;
        }else{
            printf("Unknown file type\n");
            return;
        }
    }
    fclose(fp);
}

















