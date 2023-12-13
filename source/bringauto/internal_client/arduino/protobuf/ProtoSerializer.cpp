#include <ProtoSerializer.hpp>

#include <general_error_codes.h>
#include <internal_client.h>
#include <pb_encode.h>
#include <pb_decode.h>

namespace bringauto::internal_client::arduino::protobuf {
	
int ProtoSerializer::serializeInternalClientMessageToBuffer(InternalProtocol_InternalClient &internalClientMessage, buffer *buffer) {
	pb_ostream_t size_stream {0};
	pb_encode(&size_stream, InternalProtocol_InternalClient_fields, &internalClientMessage);

	if(allocate(buffer, size_stream.bytes_written) != OK){
		return NOT_OK;
	}
	
	pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *) buffer->data, size_stream.bytes_written);
	if(!pb_encode(&stream, InternalProtocol_InternalClient_fields, &internalClientMessage)){
		return NOT_OK; // probably the max size of some limit specified in InternalProtocol.options was exceeded, TODO add better return value?
	}

	return OK;
}

InternalProtocol_InternalClient
ProtoSerializer::createInternalStatus(const struct buffer &statusData, const InternalProtocol_Device &device) {
	InternalProtocol_InternalClient internalClientMessage = InternalProtocol_InternalClient_init_zero;

	internalClientMessage.which_MessageType = InternalProtocol_InternalClient_deviceStatus_tag;
	memcpy(&internalClientMessage.MessageType.deviceStatus.statusData.bytes, statusData.data, statusData.size_in_bytes);
	internalClientMessage.MessageType.deviceStatus.statusData.size = statusData.size_in_bytes;
	memcpy(&internalClientMessage.MessageType.deviceStatus.device, &device, sizeof(InternalProtocol_Device));
	internalClientMessage.MessageType.deviceStatus.has_device = true;

	return internalClientMessage;
}


int ProtoSerializer::checkAndParseCommand(String &command, const String &internalServerCommand,
										  const InternalProtocol_Device& device) {
	if(internalServerCommand.length() == 0) {
		return NOT_OK;
	}
	InternalProtocol_InternalServer internalServerMessage = InternalProtocol_InternalServer_init_zero;

	pb_istream_t stream = pb_istream_from_buffer((const unsigned char *)internalServerCommand.c_str(), internalServerCommand.length());

	if (!pb_decode(&stream, InternalProtocol_InternalServer_fields, &internalServerMessage)) {
		return COMMAND_INCORRECT; // probably the max size of sent bytes as specified in InternalProtocol.options was exceeded, TODO add better return value?
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

		char tmp[commandSize + 1];
		memcpy(tmp, (char *) internalServerMessage.MessageType.deviceCommand.commandData.bytes, commandSize);
		tmp[commandSize] = '\0';
		command = String(tmp);

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

int ProtoSerializer::checkConnectResponse(const String &internalServerConnectResponse,
										  const _InternalProtocol_Device &device) {
	if(internalServerConnectResponse.length() == 0) {
		return NOT_OK;
	}
	InternalProtocol_InternalServer internalServerMessage = InternalProtocol_InternalServer_init_zero;

	pb_istream_t stream = pb_istream_from_buffer((const unsigned char *)internalServerConnectResponse.c_str(), internalServerConnectResponse.length());

	if (!pb_decode(&stream, InternalProtocol_InternalServer_fields, &internalServerMessage)) {
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
