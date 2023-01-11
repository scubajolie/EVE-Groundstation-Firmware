#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <ESP32FtpServer.h>
#include <ESP32_files.h>
#include <filesystem.h>


// ESP32: Guide for MicroSD Card Module using Arduino IDE
// Used for this Filesystem
// https://randomnerdtutorials.com/esp32-microsd-card-arduino/
// #define DEBUG_SERIAL Serial // TODO: Declare this in a variants file

const char * TelemetryFile = "/TelemetryFile.txt";
const char * StateFile     = "/StateFile.txt";
const char * LogFile       = "/LogFile.txt";
const char * CommandFile   = "/CommandFile.txt";

// TODO: Change ifdefs so that when groundstation is connected
// all error messages are sent.

int initSDCard() {
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.print("Initializing filesystem...");
    #endif
    bool _success = SD.begin(14, SD_SPI);
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.println(_success ? "done!" : "Failed to initialize filesystem!");
    #endif
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return SETUP_FAIL_SD_CARD;
    }
    return 0;
}

void SD_listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Listing directory: %s\n\r", dirname);
    #endif

    File root = fs.open(dirname);
    if (!root) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open directory");
        #endif

        return;
    }
    if (!root.isDirectory()) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Not a directory");
        #endif
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            #ifdef DEBUG_SERIAL
                DEBUG_SERIAL.print("  DIR : ");
                DEBUG_SERIAL.println(file.name());
            #endif
            if (levels){
                SD_listDir(fs, file.path(), levels--);
            }
        } 
        else {
            #ifdef DEBUG_SERIAL
                DEBUG_SERIAL.print("  FILE: ");
                DEBUG_SERIAL.print(file.name());
                DEBUG_SERIAL.print("  SIZE: ");
                DEBUG_SERIAL.println(file.size());
            #endif
        }
        file = root.openNextFile();
    }
}

bool SD_createDir(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Creating Dir: %s ...", path);
    #endif
    bool _success = fs.mkdir(path);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "done!" : "couldn't create directory!");
    #endif
    return _success;
}

bool SD_removeDir(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Removing Dir: %s ", path);
    #endif
    bool _success = fs.rmdir(path);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "done!" : "couldn't remove directory!");
    #endif
    return _success;
}

bool SD_readFile(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Reading file: %s\r\n", path);
    #endif

    File file = fs.open(path);
    if (!file) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open file for reading");
        #endif
        return false;
    }

    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.print("Read from file: ");
    #endif
    while (file.available()) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.write(file.read());
        #endif
    }
    file.close();
    return true;
}

bool SD_writeFile(fs::FS &fs, const char * path, const char * message) {
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.printf("Writing file: %s\n\r", path);
    #endif
    
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        #ifdef SDCARD_DEBUG
            DEBUG_SERIAL.println("Failed to open file for writing");
        #endif
        return false;
    }

    bool _success = file.print(message);
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.println(_success ? "Message written!" : "Failed to write message");
    #endif
    file.close();
    return _success;
}

bool SD_appendFile(fs::FS &fs, const char * path, const char * message) {
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.printf("Appending file: %s\n\r", path);
    #endif

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        #ifdef SDCARD_DEBUG
            DEBUG_SERIAL.println("Failed to open file for appending");
        #endif
        return false;
    }

    bool _success = file.print(message);
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.println(_success ? "Message appended!" : "Failed to append message");
    #endif
    file.close();
    return _success;
}

bool SD_renameFile(fs::FS &fs, const char * path1, const char * path2) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Renaming file %s to %s\n\r", path1, path2);
    #endif
    bool _success = fs.rename(path1, path2);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "File renamed" : "Failed to rename file");
    #endif
    return _success;
}

bool SD_deleteFile(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Deleting file: %s\n\r", path);
    #endif
    bool _success = fs.remove(path);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "File deleted" : "Failed to delete file");
    #endif
    return _success;
}

void SD_testFileIO(fs::FS &fs, const char * path) {
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.printf("%u bytes read for %u ms\n\r", flen, end);
        #endif
        file.close();
    } 
    else {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open file for reading");
        #endif
    }

    file = fs.open(path, FILE_WRITE);
    if (!file) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open file for writing");
        #endif
        return;
    }

    size_t i;
    start = millis();
    for (i=0; i<2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("%u bytes written for %u ms\n\r", 2048 * 512, end);
    #endif
    file.close();
}

bool SD_initLogFile(fs::FS &fs, char * path, char * header) {
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        sprintf(path, "/log_%03d.csv", x);
        if (!fs.exists(path)) break; // If a new unique log file has been named, exit loop
        if (x==254) return false; // If no unique log could be created, return an error
    }
    if (!fs.open(path, FILE_WRITE)) {
        #ifdef SDCARD_DEBUG
            DEBUG_SERIAL.println("Unable to open file for writing");
        #endif
        return false; // If unable to open the new log file, return an error
    }
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.printf("Logging to: %s", path);
    #endif

    // Write header for the log file
    SD_writeFile(fs, path, header);
    return true;
}