#include "can_handler.h"
#include "pc_comm.h"

// Try to init CAN interface on one of the 2 avaliable built in interfaces
canbus_handler::canbus_handler(CANRaw* can) {
    if (!can) {
        PCCOMM::logToSerial("CONSTRUCTOR - WTF Can is null!?");
        return;
    }
    this->can = can;
}

// Is this interface handler free to be claimed?
bool canbus_handler::isFree() {
    return !this->inUse;
}

// Set a filter on one of the Rx mailboxes
void canbus_handler::setFilter(uint32_t canid, uint32_t mask, bool isExtended) {
    char buf[40] = {0x00};
    sprintf(buf, "Setting Rx Filters - MASK: 0x%04X, Filter: 0x%04X", mask, canid);
    PCCOMM::logToSerial(buf);
    for (int i = 0; i < 7; i++) {
        this->can->setRXFilter(canid, mask, isExtended);
    }
}

// Transmits a frame on the bus
void canbus_handler::transmit(CAN_FRAME f) {
    if (!this->can) {
        PCCOMM::logToSerial("TRANSMIT - WTF Can is null!?");
        return;
    }
    digitalWrite(this->actLED, LOW);
    char buf[100] = {0x00};
    sprintf(buf, "CAN Sending CAN Frame. ID: 0x%04X - DLC: %d", f.id, f.length);
    PCCOMM::logToSerial(buf);
    if (!this->can->sendFrame(f)) {
        PCCOMM::logToSerial("Error sending CAN Frame!");
    }
}
 // Attempts to read an avaliable frame from one of the mailboxes
bool canbus_handler::read(CAN_FRAME f) {
    if (this->can->available() > 0) {
        digitalWrite(this->actLED, LOW);
        this->can->read(f);
        return true;
    }
    return false;
}

// Locks the interface - Stops another channel from using it
void canbus_handler::lock(uint32_t baud) {
    PCCOMM::logToSerial("Locking CAN Interface");
    if (!this->can) {
        PCCOMM::logToSerial("LOCK - WTF Can is null!?");
        return;
    }
    this->can->init(baud);
    PCCOMM::logToSerial("CAN enabled andbaud set!");
    this->inUse = true;
}

// Unlocks the interface - Marks it as being avaliable for a new channel
void canbus_handler::unlock() {
    PCCOMM::logToSerial("Unlocking CAN Interface");
    if (!this->can) {
        PCCOMM::logToSerial("UNLOCK - WTF Can is null!?");
        return;
    }
    this->can->disable();
    PCCOMM::logToSerial("CAN Disabled!");
    this->inUse = false;
    //TODO Clear Tx and Rx buffers here, and put CAN To sleep
}

// nullptr implies they are not used yet
extern canbus_handler ch0 = canbus_handler(&Can0); // First avaliable interface  (Use can0)
extern canbus_handler ch1 = canbus_handler(&Can1); // Second avaliable interface (Use can1)