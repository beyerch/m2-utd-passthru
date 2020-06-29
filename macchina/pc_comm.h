#pragma once

#ifndef PC_COMM_H
#define PC_COMM_H

#include <stdint.h>
#include <Arduino.h>

struct PCMSG { // Total 512 bytes
    uint8_t cmd_id;
    uint16_t arg_size;
    uint8_t args[509];
};


namespace PCCOMM {
    bool pollMessage(PCMSG *msg);
    void sendMessage(PCMSG *msg);
    void logToSerial(char* msg);
};


// Command ID's for Misc
#define CMD_LOG  0x01
#define CMD_PING 0x02

// Channel command ID's
#define CMD_CHANNEL_CREATE     0x03 // Creating a new channel
#define CMD_CHANNEL_DATA       0x04 // Send data to/from channel
#define CMD_CHANNEL_DESTROY    0x05 // Killing a channel
#define CMD_CHANNEL_IOCTL_REQ  0x06 // IOCTL request to device
#define CMD_CHANNEL_IOCTL_RESP 0x07 // IOCTL Response from device
#define CMD_CHANNEL_SET_FILTER 0x08 // Add a filter to a channel
#define CMD_CHANNEL_REM_FILTER 0x09 // Remove a filter from a channel;

#define CMD_EXIT 0xFF // If sent, device will reset itself back into default state (Assume use app has quit)


#endif