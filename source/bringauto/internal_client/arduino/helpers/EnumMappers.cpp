#include <EnumMappers.hpp>

#include <pb_decode.h>
#include <InternalProtocol.pb.h>
#include <ic_error_codes.h>
#include <general_error_codes.h>

namespace bringauto::internal_client::arduino::helpers {

int EnumMappers::ConnectResponseToInternalClientCode(const String &internalServerConnectResponse) {
	if(internalServerConnectResponse.length() == 0){
		return NOT_OK;
	}

	InternalProtocol_InternalServer internalServerMessage = InternalProtocol_InternalServer_init_zero;

	pb_istream_t stream = pb_istream_from_buffer((const unsigned char*)internalServerConnectResponse.c_str(), internalServerConnectResponse.length());

	if(!pb_decode(&stream, InternalProtocol_InternalServer_fields, &internalServerMessage)){
		return NOT_OK;
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