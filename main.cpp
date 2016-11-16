#include "WC.h"
#include "WCNetwork.h"
#include "WCCore.h"
#include "WCJson.h"
#include "WCUtil.h"
#include "WCInput.h"
#include "WCFileServer.h"

#include <cstdio>

void showMenu()
{
    printf("****************** We Chat ******************\n");
    printf("* It's a Local Area Network chating program *\n");
    printf("* All the command supported :               *\n");
    printf("*   1) send <ip address> <message>          *\n");
    printf("*   2) sendf <ip address> <file name>       *\n");
    printf("*********************************************\n");
}

int main(int argc, char* argv[])
{
    showMenu();

    WCFileServer::instance()->runFileServer();

    WCNetwork *network = WCNetwork::instance();

    WCJson json;
    json.add(WC_CMD, WC_ONLINE);
    json.add(WC_NAME, WCUtil::getHostname());
    network->send(json.print());

    WCInput input;
    while(1){
        input.loop();
    }

    return 0;
}
