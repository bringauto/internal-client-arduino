#include <Context.hpp>
#include <general_error_codes.h>

#include "SPI.h"
#include "Ethernet.h"

namespace bringauto::internal_client::arduino {

int Context::createConnection(const char *ipv4_address, unsigned int port){

    if(!socket_.connect(ipv4_address, port)){
        return NOT_OK;
    }

    serverAddress_ = String(ipv4_address);
    serverPort_ = port;
    
    return OK;
}

int Context::reconnect() {
    socket_.stop();

    if(!socket_.connect(serverAddress_.c_str(), serverPort_)){
        return NOT_OK;
    }

    return OK;
}

int Context::setDevice(struct device_identification device) {
    device_.module = (InternalProtocol_Device_Module) device.module;
    device_.deviceType = device.device_type;
    device_.priority = device.priority;

    if(device.device_role.size_in_bytes > sizeof(device_.deviceRole)) {
        return NOT_OK;
    }
    memcpy(device_.deviceRole, device.device_role.data, device.device_role.size_in_bytes);

    if(device.device_name.size_in_bytes > sizeof(device_.deviceName)) {
        return NOT_OK;
    }
    memcpy(device_.deviceName, device.device_name.data, device.device_name.size_in_bytes);

    return OK;
}

size_t Context::sendMessage(struct buffer message) {
    if(!socket_.connected()){
        return NOT_OK;
    }
    // Using uint32_t - ensures that 4 bytes will be sent
    uint32_t message_size = message.size_in_bytes;

    socket_.write((char *)(&message_size), sizeof(message_size));
    return socket_.write((char *) message.data, message.size_in_bytes);
}

uint32_t Context::readSizeFromSocket(unsigned timeout) {
    int value;
    byte bytes[4];
    unsigned time_spent = 0;
    const int delay_constant = 100;

    for(int i = 0; i < 4; i++){
        while(socket_.available() == 0){
            if(time_spent >= timeout*1000){
                return 0;
            }
            delay(delay_constant);
            time_spent += delay_constant;
        }
        value = socket_.read();
        if(value < 0){
            return 0;
        }
        bytes[i] = value;
    }

    uint32_t commandSize = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
    return commandSize;
}

String Context::readMessageFromSocket(uint32_t commandSize, unsigned timeout) {
    char value;
    String message = "";
    unsigned time_spent = 0;
    const int delay_constant = 100;

    for(uint32_t i = 0; i < commandSize; i++){
        while(socket_.available() == 0){
            if(time_spent >= timeout*1000){
                return ""; // TODO better return value? timeout exceeded?
            }
            delay(delay_constant);
            time_spent += delay_constant;
        }
        value = socket_.read();
        message += value;
    }

    return message;
}

void Context::saveCommand(const String &command) {
    commandSize_ = command.length();
    if(commandData_ != nullptr) {
        delete[] commandData_;
        commandData_ = nullptr;
    }
	commandData_ = new char[commandSize_];
	memcpy(getCommandData(), command.c_str(), commandSize_);
}

Context::~Context() {
    socket_.stop();
    if(commandData_ != nullptr) {
        delete[] commandData_;
    }
}

}
