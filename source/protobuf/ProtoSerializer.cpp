#include <protobuf/ProtoSerializer.hpp>

#include <general_error_codes.h>
#include <internal_client.h>
#include <google/protobuf/util/message_differencer.h>

namespace protobuf {
struct buffer ProtoSerializer::serializeProtobufMessageToBuffer(const google::protobuf::Message &protobufMessage) {
	struct buffer message {};
	if ((allocate(&message, protobufMessage.ByteSizeLong())) == OK) {
			protobufMessage.SerializeToArray(message.data, (int)message.size_in_bytes);
	}
	return message;
}

_InternalProtocol_InternalClient
ProtoSerializer::createInternalStatus(const struct buffer &statusData, const _InternalProtocol_Device &device) {
	_InternalProtocol_InternalClient internalClientMessage;

	_InternalProtocol_DeviceStatus *deviceStatus = internalClientMessage.mutable_devicestatus();
	_InternalProtocol_Device *devicePtr = deviceStatus->mutable_device();
	devicePtr->CopyFrom(device);

	deviceStatus->set_statusdata(statusData.data, statusData.size_in_bytes);

	return internalClientMessage;
}


int ProtoSerializer::checkAndParseCommand(std::string &command, const std::string &internalServerCommand,
										  const _InternalProtocol_Device& device) {
	if(internalServerCommand.empty()) {
		return NOT_OK;
	}
	_InternalProtocol_InternalServer internalServerMessage;

	if (not internalServerMessage.ParseFromString(internalServerCommand)) {
		return COMMAND_INCORRECT;
	}

	if (internalServerMessage.has_devicecommand()) {
		if(not internalServerMessage.devicecommand().has_device() || google::protobuf::util::MessageDifferencer::Equals(internalServerMessage.devicecommand().device(), device)) {
			return DEVICE_INCORRECT;
		}
		auto commandSize = internalServerMessage.devicecommand().commanddata().length();
		command.assign(internalServerMessage.devicecommand().commanddata().data(), commandSize);

		return OK;
	} else {
		return COMMAND_INCORRECT;
	}

}

_InternalProtocol_InternalClient ProtoSerializer::createInternalConnect(const _InternalProtocol_Device &device) {
	_InternalProtocol_InternalClient internalClientMessage;
	_InternalProtocol_DeviceConnect* deviceConnectMessage = internalClientMessage.mutable_deviceconnect();
	_InternalProtocol_Device *devicePtr = deviceConnectMessage->mutable_device(); // Returns pointer to device field in deviceConnectMessage
	devicePtr->CopyFrom(device);
	return internalClientMessage;
}

int ProtoSerializer::checkConnectResponse(const std::string &internalServerConnectResponse,
										  const _InternalProtocol_Device &device) {
	if(internalServerConnectResponse.empty()) {
		return NOT_OK;
	}
	_InternalProtocol_InternalServer internalServerMessage;

	if (not internalServerMessage.ParseFromString(internalServerConnectResponse)) {
		return NOT_OK;
	}

	if (internalServerMessage.has_deviceconnectresponse()) {
		const auto& deviceConnectResponse = internalServerMessage.deviceconnectresponse();
		if(google::protobuf::util::MessageDifferencer::Equals(deviceConnectResponse.device(),
															  device)) {
			return DEVICE_INCORRECT;
		}
		return OK;
	} else {
		return NOT_OK;
	}
}

}