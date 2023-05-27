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

#ifndef SERIAL
    #define SERIAL Serial
#endif



/* Prints error message to the serial port. 
*  -@param msg: string describing error message
*  -@param error: error number (integer)
*/
void serialError(const char * msg, int error ) {
    Serial.printf( "ERROR [%d]: %s \n", error, msg );
}

/* Initializes the following: 
*   -- Serial connection
*   -- SD card
*   -- LoRa Radio
*   -- Files in root directory on SD card
*/
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

	// Initialize LoRa Radio
	Serial.print("Initializing radio...");
    SPIClass LoRaSPI(FSPI);
    LoRa.setPins(RFM_CS_PIN, RFM_RST_PIN, RFM_IRQ_PIN);
    LoRaSPI.begin(FSPI_SCLK_PIN, FSPI_MISO_PIN, FSPI_MOSI_PIN, RFM_CS_PIN);
    LoRa.setSPI(LoRaSPI);

    // TO-DO: Add Blink header file to have error blink codes for Core
    if (!LoRa.begin(915E6)) {
        serialError("G: Starting LoRa failed!", 0);
        // while(true) blinkCode((byte) RADIO_ERROR_CODE, RED); // Block further code execution
    }
    LoRa.setSyncWord(0xF3);
    Serial.println("done!");


    // Try to open EVE directory on SD card
    // if it does not exist, create the directory
    File EVE_Root = SD.open(EVEDIR);
    
    if (!EVE_Root) {
        Serial.println("No EVE directory. Making Directory...");
        int status = SD_createDir(SD, EVEDIR);
        if (status != 0) {
            serialError("failed to create EVE Directory", status);
        }
    } else if (!EVE_Root.isDirectory()) {
        serialError("Not a Directory.", FILE_FAIL_NOT_DIRECTORY);
    }
    strcat(TestFilePath, EVEDIR);
    strcat(TestFilePath,"/TestLog.txt");
    SD_writeFile(SD, TestFilePath, "Begin Test Log File:");

    // Serial.println("Timestamp (ISO8601),voltage,GPSFix,numSats,HDOP,latitude (°),longitude (­°),speed (kts),course (°), \
                    barometer temp (°C),pressure (Pa),altitude AGL (m),sysCal,gyroCal,accelCal,magCal,accelX (m/s), \
                    accelY (m/s),accelZ (m/s),gyroX (rad/s),gyroY (rad/s),gyroZ (rad/s), roll (°), pitch (°), yaw (°), \
                    linAccelX (m/s),linAccelY (m/s),linAccelZ (m/s),imu temp (°C),sht temp (°C),humidity (%),state,\
                    packet size (b),period (s),frequency (Hz),RSSI (dB),dX (m),dY (m),displacement (m)");
}

void loop() {

    // ESP32_loop();
    radioListen();
    
}