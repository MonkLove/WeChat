#ifndef WCNETWORK_H
#define WCNETWORK_H

#include <inttypes.h>
#include "WC.h"
#include "WCJson.h"

class WCNetwork
{
private:
    int       _udpfd;            /* Socket file descriptor */
    pthread_t _tid;              /* Thread id for handling network message*/
    char      _buf[MAX_MSG_LEN]; /* Buffer to store message from network */

    WCNetwork();

    /* Functions to handle message from network*/
    /* Parameters : json --> json data recvfrom network
    **              peerid --> peer end ip address(network ip)
    */
    void handleOnline(WCJson& json, uint32_t peerip);
    void handleOnlineAck(WCJson& json, uint32_t peerip);
    void handleSend(WCJson& json);

public:
    /* Singleton */
    static WCNetwork* instance();

    /* To dispatch message from network to handles function */
    static void* msg_dispatch(void* arg);
    void* _msg_dispatch(void* arg);

    /* Send message to peer end */
    void send(string msg, uint32_t ip = 0xffffffff);
};

#endif // WCNETWORK_H4
