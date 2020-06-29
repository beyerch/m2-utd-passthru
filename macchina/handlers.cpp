#include "channels.h"
#include "pc_comm.h"


handler::handler(unsigned long baud) {
}

void handler::add_filter(uint8_t id, uint8_t type, uint32_t mask, uint32_t filter, uint32_t resp) {
    if (id > MAX_FILTERS_PER_HANDLER-1) {
        PCCOMM::logToSerial("Cannot add filter - ID is out of range");
        return;
    }
    if (this->filters[id-1] != nullptr) {
        PCCOMM::logToSerial("Cannot add filter - Already in use");
        return;
    }
    filters[id-1] = new handler_filter {
        id,
        type,
        mask,
        filter,
        resp
    };
    char buf[100] = {0x00};
    sprintf(buf, "Setting filter. Type: %02X, Mask: %08X, Filter: %08X, Resp: %08X", type, mask, filter, resp);
    PCCOMM::logToSerial(buf);
}

void handler::destroy_filter(uint8_t id) {
    if (this->filters[id-1] != nullptr) {
        delete filters[id-1];
        filters[id-1] = nullptr;
    } else {
         PCCOMM::logToSerial("Cannot remove filter - doesn't exist");
    }
}

// CAN stuff (Normal CAN Payloads)

can_handler::can_handler(unsigned long baud) : handler(baud) {
    PCCOMM::logToSerial("Setting up CAN Handler");
    if (h0 == nullptr) {
        h0 = new canbus_handler(0, baud);
        this->can_handle = h0;
    } else if (h1 == nullptr) {
        h1 = new canbus_handler(1, baud);
        this->can_handle = h1;
    } else {
        PCCOMM::logToSerial("NO AVALIABLE CAN HANDLERS!");
    }
}

void can_handler::update() {

}

void can_handler::destroy() {
    bool usescan1 = this->can_handle == h1;
    delete this->can_handle;
    if (usescan1) {
        h1 = nullptr;
    } else {
        h0 = nullptr;
    }
}

void can_handler::transmit(uint8_t* args, uint16_t len) {

}

void can_handler::add_filter(uint8_t id, uint8_t type, uint32_t mask, uint32_t filter, uint32_t resp) {
    handler::add_filter(id, type, mask, filter, resp);
}

// ISO 9141 stuff (K-Line)

iso9141_handler::iso9141_handler(unsigned long baud) : handler(baud) {
    PCCOMM::logToSerial("Setting up ISO9141 Handler");
}

void iso9141_handler::update() {

}

void iso9141_handler::destroy() {

}

void iso9141_handler::transmit(uint8_t* args, uint16_t len) {
    // TODO Kline stuff
}

void iso9141_handler::add_filter(uint8_t id, uint8_t type, uint32_t mask, uint32_t filter, uint32_t resp) {
    handler::add_filter(id, type, mask, filter, resp);
}


// ISO 15765 stuff (Big CAN Payloads)

iso15765_handler::iso15765_handler(unsigned long baud) : handler(baud) {
    PCCOMM::logToSerial("Setting up ISO15765 Handler");
    if (h0 == nullptr) {
        h0 = new canbus_handler(0, baud);
        this->can_handle = h0;
    } else if (h1 == nullptr) {
        h1 = new canbus_handler(1, baud);
        this->can_handle = h1;
    } else {
        PCCOMM::logToSerial("NO AVALIABLE CAN HANDLERS!");
    }
}

void iso15765_handler::update() {

}

void iso15765_handler::destroy() {
    bool usescan1 = this->can_handle == h1;
    delete this->can_handle;
    if (usescan1) {
        h1 = nullptr;
    } else {
        h0 = nullptr;
    }
}

void iso15765_handler::transmit(uint8_t* args, uint16_t len) {
    if (this->can_handle == nullptr) {
        PCCOMM::logToSerial("ISO15765 cannot transmit - Handler is null");
        return;
    }
    if (len-4 < 7) {
        CAN_FRAME f = {0x00};
        f.extended = false; // TODO Allow for extended frames
        f.id = args[0] << 24 | args[1] << 16 | args[2] << 8 | args[3];
        f.data.byte[0] = len-4;
        f.length = 8; // Always for ISO15765
        memcpy(&f.data.bytes[1], &args[4], len-4);
        this->can_handle->transmit(&f);
    } else {
        // TODO Multi-frame packets
    }
}

void iso15765_handler::add_filter(uint8_t id, uint8_t type, uint32_t mask, uint32_t filter, uint32_t resp) {
    handler::add_filter(id, type, mask, filter, resp);
}