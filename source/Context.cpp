#include <Context.hpp>
#include <general_error_codes.h>

#include <SPI.h>
#include <Ethernet.h>
#include <Arduino.h>

int Context::createConnection(const char *ipv4_address, unsigned int port){

    if(socket_.connect(ipv4_address, port) != 1){
        return NOT_OK;
    }

    serverAddress_ = std::string(ipv4_address);
    serverPort_ = port;

    Serial.print("Connection to ");
    Serial.print(ipv4_address);
    Serial.print(":");
    Serial.print(String(port));
    Serial.println(" successful.");
    
    return OK;
}

int Context::reconnect() {
    socket_.stop();

    if(socket_.connect(serverAddress_.c_str(), serverPort_) != 1){
        return NOT_OK;
    }

    return OK;
}

void Context::setDevice(struct device_identification device) {
    device_.module = (InternalProtocol_Device_Module) device.module;
    strcpy(device_.deviceRole, device.device_role);
    strcpy(device_.deviceName, device.device_name);
    device_.deviceType = device.device_type;
    device_.priority = device.priority;
}

size_t Context::sendMessage(struct buffer message) {
    socket_.print(message.size_in_bytes);
    return socket_.write((char *) message.data, message.size_in_bytes);
}

uint32_t Context::readSizeFromSocket() {
    int value;
    byte bytes[4];

    for(int i = 0; i <= 4; i++){
        value = socket_.read();
        if(value < 0){
            return 0;
        }
        bytes[i] = value;
    }

    uint32_t commandSize = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
    return commandSize;
}

std::string Context::readMessageFromSocket(uint32_t commandSize) {
    char value;
    std::string message = "";

    for(int i = 0; i < commandSize; i++){
        value = socket_.read();
        message.append(1, value);
    }

    return message;
}

void Context::saveCommand(const std::string &command) {
    commandSize_ = command.length();
	commandData_ = std::make_unique<char[]>(commandSize_);
	std::memcpy(getCommandData(), command.data(), commandSize_);
}

Context::~Context() {
    socket_.stop();
}
