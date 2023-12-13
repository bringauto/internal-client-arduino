#pragma once

#include <device_management.h>
#include <memory_management.h>
#include <InternalProtocol.pb.h>

#include "SPI.h"
#include "Ethernet.h"

namespace bringauto::internal_client::arduino {

class Context {
public:
	/**
	 * @brief Creates socket and connects it to the a server
	 *
	 * @param ipv4_address IPv4 address of the server
	 * @param port port number
	 *
	 * @return OK if connection was successfully created
	 * @return NOT_OK otherwise
	 */
	int createConnection(const char *ipv4_address, unsigned port);

	/**
	 * @brief Reconnects socket to server
	 *
	 * @return OK if reconnect was successful
	 * @return NOT_OK if and error occurred
	 */
	int reconnect();

	/**
	 * @brief Set device
	 * @param device
	 * @return OK if device wat successfully set
	 * @return NOT_OK if given device strings are larger than limit
	 */
	int setDevice(struct device_identification device);

	/**
	 * @brief Returns current device
	 * @return device
	 */
	InternalProtocol_Device getDevice() { return device_; }

	/**
	 * @brief Reads 4 bytes, that contains the length of a message that follows it in socket
	 *
	 * @param timeout timeout in seconds for tcp communication
	 * @return size in bytes of the next message
	 */
	uint32_t readSizeFromSocket(unsigned timeout);

	/**
	 * @brief Reads from message from socket
	 * Reads number of bytes given by parameter from socket and saves it to a string
	 *
	 * @param commandSize number of bytes to read
	 * @param timeout timeout in seconds for tcp communication
	 * @return string containing the message
	 */
	String readMessageFromSocket(uint32_t commandSize, unsigned timeout);

	/**
	 * @brief Sends message in buffer and return number of bytes sent
	 * First sends 4 bytes containing the message length, then the message in the buffer
	 * @param message buffer containing message that will be sent
	 * @return number of bytes sent
	 * @return -1 if server is not connected
	 */
	size_t sendMessage(struct buffer message);

	/**
	 * @brief Save commandData
	 * @param command string containing command data
	 */
	void saveCommand(const String &command);

	/**
	 * @brief Returns pointer to the command data
	 * To get the length of the command data, use getCommandSize() function
	 * @return pointer to char
	 */
	char* getCommandData() const { return commandData_; };

	/**
	 * @brief Return length of the command data
	 * @return Data size
	 */
	size_t getCommandSize() const { return commandSize_; };

	/**
	 * @brief Closes socket
	 */
	~Context();

private:
	/// Socket connected to the server
	EthernetClient socket_ {};
	/// Server address saved for reconnecting
	String serverAddress_;
	unsigned serverPort_;
	/// Protobuf Device message
	InternalProtocol_Device device_ {};
	/// Command data pointer
	char* commandData_ { nullptr };
	/// Size of commandData_
	size_t commandSize_ { 0 };

	/// Size of message header, where the length of the message is set
	static constexpr short headerSize_ { 4 };
};

}
