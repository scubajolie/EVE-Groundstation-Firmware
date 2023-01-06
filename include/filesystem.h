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

void SD_listDir(fs::FS &fs, const char * dirname, uint8_t levels);

bool SD_createDir(fs::FS &fs, const char * path); 

bool SD_removeDir(fs::FS &fs, const char * path); 

bool SD_readFile(fs::FS &fs, const char * path);

bool SD_writeFile(fs::FS &fs, const char * path, const char * message);

bool SD_appendFile(fs::FS &fs, const char * path, const char * message);

bool SD_renameFile(fs::FS &fs, const char * path1, const char * path2);

bool SD_deleteFile(fs::FS &fs, const char * path);

void SD_testFileIO(fs::FS &fs, const char * path);

bool SD_initLogFile(fs::FS &fs, char * path, char * header);

#endif