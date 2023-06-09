#include <Context.hpp>

int Context::createConnection(const char *ipv4_address, unsigned int port){

}

int Context::reconnect() {

}

void Context::setDevice(struct device_identification device) {

}

size_t Context::sendMessage(struct buffer message) const {

}

uint32_t Context::readSizeFromSocket() const {

}

std::string Context::readMessageFromSocket(uint32_t commandSize) const {

}

void Context::saveCommand(const std::string &command) {

}

Context::~Context() {

}
