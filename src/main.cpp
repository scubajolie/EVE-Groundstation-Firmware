#include <pins.h>
#include <LoRa.h>
#include <radio.h>
#include <fstream>
#include <ESP32FtpServer.h>
#include <telemetry.h>
#include <filesystem.h>

/* Prints error message to the serial port. 
* @param msg: string describing error message
* @param error: error number (integer)
*/
void serialError(const char * msg, int error ) {
    Serial.printf( "ERROR [%d]: %s \n", error, msg );
}

void setup() {
	Serial.begin(115200);
	while(!Serial); // Wait for serial connection to open

    int status = ESP32_setup();
    if (status != 0) {
        serialError("ESP32_setup failed.", status );
        return;
    }

	// Initialize LoRa Radio
	Serial.print("Initializing radio...");
    LoRa.setPins(RFM_CS_PIN, RFM_RST_PIN, RFM_IRQ_PIN);
    LoRaSPI.begin(FSPI_SCLK_PIN, FSPI_MISO_PIN, FSPI_MOSI_PIN, FSPI_CS_PIN);
    LoRa.setSPI(LoRaSPI);

    // Initialize files for Telemetery, State, and Log data
    
    // SD_createDir(SD, "/EVEdir");
    // SD_writeFile(SD, TelemetryFile, "Telemetry Data File:");
    // SD_writeFile(SD, StateFile, "State Fle: ");
    // SD_writeFile(SD, LogFile, "Log File: ");
    // SD_writeFile(SD, CommandFile, "Command File:");

    // TO-DO: Add Blink header file to have error blink codes for Core
    if (!LoRa.begin(915E6)) {
        serialError("G: Starting LoRa failed!", 0);
        // while(true) blinkCode((byte) RADIO_ERROR_CODE, RED); // Block further code execution
    }
    LoRa.setSyncWord(0xF3);
    Serial.println("done!");

    Serial.println("Timestamp (ISO8601),voltage,GPSFix,numSats,HDOP,latitude (°),longitude (­°),speed (kts),course (°), \
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
                printBaseStationTelemetry();
                SD_appendFile(SD,TelemetryFile,_buf);
            break;
            case LOG_PACKET:
                #ifdef DIAGNOSTIC
                    Serial.println("Log packet Recieved");
                #endif
                SD_appendFile(SD,LogFile,_buf);
            break;
            case COMMAND_PACKET:
                #ifdef DIAGNOSTIC
                    Serial.println("Command Recieved");
                #endif
                SD_appendFile(SD,CommandFile,_buf);
            break;
            case STATE_PACKET:
                #ifdef DIAGNOSTIC
                    Serial.println("State Recieved");
                #endif
                SD_appendFile(SD,StateFile,_buf);
            break;
            default:
                Serial.println("Unknown LoRa Packet Type");
            break;
        }
    }
}