#include <radio.h>
#include <SPI.h>
#include <telemetry.h>
#include <filesystem.h>
#include <LoRa.h>

int counter = 0;

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
    Serial.println(counter);

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();
    counter++;

}