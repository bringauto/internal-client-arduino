#include <Context.hpp>
#include <Communication.hpp>
#include <ProtoSerializer.hpp>
#include <EnumMappers.hpp>

#include <internal_client.h>

int init_connection(void **context, const char *const ipv4_address, unsigned port,
					const struct device_identification device) {
	auto newContext = new bringauto::internal_client::arduino::Context();

	if (newContext->createConnection(ipv4_address, port) != OK) {
		delete newContext;
		return UNABLE_TO_CONNECT;
	}

	*context = newContext;

	if(newContext->setDevice(device) == NOT_OK) {
		return NOT_OK;
	}

	if (bringauto::internal_client::arduino::Communication::sendConnectMessage(newContext) == OK) {
		return bringauto::internal_client::arduino::Communication::readConnectResponse(newContext);
	} else {
		return NOT_OK;
	}
}

int destroy_connection(void **context) {
	if (*context == nullptr) {
		return OK;
	}
	auto currentContext = (bringauto::internal_client::arduino::Context **)context;
	delete *currentContext;
	*currentContext = nullptr;
	return OK;
}

int send_status(void *context, const struct buffer status, unsigned timeout) {
	if (context == nullptr) {
		return CONTEXT_INCORRECT;
	}
	if (status.data == nullptr) {
		return NOT_OK;
	}
	auto currentContext = (bringauto::internal_client::arduino::Context *)context;

	auto internalClientMessage = bringauto::internal_client::arduino::protobuf::ProtoSerializer::createInternalStatus(status, currentContext->getDevice());
	buffer statusMessageBuffer {nullptr, 0};
	bringauto::internal_client::arduino::protobuf::ProtoSerializer::serializeInternalClientMessageToBuffer(internalClientMessage, &statusMessageBuffer);

	int sendMessage_rc;
	sendMessage_rc = currentContext->sendMessage(statusMessageBuffer);

	if(sendMessage_rc == NOT_OK){
		deallocate(&statusMessageBuffer);
		return UNABLE_TO_CONNECT;
	}
	else if(sendMessage_rc <= 0) {
		deallocate(&statusMessageBuffer);
		return NOT_OK;
	}

	String internalServerMessageString {};
	int rc = OK;

	uint32_t commandSize = currentContext->readSizeFromSocket(timeout);
	if (commandSize == 0) {    // Begin reconnect sequence
		rc = bringauto::internal_client::arduino::Communication::startReconnectSequence(currentContext, statusMessageBuffer, &commandSize, timeout);
	}
	if (rc == OK) {
		internalServerMessageString = currentContext->readMessageFromSocket(commandSize, timeout);
	}

	deallocate(&statusMessageBuffer);

	if (rc != OK) {
		return rc;
	}

	String command {};
	if (bringauto::internal_client::arduino::protobuf::ProtoSerializer::checkAndParseCommand(command, internalServerMessageString,
														currentContext->getDevice()) == NOT_OK) {
		return COMMAND_INCORRECT;
	}
	currentContext->saveCommand(command);
	return OK;
}

int get_command(void *context, struct buffer *command) {
	if (context == nullptr) {
		return CONTEXT_INCORRECT;
	}
	auto currentContext = (bringauto::internal_client::arduino::Context *)context;

	auto newCommandSize = currentContext->getCommandSize();
	if (newCommandSize <= 0) {
		return NO_COMMAND_AVAILABLE;
	}

	if (command->size_in_bytes == 0) {
		if (allocate(command, newCommandSize) == NOT_OK) {
			return NOT_OK;
		}
	} else if (command->size_in_bytes > 0 && command->data == nullptr) {
		return NOT_OK;
	} else if (command->size_in_bytes < newCommandSize) {
		if ((command->data = realloc(command->data, newCommandSize)) == nullptr) {
			return NOT_OK;
		}
		command->size_in_bytes = newCommandSize;
	}
	memcpy(command->data, currentContext->getCommandData(), newCommandSize);
	return OK;
}