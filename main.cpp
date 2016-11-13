
#include "WC.h"
#include "WCNetwork.h"
#include "WCCore.h"
#include "WCJson.h"
#include "WCUtil.h"
#include "WCInput.h"

int main(int argc, char* argv[])
{

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
