#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "WCUtil.h"


WCUtil::WCUtil()
{

}

char* WCUtil::getHostname()
{
    static char hostname[64] = {};
    if(strlen(hostname) != 0){
        return hostname;
    }

    FILE* fp = fopen("/etc/hostname", "r");
    fgets(hostname, sizeof(hostname), fp);
    hostname[strlen(hostname) - 1] = 0;
    fclose(fp);

    return hostname;
}

list<uint32_t> WCUtil::getLocalIpAddress()
{
    list<uint32_t> ret;
    FILE* fp = popen("ifconfig  | grep inet | grep -v inet6 | awk '{print $2}' | awk -F \":\" '{print $2}'", "r");

    char buf[64];
    while(fgets(buf, sizeof(buf), fp)){/* May have multi ip address */
        buf[strlen(buf) - 1] = 0;
        uint32_t ip = inet_addr(buf);
        ret.push_back(ip);
    }
    pclose(fp);
    return ret;
}

/* Transfer network ip to host ip */
string WCUtil::ipaddr(uint32_t ip)
{
    uint8_t* p = (uint8_t*)&ip;
    char buf[16];
    sprintf(buf,"%d.%d.%d.%d", *p, *(p+1), *(p+2), *(p+3) );
    return string(buf);
}

bool WCUtil::isBroadcast(string ip)
{
    const char* p = ip.c_str();
    const char* pos = rindex(p, '.');
    pos++;
    if(strcmp(pos, "255") == 0){
        return true;
    }else{
        return false;
    }
}
