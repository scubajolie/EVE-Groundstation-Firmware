#ifndef EVE_LOGGING_H
#define EVE_LOGGING_H

enum LogLevel {
    FATAL = 1,      //Indicates a fatal event
    ERROR,          //Indicates a major error, but not fatal
    WARN,           //Indicates a substantial event that is not an error or fatal
    INFO,           //Indicates basic information for logging
    DEBUG,          //Indicates some parameter that is less important than basic information
    TRACE         //Indicates some information that is less important than debug information
};

void serialError(const char * msg, int error );

#endif