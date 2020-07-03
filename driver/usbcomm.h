#pragma once

#include <stdint.h>

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

// Command responses (From macchina)
#define CMD_RES_FROM_CMD       0xA0 // This gets put onto the first nibble of a CMD Id if its the Macchina responding from it 
#define CMD_RES_STATE_OK       0x10 // Command sent to Macchina was OK - Args contains response data if necessary
#define CMD_RES_STATE_FAIL     0x20 // Command send to Macchina failed, args contains error message

#define CMD_EXIT 0xFF // If sent, device will reset itself back into default state (Assume use app has quit)

struct PCMSG { // Total 512 bytes
    uint8_t cmd_id;
    uint16_t arg_size;
    uint8_t args[509];
};


enum CMD_RES {
    // Send Failed (Macchina didn't even receive the request)
    SEND_FAIL = 0,
    // Send OK - And Macchina was OK processing request
    CMD_OK = 1,
    // Macchina failed when processing the request
    CMD_FAIL = 2,
    // Macchina did not respond in time
    CMD_TIMEOUT = 3
};

namespace usbcomm
{
    bool pollMessage(PCMSG* msg);
    CMD_RES sendMsg(PCMSG* msg, bool getResponse, unsigned long maxWaitMs);
    CMD_RES sendMsg(PCMSG* msg, bool getResponse);
    bool isConnected();
    bool OpenPort();
    void ClosePort();
    const char* getLastError();
};

