//
// Created by Víctor Jiménez Rugama on 12/28/21.
//
#include "../../include/utils/logger.h"
#include "../../include/utils/notifi.h"

std::mutex logger::mtx;
std::ofstream logger::logStream;
logger::logger(const std::string &funcName) : lock(mtx) {
    auto currentTime = std::chrono::system_clock::now(); // get the time
    auto formattedTime = std::chrono::system_clock::to_time_t(currentTime); // convert it to time_t type (loses some precision)
    std::string time(std::ctime(&formattedTime));
    time.pop_back();
    logStream << "\n[" << time << "] " << funcName << ": ";
}

int logger::init(const char *PATH){
    logStream.open(PATH);
    if(logStream.fail()) {
        notifi(NULL,"LogStream Init failed!");
        return -1;
    }
    LOG << "Initialized Logger.";
    return 0;
}

logger& logger::operator<<(const char *message) {
    logStream << message;
    logStream.flush();
    return *this;
}
logger& logger::operator<<(const std::string& message){
    logStream << message;
    logStream.flush();
    return *this;
}

void logger::closeLogger() {
    if(logStream.is_open())
        logStream.close();
}





