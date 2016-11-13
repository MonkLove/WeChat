#ifndef WC_H
#define WC_H

/*#include <list>
#include <map>
#include <string>
#include <inttypes.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;*/

#define MAX_MSG_LEN 1024   /**/

#define UDP_PORT    23232   /**/

#define WC_CMD        "cmd"
#define WC_MSG        "msg"
#define WC_LIST       "list"
#define WC_NAME       "name"
#define WC_ONLINE     "online"
#define WC_ONLINEACK  "onlineack"
#define WC_SEND       "send"

#define DEBUG_ON 0x01
#ifdef DEBUG_ON
   #define DEBUG(s) printf("[%s:%d] - %s\n", __FILE__, __LINE__, (s))
#else
   #define DEBUG(s)
#endif

#endif // WC_H
