#ifndef WCUTIL_H
#define WCUTIL_H

#include <list>
#include <string>
#include <inttypes.h>
#include "WC.h"
using namespace std;

class WCUtil
{
public:
    WCUtil();

    static char* getHostname();
    static list<uint32_t> getLocalIpAddress();

    /* Thread safe function transfer network ip to host ip */
    static string ipaddr(uint32_t ip);

    /* Judge whether the ip is a broadcast ip(e.g x.x.x.255) */
    static bool isBroadcast(string ip);
};

#endif // WCUTIL_H
