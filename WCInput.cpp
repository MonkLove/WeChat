#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "WCInput.h"
#include "WCCore.h"
#include "WCUtil.h"
#include "WCJson.h"
#include "WCNetwork.h"

WCInput::WCInput()
{

}

void WCInput::loop()
{
AGAIN:
    fflush(stdout);
    printf(">>>");
    memset(_buf, 0, sizeof(_buf));
    fgets(_buf, sizeof(_buf), stdin);
    if(strlen(_buf) <= 2)
        goto AGAIN;
    _buf[strlen(_buf) - 1] = 0;

    cmdSplit();
    cmdDispatch();
}

void WCInput::cmdSplit()
{
    _cmds.clear();

    char* save = NULL;
    char* first = strtok_r(_buf, " \t", &save);
    _cmds.push_back(first);

    char* second = strtok_r(NULL, " \t", &save);
    if(second){
        _cmds.push_back(second);
    }else{
        return;
    }

    char* third = strtok_r(NULL, "\0", &save);
    if(third){
        _cmds.push_back(third);
    }

    return;
}

void WCInput::cmdDispatch()
{
#define DISPATCH(cmd, func) if(_cmds[0] == cmd) func()
    DISPATCH(WC_SEND, cmdsend);
    DISPATCH(WC_LIST, cmdlist);
    DISPATCH(WC_SENDF, cmdsendf);
}

void WCInput::cmdsendf()
{
    if(_cmds.size() < 3){
        printf("Usage : sendf <ip> <filename>\n");
        return;
    }

    string& ip = _cmds[1];
    string& file = _cmds[2];

    //printf("ip : %s\n", ip.c_str());
    //printf("file : %s\n", file.c_str());

    WCJson json;
    json.add(WC_CMD, WC_SENDF);
    json.add(WC_NAME, WCCore::instance()->_name);
    json.add(WC_FILE, file);

    WCNetwork::instance()->send(json.print(), inet_addr(ip.c_str()));
}

void WCInput::cmdlist()
{
    WCCore* core = WCCore::instance();

    printf("User list :\n");

    map<uint32_t, Peer*>::iterator it = core->_peers.begin();
    for( ; it != core->_peers.end(); ++it){
        Peer* peer = it->second;
        const char* name = peer->_name.c_str();
        string ip = WCUtil::ipaddr(peer->_ip);

        printf("   %s(%s)\n", name, ip.c_str());
    }
}

void WCInput::cmdsend()
{
    if(_cmds.size() < 3){
        printf("Usage : sendf <ip> <message>\n");
        return;
    }

    string& ip = _cmds[1];
    string& msg = _cmds[2];

    WCJson json;
    json.add(WC_CMD, WC_SEND);
    json.add(WC_NAME, WCCore::instance()->_name);
    if(WCUtil::isBroadcast(ip)){/* Send all */
        json.add(WC_MSG, msg + "[broadcast]");
    }else{/* Send one */
        json.add(WC_MSG, msg);
    }

    DEBUG(msg.c_str());

    WCNetwork::instance()->send(json.print(), inet_addr(ip.c_str()));
}
