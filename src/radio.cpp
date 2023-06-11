#include <radio.h>
#include <telemetry.h>
#include <filesystem.h>
#include <logging.h>
#include <LoRa.h>
#include <pins.h>

int radiocounter = 0;

void initRadio() {
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
}

void radioListen() {
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        // received a packet
        // Serial.print("Received packet '");
        
        // byte UUID = LoRa.read(); // First byte is the Launchsonde UUID
        // byte destUUID = LoRa.read(); // Second byte is the destination UUID
        // byte packetType = LoRa.read(); // Third byte is the packet type
        Serial.println("Received packet '");


        // read packet
        char _buf[sizeof(data)+1];
        int i = 0;
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
            // char _byte = LoRa.read();
            // _buf[i] = _byte;
            // i++;
        }
        // memcpy(&data, _buf, sizeof(data)+1);

        // check packet type, write to terminal and file
        // TO-DO: Write to files for each type of packet
        // TO-DO: Debug file system lol
        // switch(packetType) {
        //     case TELEMETRY_PACKET:
        //     {
        //         printBaseStationTelemetry();
        //         SD_appendFile(SD,TelemetryFile,_buf);
        //     break;
        //     }
        //     case LOG_PACKET:
        //     {
        //         #ifdef DIAGNOSTIC
        //             Serial.println("Log packet Recieved");
        //         #endif
        //         int success = SD_appendFile(SD,LogFile,_buf);
        //         if (success == 0) {
        //             serialError("Failed to write Logging Data", success);
        //         }
        //     break;
        //     }
        //     case COMMAND_PACKET:
        //     {
        //         #ifdef DIAGNOSTIC
        //             Serial.println("Command Recieved");
        //         #endif
        //         int success = SD_appendFile(SD,CommandFile,_buf);
        //         if (success == 0) {
        //             serialError("Failed to write to Command File", success);
        //         }
        //     }
        //     break;
        //     case STATE_PACKET:
        //     {
        //         #ifdef DIAGNOSTIC
        //             Serial.println("State Recieved");
        //         #endif
        //         int success = SD_appendFile(SD,StateFile,_buf);
        //         if (success == 0) {
        //             serialError("Failed to write Log Packet", success);
        //         }
        //     break;
        //     }
        //     default:
        //         Serial.println("Unknown LoRa Packet Type");

        //     break;
        // }
    }
}

void radioSend() {
    Serial.println("Sending Packet: ");
    Serial.println(radiocounter);

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(radiocounter);
    LoRa.endPacket();
    radiocounter++;

    delay(5000);

}