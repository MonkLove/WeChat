#ifndef WCCORE_H
#define WCCORE_H

#include <inttypes.h>
#include <string>
#include <list>
#include <map>
#include "WC.h"
using namespace std;


class Peer
{
public:
    string    _name;        /* Peer host's name */
    uint32_t  _ip;          /* Peer host's ip */
};


class WCCore
{
public:
    string _name;        /* Own host's name */
    list<uint32_t> _ips; /* Own host's ips */
    map<uint32_t, Peer*> _peers; /* Information about who is online */
    pthread_mutex_t mutex;


    WCCore();
public:
    /* Implementation of singleton */
    static WCCore* instance();

    /* To store other user's information */
    void add_user(uint32_t ip, string name);
};

#endif // WCCORE_H
