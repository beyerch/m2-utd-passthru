#pragma once

#include <stdint.h>

// Command ID's
#define CMD_LOG 0x01
#define CMD_PING 0x02
#define CMD_EXIT 0xFF;

struct PCMSG { // Total 512 bytes
    uint8_t cmd_id;
    uint16_t arg_size;
    uint8_t args[509];
};

namespace usbcomm
{
    bool pollMessage(PCMSG* msg);
    bool sendMessage(PCMSG* msg);
    bool isConnected();
    bool OpenPort();
    void ClosePort();
};

