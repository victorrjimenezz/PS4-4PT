//
// Created by Víctor Jiménez Rugama on 12/28/21.
//

#ifndef CYDI4_LOGGER_H
#define CYDI4_LOGGER_H
#include <iostream>
#include <fstream>
class logger {
private:
    std::mutex mtx;
    static std::ofstream logStream;
public:
    static int init(const char *PATH);
    explicit logger(const std::string &funcName);
    logger& operator<<(const char * message);
    logger& operator<<(const std::string& message);
    template<typename T>
    logger& operator<<(T value) {return logger::operator<<(std::to_string(value));};
    static void closeLogger();
    ~logger();
};

#define LOG logger(__FUNCTION__)

#endif //CYDI4_LOGGER_H
