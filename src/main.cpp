/**
* PROJECT EVE
* FLORIDA INSTITUTE OF TECHNOLOGY, OEMS DEPARTMENT
* UNDERWATER TECHNOLOGY LABORATORY
* Supervising Professor: Dr. Stephen Wood, Ph.D, PE
*
* @file   Ground Station Firmware Version 1.1
* @date   4/26/2023
* @author Braidan Duffy & Jolie Elliott
* 
* Theory of Operation:
*/



#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <pins.h>
#include <LoRa.h>
#include <radio.h>
#include <fstream>
#include <telemetry.h>
#include <filesystem.h>
#include <logging.h>


#ifndef SERIAL
    #define SERIAL Serial
#endif

/* Initializes the following: 
*   -- Serial connection
*   -- SD card
*   -- LoRa Radio
*   -- Files in root directory on SD card
*/

int counter = 0; 
void setup() {
	Serial.begin(115200);
	while(!Serial); // Wait for serial connection to open

    // If connected to the serial port, establish wifi and FTP server connection. 
    #ifdef SERIAL
        int SDstatus = initSDcard();
        if (SDstatus != 0) {
            serialError("initSDcard failed.", SDstatus );
        }
    #endif

    initRadio();

    
	


    // Try to open EVE directory on SD card
    // if it does not exist, create the directory
    // File EVE_Root = SD.open(EVEDIR);
    
    // if (!EVE_Root) {
    //     Serial.println("No EVE directory. Making Directory...");
    //     int status = SD_createDir(SD, EVEDIR);
    //     if (status != 0) {
    //         serialError("failed to create EVE Directory", status);
    //     }
    // } else if (!EVE_Root.isDirectory()) {
    //     serialError("Not a Directory.", FILE_FAIL_NOT_DIRECTORY);
    // }
    // strcat(TestFilePath, EVEDIR);
    // strcat(TestFilePath,"/TestLog.txt");
    // SD_writeFile(SD, TestFilePath, "Begin Test Log File:");

    // Serial.println("Timestamp (ISO8601),voltage,GPSFix,numSats,HDOP,latitude (°),longitude (­°),speed (kts),course (°), \
                    barometer temp (°C),pressure (Pa),altitude AGL (m),sysCal,gyroCal,accelCal,magCal,accelX (m/s), \
                    accelY (m/s),accelZ (m/s),gyroX (rad/s),gyroY (rad/s),gyroZ (rad/s), roll (°), pitch (°), yaw (°), \
                    linAccelX (m/s),linAccelY (m/s),linAccelZ (m/s),imu temp (°C),sht temp (°C),humidity (%),state,\
                    packet size (b),period (s),frequency (Hz),RSSI (dB),dX (m),dY (m),displacement (m)");
}

void loop() {

    // ESP32_loop();
    radioListen();
    // radioSend();

}