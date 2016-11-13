#include <cstdlib>
#include "WCJson.h"

WCJson::WCJson()
{
    _root = cJSON_CreateObject();
}

WCJson::~WCJson()
{
    cJSON_Delete(_root);
}

void WCJson::add(string key, string value)
{
    cJSON_AddItemToObject(_root, key.c_str(),
                          cJSON_CreateString(value.c_str()));

}

string WCJson::print()
{
    char* p = cJSON_Print(_root);
    string ret(p);
    free(p);
    return ret;
}

bool WCJson::parse(char* buf)
{
    cJSON* root = cJSON_Parse(buf);
    if(root == NULL){
        return false;
    }
    cJSON_Delete(_root);
    _root = root;
    return true;
}

/* Get value by key */
string WCJson::get(string key)
{
    cJSON* obj = cJSON_GetObjectItem(_root, key.c_str());
    return obj->valuestring;
}
