#include <EnumMappers.hpp>

#include <pb_decode.h>
#include <InternalProtocol.pb.h>
#include <ic_error_codes.h>
#include <general_error_codes.h>

#include <vector>
#include <Arduino.h>

namespace helpers {

int helpers::EnumMappers::ConnectResponseToInternalClientCode(const std::string &internalServerConnectResponse) {
	_InternalProtocol_InternalServer internalServerMessage = InternalProtocol_InternalServer_init_zero;

	std::vector<uint8_t> messageVector(internalServerConnectResponse.begin(), internalServerConnectResponse.end());
	uint8_t *buffer = &messageVector[0];

	pb_istream_t stream = pb_istream_from_buffer(buffer, internalServerConnectResponse.size());

	if(!pb_decode(&stream, InternalProtocol_InternalServer_fields, &internalServerMessage)){
		digitalWrite(LED_D0, HIGH); // Debug
	}

	switch(internalServerMessage.MessageType.deviceConnectResponse.responseType) {
		case InternalProtocol_DeviceConnectResponse_ResponseType_OK:
			return OK;
		case InternalProtocol_DeviceConnectResponse_ResponseType_ALREADY_CONNECTED:
			return DEVICE_ALREADY_CONNECTED;
		case InternalProtocol_DeviceConnectResponse_ResponseType_MODULE_NOT_SUPPORTED:
			return MODULE_NOT_SUPPORTED;
		case InternalProtocol_DeviceConnectResponse_ResponseType_DEVICE_NOT_SUPPORTED:
			return DEVICE_TYPE_NOT_SUPPORTED;
		case InternalProtocol_DeviceConnectResponse_ResponseType_HIGHER_PRIORITY_ALREADY_CONNECTED:
			return HIGHER_PRIORITY_ALREADY_CONNECTED;
		default:
			return NOT_OK;
	}
}

}