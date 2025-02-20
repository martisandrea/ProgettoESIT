#include "errors.h"
#include <Arduino.h>

void lwMQTTErr(lwmqtt_err_t type) {
    if (type == LWMQTT_SUCCESS)
        Serial.print("Success");
    else if (type == LWMQTT_BUFFER_TOO_SHORT)
        Serial.print("Buffer too short");
    else if (type == LWMQTT_VARNUM_OVERFLOW)
        Serial.print("Varnum overflow");
    else if (type == LWMQTT_NETWORK_FAILED_CONNECT)
        Serial.print("Network failed connect");
    else if (type == LWMQTT_NETWORK_TIMEOUT)
        Serial.print("Network timeout");
    else if (type == LWMQTT_NETWORK_FAILED_READ)
        Serial.print("Network failed read");
    else if (type == LWMQTT_NETWORK_FAILED_WRITE)
        Serial.print("Network failed write");
    else if (type == LWMQTT_REMAINING_LENGTH_OVERFLOW)
        Serial.print("Remaining length overflow");
    else if (type == LWMQTT_REMAINING_LENGTH_MISMATCH)
        Serial.print("Remaining length mismatch");
    else if (type == LWMQTT_MISSING_OR_WRONG_PACKET)
        Serial.print("Missing or wrong packet");
    else if (type == LWMQTT_CONNECTION_DENIED)
        Serial.print("Connection denied");
    else if (type == LWMQTT_FAILED_SUBSCRIPTION)
        Serial.print("Failed subscription");
    else if (type == LWMQTT_SUBACK_ARRAY_OVERFLOW)
        Serial.print("Suback array overflow");
    else if (type == LWMQTT_PONG_TIMEOUT)
        Serial.print("Pong timeout");
}

void lwMQTTErrConnection(lwmqtt_return_code_t type) {
    if (type == LWMQTT_CONNECTION_ACCEPTED)
        Serial.print("Connection Accepted");
    else if (type == LWMQTT_UNACCEPTABLE_PROTOCOL)
        Serial.print("Unacceptable Protocol");
    else if (type == LWMQTT_IDENTIFIER_REJECTED)
        Serial.print("Identifier Rejected");
    else if (type == LWMQTT_SERVER_UNAVAILABLE)
        Serial.print("Server Unavailable");
    else if (type == LWMQTT_BAD_USERNAME_OR_PASSWORD)
        Serial.print("Bad UserName/Password");
    else if (type == LWMQTT_NOT_AUTHORIZED)
        Serial.print("Not Authorized");
    else if (type == LWMQTT_UNKNOWN_RETURN_CODE)
        Serial.print("Unknown Return Code");
}
