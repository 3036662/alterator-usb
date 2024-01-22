#include "utils.hpp"
#include <iostream>

std::string ToLisp(const vecPairs& vec){
    std::string res;
    res+="(";
    // name for list  
    res+=WrapWithQuotes(vec[0].second);
    res+=" ";
    for (const auto& pair : vec){    
        res+=WrapWithQuotes(pair.first);
        res+=" ";
        res+=WrapWithQuotes(pair.second);        
        res+=" ";   
    }
    res+=")";
    std::cerr << "result string: " <<std::endl <<res << std::endl;
    return res;
}


std::string WrapWithQuotes(const std::string& str){
    std::string res;
    res+="\"";
    res+=str;
    res+="\"";
    return res;
}
