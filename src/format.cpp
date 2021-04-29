#include <string>
#include "format.h"
#include <vector>
#include <cmath>

using std::string;

string Format::ElapsedTime(long seconds) { 
    float hour = (float)seconds/3600;
    float minutes = (hour - (long)hour) * 60;
    float secs = round((minutes - (long)minutes) * 60);
    
    std::vector<string> outputVec;
    outputVec.push_back(std::to_string((long)hour));
    outputVec.push_back(std::to_string((long)minutes));
    outputVec.push_back(std::to_string((long)secs));
    
    string output = "";
    for (unsigned i = 0; i < outputVec.size(); i++) {
        if (outputVec[i].length() < 2) {
            outputVec[i].insert(0,"0");    
        }
        output += outputVec[i];
        if (i < outputVec.size()-1) output += ":";
    }
    return output;
}