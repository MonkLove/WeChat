#ifndef WC_H
#define WC_H

#define MAX_MSG_LEN 1024   /**/

#define UDP_PORT    8111   /**/
#define TCP_PORT    8112

#define MAX_FDS     12   /* select's max fd num */

#define WC_CMD        "cmd"
#define WC_MSG        "msg"
#define WC_LIST       "list"
#define WC_NAME       "name"
#define WC_ONLINE     "online"
#define WC_ONLINEACK  "onlineack"
#define WC_SEND       "send"
#define WC_SENDF       "sendf"
#define WC_FILE        "file"

#define WC_SENTINEL   "5h5h5h"
#define WC_FILEEOF    "0"
#define WC_FILE_REG    "reg"
#define WC_FILE_DIR    "dir"

#define DEBUG_ON 0x01
#ifdef DEBUG_ON
   #define DEBUG(s) printf("[%s:%d] - %s\n", __FILE__, __LINE__, (s))
#else
   #define DEBUG(s)
#endif

#endif // WC_H
