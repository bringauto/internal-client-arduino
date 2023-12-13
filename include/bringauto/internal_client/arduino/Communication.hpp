#pragma once

#include <Context.hpp>

namespace bringauto::internal_client::arduino {

class Communication {
public:
	/// @brief timeout for TCP communication
	static const int TIMEOUT_IN_SECONDS = 3;

	/**
	 * @brief Creates Internal Client Connect message and sends it to server
	 *
	 * @param context Current context
	 *
	 * @return OK if message was sent
	 * @return NOT_OK otherwise
	 */
	static int sendConnectMessage(Context *context);

	/**
	 * @brief Read connect response from server, waits a maximum of 1 minute for a response 
	 *
	 * @param context
	 *
	 * @return OK, if connect response is OK
	 * @return Error code according to ic_error_codes relevant to Connect response types
	 * @return NOT_OK if an error occurred
	 * @return DEVICE_INCORRECT if connect response had different device set than current device
	 */
	static int readConnectResponse(Context *context);

	/**
	 * @brief Attempts to reconnect to the server. The reconnect is successful only if all steps are successful
	 * 1. Restart socket and connect it
	 * 2. Send Connect message and read Connect Response
	 * 3. Send status
	 * 4. Read size of the next message. If size > 0 -> connection is restored and client is able to communicate.
	 *
	 * The size of the next message is saved to nextMessageSize pointer
	 *
	 * @param context current context
	 * @param statusMessage buffer containing status that will be sent
	 * @param nextMessageSize pointer where the size of next message will be saved
	 * @param timeout timeout in seconds how long will function wait for answer from server
	 *
	 * @return OK if the reconnect sequence was successful
	 * @return UNABLE_TO_CONNECT if there was a problem with connecting
	 * @return NOT_OK if an error occurred
	 */
	static int startReconnectSequence(Context *context, struct buffer statusMessage, uint32_t *nextMessageSize, unsigned timeout);
};

}
