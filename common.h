#ifndef COMMON_H
#define COMMON_H

using namespace std;

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Common
{
public:
    Common();
    static vector<string>& split(const string &s, char delim, vector<string> &elems);
    static vector<string> split(const string &s, char delim);
};

#endif // COMMON_H
