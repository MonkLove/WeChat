#ifndef WCJSON_H
#define WCJSON_H

#include <string>
#include "cJSON.h"
using namespace std;

class WCJson
{
private:
    cJSON* _root; /* Json data */

public:
    WCJson();
    ~WCJson();
    /* Add item to _root to format json data */
    void add(string key, string value);

    /* Transfer json data to string                    *\
    \* Return target string on success, NULL on failure*/
    string print();

    /* Transter char buf to json data          *\
    \* Return true on success, false on failure*/
    bool parse(char* buf);

    /* Get value by key */
    string get(string key);
};

#endif // WCJSON_H
