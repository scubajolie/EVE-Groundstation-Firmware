#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <pins.h>
#include <LoRa.h>
#include <radio.h>
#include <fstream>
#include <ESP32FtpServer.h>
#include <telemetry.h>
#include <filesystem.h>

#ifndef SERIAL
    #define SERIAL
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
        int status = ESP32_setup();
        if (status != 0) {
            serialError("ESP32_setup failed.", status );
            return;
        }
    #endif

	// Initialize LoRa Radio
	Serial.print("Initializing radio...");
    LoRa.setPins(RFM_CS_PIN, RFM_RST_PIN, RFM_IRQ_PIN);
    LoRaSPI.begin(FSPI_SCLK_PIN, FSPI_MISO_PIN, FSPI_MOSI_PIN, FSPI_CS_PIN);
    LoRa.setSPI(LoRaSPI);

    // TO-DO: Add Blink header file to have error blink codes for Core
    if (!LoRa.begin(915E6)) {
        serialError("G: Starting LoRa failed!", 0);
        // while(true) blinkCode((byte) RADIO_ERROR_CODE, RED); // Block further code execution
    }
    LoRa.setSyncWord(0xF3);
    Serial.println("done!");

    // Initialize files for Telemetery, State, and Log data
    #ifndef SERIAL
        int status = initSDcard();
        if (status != 0){
            serialError("SD card setup failed.", status);
        }
    #endif

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
    strcat(TestFilePath,"/TestFile.txt");
    SD_writeFile(SD, TestFilePath, "This is a test file!");

    // TODO: Check for error
    const char * TelemetryFile = SD_initFile(SD,EVEDIR,PacketType(TELEMETRY_PACKET),"Telemetry File:/n");
    const char * LogFile = SD_initFile(SD,EVEDIR,PacketType(LOG_PACKET),"Logging File:/n");
    const char * CommandFile = SD_initFile(SD,EVEDIR,PacketType(COMMAND_PACKET),"Command File:/n");
    const char * StateFile = SD_initFile(SD,EVEDIR,PacketType(STATE_PACKET),"State File:/n");

    // Date Time [Source] [Type] [#]: Message
    // TODO: Find Logging Library?

    SD_listDir(SD,"/",0);

    // Serial.println("Timestamp (ISO8601),voltage,GPSFix,numSats,HDOP,latitude (°),longitude (­°),speed (kts),course (°), \
                    barometer temp (°C),pressure (Pa),altitude AGL (m),sysCal,gyroCal,accelCal,magCal,accelX (m/s), \
                    accelY (m/s),accelZ (m/s),gyroX (rad/s),gyroY (rad/s),gyroZ (rad/s), roll (°), pitch (°), yaw (°), \
                    linAccelX (m/s),linAccelY (m/s),linAccelZ (m/s),imu temp (°C),sht temp (°C),humidity (%),state,\
                    packet size (b),period (s),frequency (Hz),RSSI (dB),dX (m),dY (m),displacement (m)");
}

void loop() {

    ESP32_loop();

    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        // received a packet
        // Serial.print("Received packet '");
        
        byte UUID = LoRa.read(); // First byte is the Launchsonde UUID
        byte destUUID = LoRa.read(); // Second byte is the destination UUID
        byte packetType = LoRa.read(); // Third byte is the packet type

        // read packet
        char _buf[sizeof(data)+1];
        int i = 0;
        while (LoRa.available()) {
            char _byte = LoRa.read();
            _buf[i] = _byte;
            i++;
            // Serial.print(_byte, HEX); // DEBUG
        }
        memcpy(&data, _buf, sizeof(data)+1);

        // check packet type, write to terminal and file
        // TO-DO: Write to files for each type of packet
        // TO-DO: Debug file system lol
        switch(packetType) {
            case TELEMETRY_PACKET:
            {
                printBaseStationTelemetry();
                SD_appendFile(SD,TelemetryFile,_buf);
            break;
            }
            case LOG_PACKET:
            {
                #ifdef DIAGNOSTIC
                    Serial.println("Log packet Recieved");
                #endif
                int success = SD_appendFile(SD,LogFile,_buf);
                if (success == 0) {
                    serialError("Failed to write Logging Data", success);
                }
            break;
            }
            case COMMAND_PACKET:
            {
                #ifdef DIAGNOSTIC
                    Serial.println("Command Recieved");
                #endif
                int success = SD_appendFile(SD,CommandFile,_buf);
                if (success == 0) {
                    serialError("Failed to write to Command File", success);
                }
            }
            break;
            case STATE_PACKET:
            {
                #ifdef DIAGNOSTIC
                    Serial.println("State Recieved");
                #endif
                int success = SD_appendFile(SD,StateFile,_buf);
                if (success == 0) {
                    serialError("Failed to write Log Packet", success);
                }
            break;
            }
            default:
                Serial.println("Unknown LoRa Packet Type");

            break;
        }
    }
}