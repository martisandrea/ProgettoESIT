#ifndef ERRORS_H
#define ERRORS_H

#include "MQTT.h"

void lwMQTTErr(lwmqtt_err_t type);
void lwMQTTErrConnection(lwmqtt_return_code_t type);

#endif // ERRORS_H
