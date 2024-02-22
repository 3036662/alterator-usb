/*

This tool will generate data for android device
The output data will be printed to standart output

The data expected is
https://github.com/M0Rf30/android-udev-rules/blob/main/51-android.rules

*/
#include <curl/curl.h>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "curl_wrapper.hpp"



int main(int argc, const char *argv[]) {
 CurlWrapper curl;   
 
 const std::string url="https://raw.githubusercontent.com/M0Rf30/android-udev-rules/main/51-android.rules";
 std::string buffer=std::move(curl.perfomReq(url));
 std::istringstream ss(buffer);
 std::istringstream result;

 // split buffer by vendor   
 //std::vector<std::string> splitted;
 std::list<boost::iterator_range<std::string::iterator>> it_all_vendords;
 boost::find_all(it_all_vendords,buffer,"{idVendor}");
 // TODO loop
 int counter=0;
 int counter_pid=0;
 while (!it_all_vendords.empty()){
    auto first=it_all_vendords.begin()->begin();
    it_all_vendords.pop_front();
    std::string::iterator second;
    if (!it_all_vendords.empty())
         second=it_all_vendords.begin()->begin();   
    else 
        second=buffer.end();
    //std::cerr<<"Distance = "<< std::distance(first,second)<<std::endl;
    std::string token_vid(first,second);
    size_t vid_start=token_vid.find('\"');
    size_t vid_second=token_vid.find('\"',vid_start+1);
    std::string vid=token_vid.substr(vid_start+1,vid_second-vid_start-1);
    std::cerr << "========================="<<std::endl;
    std::cerr << "VID = "<<vid<<std::endl;   
    ++counter;
    // find all pids 
    std::list<boost::iterator_range<std::string::iterator>> it_all_pids;
    boost::find_all(it_all_pids,token_vid,"{idProduct}");
    while (!it_all_pids.empty()){
        auto pid_first = it_all_pids.begin()->begin();
        it_all_pids.pop_front();
        std::string::iterator pid_second;
        if (!it_all_pids.empty())
            pid_second = it_all_pids.begin()->begin();
        else 
            pid_second=token_vid.end();   
        std::string token_pid (pid_first,pid_second);
        //std::cerr << token_pid<<std::endl;
        size_t pos_start=token_pid.find('\"');
        size_t pos_second=token_pid.find('\"',pos_start+1);
        std::string pid=token_pid.substr(pos_start+1,pos_second-pos_start-1);   
        std::cerr <<" PID = "<< pid;  
        ++counter_pid; 
    }
    std::cerr << std::endl; 
 }  

 std::cerr << "VIDs =" << counter << " " << "PIDs = " <<counter_pid <<std::endl;

 return 0;
}