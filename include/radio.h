/* Defines the types of packets sent and recieved by the launchsonde.
*/

#ifndef EVE_PACKETTYPE_H
#define EVE_PACKETTYPE_H

enum PacketType {
    TELEMETRY_PACKET, // 0
    LOG_PACKET,       // 1
    COMMAND_PACKET,   // 2
    STATE_PACKET      // 3
};

#endif