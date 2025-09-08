#include "../include/utils/string_utils.h"
#include <cctype>

using namespace std;

void ltrim(string &s) { 
    while (!s.empty() && isspace((unsigned char) s.front())) 
        s.erase(s.begin()); 
}

void rtrim(string &s) { 
    while (!s.empty() && isspace((unsigned char) s.back())) 
        s.pop_back(); 
}

void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

string stripQuotes(string v) {
    if (v.size() >= 2 && ((v.front() == '"' && v.back() == '"') || (v.front() == '\'' && v.back() == '\''))) {
        return v.substr(1, v.size() - 2);
    }
    return v;
}
