#include <Communication.hpp>
#include <EnumMappers.hpp>
#include <ProtoSerializer.hpp>

#include <ic_error_codes.h>

namespace bringauto::internal_client::arduino {

int Communication::sendConnectMessage(Context *context) {
	auto internalConnect = protobuf::ProtoSerializer::createInternalConnect(context->getDevice());
	buffer connectMessageBuffer {nullptr, 0};
	protobuf::ProtoSerializer::serializeInternalClientMessageToBuffer(internalConnect, &connectMessageBuffer);
	if (context->sendMessage(connectMessageBuffer) <= 0) {
		deallocate(&connectMessageBuffer);
		return NOT_OK;
	}
	deallocate(&connectMessageBuffer);
	return OK;
}

int Communication::readConnectResponse(Context *context) {
	auto size = context->readSizeFromSocket(Communication::TIMEOUT_IN_SECONDS);
	String connectResponse = context->readMessageFromSocket(size, Communication::TIMEOUT_IN_SECONDS);
	if (int retCode = protobuf::ProtoSerializer::checkConnectResponse(connectResponse, context->getDevice()) != OK) {
		return retCode;
	}
	return helpers::EnumMappers::ConnectResponseToInternalClientCode(connectResponse);
}

int Communication::startReconnectSequence(Context *context, struct buffer statusMessage, uint32_t *nextMessageSize, unsigned timeout) {
	if(context->reconnect() != OK) {
		return UNABLE_TO_CONNECT;
	}
	if (Communication::sendConnectMessage(context) != OK) {
		return UNABLE_TO_CONNECT;
	}
	if (Communication::readConnectResponse(context) == OK) {
		return UNABLE_TO_CONNECT;
	}
	if(context->sendMessage(statusMessage) <= 0) {
		return NOT_OK;
	}
	*nextMessageSize = context->readSizeFromSocket(timeout);
	return *nextMessageSize == 0 ? NOT_OK : OK;

}

}
