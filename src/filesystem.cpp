#include <filesystem.h>
#include <pins.h>
#include <SD.h>
#include <SPI.h>



// ESP32: Guide for MicroSD Card Module using Arduino IDE
// Used for this Filesystem
// https://randomnerdtutorials.com/esp32-microsd-card-arduino/
#define DEBUG_SERIAL Serial // TODO: Declare this in a variants file

char         TestFilePath[80];
const char * EVEDIR        = "/EVE";
const char * TelemetryFile;
const char * LogFile;
const char * CommandFile;
const char * StateFile;
char * FilePath;

// TODO: Change ifdefs so that when groundstation is connected
// all error messages are sent.

int initSDcard() {
    DEBUG_SERIAL.print("Initializing filesystem...");
    SPIClass SD_SPI(FSPI);
    SD_SPI.begin(FSPI_SCLK_PIN, FSPI_MISO_PIN, FSPI_MOSI_PIN, SD_CS_PIN);
    // bool _success = SD.begin(SD_CS_PIN, SD_SPI);
    if (SD.begin(SD_CS_PIN, SD_SPI)){
        Serial.println("SD opened!");
        return 0;
    } else {
        Serial.println("SD card failed to open");
        return -1;
    }
    return 0;
}

int SD_listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Listing directory: %s\n\r", dirname);
    #endif

    File root = fs.open(dirname);
    if (!root) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open directory");
        #endif

        return 0;
    }
    if (!root.isDirectory()) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Not a directory");
        #endif
        return 0;
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
    return 0;
}

int SD_createDir(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Creating Dir: %s ...", path);
    #endif
    bool _success = fs.mkdir(path);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "done!" : "couldn't create directory!");
    #endif

    if (_success== true) {
        return 0;
    } else {
        return FILE_FAIL_GENERAL;
    }
}

int SD_removeDir(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Removing Dir: %s ", path);
    #endif

    bool _success = fs.rmdir(path);

    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "done!" : "couldn't remove directory!");
    #endif

    if (_success== true) {
        return 0;
    } else {
        return FILE_FAIL_GENERAL;
    }
}

int SD_readFile(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Reading file: %s\r\n", path);
    #endif

    File file = fs.open(path);
    if (!file) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open file for reading");
        #endif
        return FILE_FAIL_COULDNT_OPEN;
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
    return 0;
}

int SD_writeFile(fs::FS &fs, const char * path, const char * message) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Writing file: %s\n\r", path);
    #endif

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        #ifdef DEBUG_SERIAL
            DEBUG_SERIAL.println("Failed to open file for writing");
        #endif
        return FILE_FAIL_COULDNT_OPEN;
    }
        
    bool _success = file.print(message);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "Message written!" : "Failed to write message");
    #endif
    file.close();

    if (_success == true) {
        return 0;
    } else {
        return FILE_FAIL_GENERAL;
    }
}

int SD_appendFile(fs::FS &fs, const char * path, const char * message) {
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.printf("Appending file: %s\n\r", path);
    #endif

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        #ifdef SDCARD_DEBUG
            DEBUG_SERIAL.println("Failed to open file for appending");
        #endif
        return FILE_FAIL_COULDNT_OPEN;
    }

    bool _success = file.print(message);
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.println(_success ? "Message appended!" : "Failed to append message");
    #endif
    file.close();

    if (_success == true) {
        return 0;
    } else {
        return FILE_FAIL_GENERAL;
    }
}

int SD_renameFile(fs::FS &fs, const char * path1, const char * path2) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Renaming file %s to %s\n\r", path1, path2);
    #endif
    bool _success = fs.rename(path1, path2);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "File renamed" : "Failed to rename file");
    #endif

    if (_success == true) {
        return 0;
    } else {
        return FILE_FAIL_GENERAL;
    }
}

int SD_deleteFile(fs::FS &fs, const char * path) {
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.printf("Deleting file: %s\n\r", path);
    #endif
    bool _success = fs.remove(path);
    #ifdef DEBUG_SERIAL
        DEBUG_SERIAL.println(_success ? "File deleted" : "Failed to delete file");
    #endif

    if (_success == true) {
        return 0;
    } else {
        return FILE_FAIL_GENERAL;
    }
}

int SD_testFileIO(fs::FS &fs, const char * path) {
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
        return FILE_FAIL_COULDNT_OPEN;
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
    return 0;
}

char * SD_initFile(fs::FS &fs, const char * path, PacketType packet, const char * header) {
    char FileName[80];

    strcpy(FilePath, path);
    
    for (uint8_t x=0; x<255; x++) { // Initialize log file
        switch(packet) {
            case (PacketType) TELEMETRY_PACKET:
            {
                sprintf(FileName, "/TELEMETRY_%03d.csv", x);
                break;
            }
            case (PacketType) LOG_PACKET:
            {
                sprintf(FileName,"/LOG_%03d.txt", x);
                break;
            }
            case (PacketType) COMMAND_PACKET:
            {
                sprintf(FileName, "/COMMAND_%03d.txt", x);
                break;
            }
            case (PacketType) STATE_PACKET:
            {
                sprintf(FileName,"/STATE_%03d.txt",x);
                break;
            }
        }
        
        strcat(FilePath, FileName);

        if (!fs.exists(FilePath)) break; // If a new unique log file has been named, exit loop
        if (x==254) 
        {
              sprintf(FilePath, "%d", FILE_FAIL_NO_UNIQUE_NAME);
            return  FilePath;  // If no unique log could be created, return an error
        }
        // TODO: remove filename without messing with filepath each time
        memset(FilePath, '\0', 80);

        strcat(FilePath, path);
    }

    if (!fs.open(FilePath, FILE_WRITE)) {
        #ifdef SDCARD_DEBUG
            DEBUG_SERIAL.println("Unable to open file for writing");
        #endif
        sprintf(FilePath, "%d", FILE_FAIL_COULDNT_OPEN);
        return FilePath; // If unable to open the new log file, return an error
    }
    #ifdef SDCARD_DEBUG
        DEBUG_SERIAL.printf("Logging to: %s", path);
    #endif

    // Write header for the log file
    SD_writeFile(fs, FilePath, header);
    return FilePath;
} 