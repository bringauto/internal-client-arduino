#include <ProtoSerializer.hpp>

#include <general_error_codes.h>
#include <internal_client.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include <vector>

namespace protobuf {
struct buffer ProtoSerializer::serializeInternalClientMessageToBuffer(InternalProtocol_InternalClient &internalClientMessage) {
	struct buffer buffer;
	pb_ostream_t size_stream = {0};
	pb_encode(&size_stream, InternalProtocol_InternalClient_fields, &internalClientMessage);

	if(allocate(&buffer, size_stream.bytes_written) != OK){
		return buffer;
	}
	
	pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *) buffer.data, size_stream.bytes_written);
	if(!pb_encode(&stream, InternalProtocol_InternalClient_fields, &internalClientMessage)){
		Serial.println("Encoding failed");
	}
	buffer.size_in_bytes = stream.bytes_written;
	return buffer;
}

InternalProtocol_InternalClient
ProtoSerializer::createInternalStatus(const struct buffer &statusData, const InternalProtocol_Device &device) {
	InternalProtocol_InternalClient internalClientMessage = InternalProtocol_InternalClient_init_zero;

	internalClientMessage.which_MessageType = InternalProtocol_InternalClient_deviceStatus_tag;
	memcpy(&internalClientMessage.MessageType.deviceStatus.statusData.bytes, statusData.data, statusData.size_in_bytes);
	memcpy(&internalClientMessage.MessageType.deviceStatus.device, &device, sizeof(InternalProtocol_Device));
	internalClientMessage.MessageType.deviceStatus.has_device = true;

	return internalClientMessage;
}


int ProtoSerializer::checkAndParseCommand(std::string &command, const std::string &internalServerCommand,
										  const InternalProtocol_Device& device) {
	if(internalServerCommand.empty()) {
		return NOT_OK;
	}
	InternalProtocol_InternalServer internalServerMessage = InternalProtocol_InternalServer_init_zero;

	std::vector<uint8_t> messageVector(internalServerCommand.begin(), internalServerCommand.end());
	uint8_t *buffer = &messageVector[0];

	pb_istream_t stream = pb_istream_from_buffer(buffer, internalServerCommand.size());

	if (!pb_decode(&stream, InternalProtocol_InternalServer_fields, &internalServerMessage)) {
		digitalWrite(LED_D0, HIGH); // Debug
		return COMMAND_INCORRECT;
	}

	if (internalServerMessage.which_MessageType == InternalProtocol_InternalServer_deviceCommand_tag) {
		const InternalProtocol_DeviceCommand deviceConnectCommand = internalServerMessage.MessageType.deviceCommand;
		if(not internalServerMessage.MessageType.deviceCommand.has_device || 
			strcmp(deviceConnectCommand.device.deviceName, device.deviceName) != 0 || strcmp(deviceConnectCommand.device.deviceRole, device.deviceRole) != 0  ||
			deviceConnectCommand.device.deviceType != device.deviceType || deviceConnectCommand.device.module != device.module || 
			deviceConnectCommand.device.priority != device.priority) {
			return DEVICE_INCORRECT;
		}

		auto commandSize = internalServerMessage.MessageType.deviceCommand.commandData.size;
		command.assign(std::string((char *) internalServerMessage.MessageType.deviceCommand.commandData.bytes, commandSize));

		return OK;
	} else {
		return COMMAND_INCORRECT;
	}

}

InternalProtocol_InternalClient ProtoSerializer::createInternalConnect(const InternalProtocol_Device &device) {
	InternalProtocol_InternalClient internalClientMessage = InternalProtocol_InternalClient_init_zero;

	internalClientMessage.which_MessageType = InternalProtocol_InternalClient_deviceConnect_tag;
	memcpy(&internalClientMessage.MessageType.deviceConnect.device, &device, sizeof(InternalProtocol_Device));
	internalClientMessage.MessageType.deviceConnect.has_device = true;

	return internalClientMessage;
}

int ProtoSerializer::checkConnectResponse(const std::string &internalServerConnectResponse,
										  const _InternalProtocol_Device &device) {
	if(internalServerConnectResponse.empty()) {
		return NOT_OK;
	}
	InternalProtocol_InternalServer internalServerMessage = InternalProtocol_InternalServer_init_zero;

	std::vector<uint8_t> messageVector(internalServerConnectResponse.begin(), internalServerConnectResponse.end());
	uint8_t *buffer = &messageVector[0];

	pb_istream_t stream = pb_istream_from_buffer(buffer, internalServerConnectResponse.size());

	if (!pb_decode(&stream, InternalProtocol_InternalServer_fields, &internalServerMessage)) {
		digitalWrite(LED_D0, HIGH); // Debug
		return NOT_OK;
	}

	if (internalServerMessage.which_MessageType == InternalProtocol_InternalServer_deviceConnectResponse_tag) {
		const InternalProtocol_DeviceConnectResponse deviceConnectResponse = internalServerMessage.MessageType.deviceConnectResponse;
		if(strcmp(deviceConnectResponse.device.deviceName, device.deviceName) != 0 || strcmp(deviceConnectResponse.device.deviceRole, device.deviceRole) != 0  ||
			deviceConnectResponse.device.deviceType != device.deviceType || deviceConnectResponse.device.module != device.module || 
			deviceConnectResponse.device.priority != device.priority){
			return DEVICE_INCORRECT;
		}
		return OK;
	} else {
		return NOT_OK;
	}
}

}