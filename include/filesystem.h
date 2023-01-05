/* Defines multiple functions capable of interacting with files
* stored on an SD card connected to the ESP32 on the groundstation
*
**/


#ifndef EVE_FILESYSTEM_H
#define EVE_FILESYSTEM_H

#include <stdint.h>
#include <FS.h>

extern const char * TelemetryFile;
extern const char * StateFile;
extern const char * LogFile;
extern const char * CommandFile;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

bool createDir(fs::FS &fs, const char * path); 

bool removeDir(fs::FS &fs, const char * path); 

bool readFile(fs::FS &fs, const char * path);

bool writeFile(fs::FS &fs, const char * path, const char * message);

bool appendFile(fs::FS &fs, const char * path, const char * message);

bool renameFile(fs::FS &fs, const char * path1, const char * path2);

bool deleteFile(fs::FS &fs, const char * path);

void testFileIO(fs::FS &fs, const char * path);

bool initLogFile(fs::FS &fs, char * path, char * header);

#endif