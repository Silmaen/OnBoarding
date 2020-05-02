/**
 * \author argawaen
 * \date 27/04/2020
 *
 */
#pragma once

#include "globalConfig.h"

namespace ob::comm {
    /**
     * \brief to be called at setup time
     */
    void setup();

    /**
     * \brief
     */
    void frame();

    /**
     * \brief send a message to the communicators
     * \param msg the message to send
     */
    void send(const String &msg);

    void send(const char *msg);
}
