#include <pthread.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "WCFileRecv.h"
#include "WCNetwork.h"
#include "WCCore.h"
#include "WCUtil.h"
#include "WC.h"

WCNetwork::WCNetwork()
{
    _udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(_udpfd < 0){
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(_udpfd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind");
        exit(0);
    }

    int opt = 1;
    setsockopt(_udpfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

    if(pthread_create(&_tid, NULL, msg_dispatch, NULL) != 0){
        perror("pthread_create");
        exit(0);
    }
}

/* Implementation of Singleton */
WCNetwork* WCNetwork::instance()
{
    static WCNetwork* obj = NULL;
    if(obj == NULL){
        obj = new WCNetwork;
    }
    return obj;
}

void* WCNetwork::msg_dispatch(void* arg)
{
    pthread_detach(pthread_self());

    WCNetwork* THIS = WCNetwork::instance();
    THIS->_msg_dispatch(arg);
}

void* WCNetwork::_msg_dispatch(void* arg)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    while(1){
        memset(&addr, 0, sizeof(addr));
        memset(&_buf, 0, sizeof(_buf));
        recvfrom(_udpfd, _buf, MAX_MSG_LEN,  0, (struct sockaddr*)&addr, &len);

        list<uint32_t>& ips = WCCore::instance()->_ips;
        list<uint32_t>::iterator it = find(ips.begin(), ips.end(), addr.sin_addr.s_addr);
        if(it != ips.end()){
            continue;
        }

        WCJson json;
        if(!json.parse(_buf)){/* Judge if the json is valid */
             continue;
        }

        string cmd = json.get(WC_CMD);
        if(cmd == WC_ONLINE){
            handleOnline(json, addr.sin_addr.s_addr);
        }else if(cmd == WC_ONLINEACK){
            handleOnlineAck(json, addr.sin_addr.s_addr);
        }else if(cmd == WC_SEND){
            handleSend(json);
        }else if(cmd == WC_SENDF){
            handleSendf(json, addr.sin_addr.s_addr);
        }
    }
}

void WCNetwork::handleSendf(WCJson& json, uint32_t peerip)
{
    string name = json.get(WC_NAME);
    string file = json.get(WC_FILE);

    printf("<   From : %s\n", name.c_str());
    printf("<      File : %s\n", file.c_str());
    printf(">>>");
    fflush(stdout);

    //recv
    new WCFileRecv(file, peerip);
}

void WCNetwork::handleSend(WCJson& json)
{
    string name = json.get(WC_NAME);
    string msg = json.get(WC_MSG);

    DEBUG("entry handle send");

    printf("<   From %s : %s\n", name.c_str(), msg.c_str());
    printf(">>>");
    fflush(stdout);
}

void WCNetwork::handleOnline(WCJson& json, uint32_t peerip)
{

    // Save peer's info
    string name = json.get(WC_NAME);
    WCCore::instance()->add_user(peerip, name);

    // Encode json
    WCJson resp;
    resp.add(WC_CMD, WC_ONLINEACK);
    resp.add(WC_NAME, WCCore::instance()->_name);
    send(resp.print(), peerip);
}

void WCNetwork::handleOnlineAck(WCJson& json, uint32_t peerip)
{
    string name = json.get(WC_NAME);
    WCCore::instance()->add_user(peerip, name.c_str());
}

/* Send message */
void WCNetwork::send(string msg, uint32_t ip)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    addr.sin_addr.s_addr = ip;

    int ret = sendto(this->_udpfd, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr*)&addr, sizeof(addr));
    if(ret <= 0){
        perror("sendto");
        return;
    }
}





