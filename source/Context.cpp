#include <Context.hpp>
#include <general_error_codes.h>

#include <SPI.h>
#include <Ethernet.h>

int Context::createConnection(const char *ipv4_address, unsigned int port){

    if(!socket_.connect(ipv4_address, port)){
        return NOT_OK;
    }

    Serial.print("Connection to ");
    Serial.print(ipv4_address);
    Serial.print(":");
    Serial.print(std::to_string(port));
    Serial.println(" successful.");
    
    return OK;
}

int Context::reconnect() {
    socket._stop();

    if(!socket_.connect(ipv4_address, port)){
        return NOT_OK;
    }

    return OK;
}

void Context::setDevice(struct device_identification device) {
    device_.module = device.module;
    strcpy(device_.deviceRole, device.device_role);
    strcpy(device_.deviceName, device.device_name);
    device_.deviceType = device.device_type;
    device_.priority = device.priority;
}

size_t Context::sendMessage(struct buffer message) const {

}

uint32_t Context::readSizeFromSocket() const {

}

std::string Context::readMessageFromSocket(uint32_t commandSize) const {

}

void Context::saveCommand(const std::string &command) {
    commandSize_ = command.length();
	commandData_ = std::make_unique<char[]>(commandSize_);
	std::memcpy(getCommandData(), command.data(), commandSize_);
}

Context::~Context() {
    socket_.stop();
}
