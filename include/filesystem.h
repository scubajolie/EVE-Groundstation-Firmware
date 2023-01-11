/* Defines multiple functions capable of interacting with files
* stored on an SD card connected to the ESP32 on the groundstation
*
**/


#ifndef EVE_FILESYSTEM_H
#define EVE_FILESYSTEM_H

#define SETUP_FAIL_SD_CARD -4

#include <stdint.h>
#include <FS.h>

extern const char * TelemetryFile;
extern const char * StateFile;
extern const char * LogFile;
extern const char * CommandFile;

// TODO: Change return to int using defined error types. 
// TODO: adjust SD_initLogFile to take filename parameter (not just used for logging)

/* Initializes the SD card and checks its card type.
*  Note: do not call if using ESP32 FTP server (duplicate statements)
*   @return success boolean Default: 0
*/
int initSDcard();

/* Finds all files and subfolders within a specified directory.
*   Directory name, File names and sizes are printed to the Serial port.
*   @param: filesystem (always SD)
*   @param: full filepath string with name
*   @param: level integer of subfolder in directory that is being checked.
*/
void SD_listDir(fs::FS &fs, const char * dirname, uint8_t levels);

/* creates desired directory at specified path synchronously
*   @param: filesystem (SD)
*   @param: full filepath string with name
*   @return: success boolean Default: true */
bool SD_createDir(fs::FS &fs, const char * path); 

/* Removes an EMPTY directory
*   @param: filesystem (SD)
*   @param: full filepath string with name
*   @return: success boolean Default: true */
bool SD_removeDir(fs::FS &fs, const char * path); 

/* Opens and reads the specified file, then closes it.
*  If DEBUG_SERIAL is defined, contents are written to Serial port. 
*   @param: filesystem (SD)
*   @param: full filepath string with name
*   @return: success boolean Default: true */
bool SD_readFile(fs::FS &fs, const char * path);

/* Opens the file, rewrites file contents, closes the file.
*   @param: filesystem (SD)
*   @param: full filepath string with name
*   @param: message string to be stored in file
*   @return: success boolean Default: true */
bool SD_writeFile(fs::FS &fs, const char * path, const char * message);

/* Opens the file, adds message to end of file, closes the file.
*   @param: filesystem (SD)
*   @param: full filepath string with name
*   @param: message string to be appended
*   @return: success boolean Default: true */
bool SD_appendFile(fs::FS &fs, const char * path, const char * message);

/* Opens the file, renames the file, closes the file.
*   @param: filesystem (SD)
*   @param: original file path (with name)
*   @param: new filepath string with name
*   @param: message string to be appended
*   @return: success boolean Default: true */
bool SD_renameFile(fs::FS &fs, const char * path1, const char * path2);

/* 
*   @param: filesystem (SD)
*   @param: string with path name
*   @param: message string to be appended
*   @return: success boolean Default: true */
bool SD_deleteFile(fs::FS &fs, const char * path);

/* Writes about 1 MB (2048 * 512 bytes) of data to the file path that you provide and measures the write time
*  Reads the same file and measures the read time
*   @param: filesystem (SD)
*   @param: full filepath string with name
*/
void SD_testFileIO(fs::FS &fs, const char * path);

/* Attempts to create logfile using sprintf with unique filename
*  using a recursive loop between 0 and 255; capable of recording 254 files.
*  checks to see if file can be opened, returns false if not
*  calls SD_writeFile() to create the file and write the header
*   @param: filesystem (SD)
*   @param: full filepath string with name
*   @param: header string for file
*   @return: success boolean Default: true */
bool SD_initLogFile(fs::FS &fs, char * path, char * header);

#endif