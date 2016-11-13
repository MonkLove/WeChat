#include <cstdio>
#include "WCCore.h"
#include "WCUtil.h"
WCCore::WCCore()
{
    _ips = WCUtil::getLocalIpAddress();
    _name = WCUtil::getHostname();

    pthread_mutex_init(&this->mutex, NULL);
}

WCCore* WCCore::instance()
{
    static WCCore* obj = NULL;
    if(obj == NULL){
        obj = new WCCore;
    }
    return obj;
}

void WCCore::add_user(uint32_t ip, string name)
{
    pthread_mutex_lock(&mutex);

    map<uint32_t, Peer*>::iterator it = _peers.find(ip);
    if(it == _peers.end()){
#if 1
        Peer* peer;
        peer = new Peer;
        peer->_ip = ip;
        peer->_name = name;
        //_peers[ip] = peer;
        _peers.insert(make_pair(ip, peer));
#endif
    }else{
        _peers[ip]->_name = name;
    }

    pthread_mutex_unlock(&mutex);
}
