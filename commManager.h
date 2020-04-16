/**
 * \author argawaen
 * \date 14/04/2020
 *
 */
#pragma once
#include "baseManager.h"

namespace ob::comm {

    constexpr uint8_t maxBufferSize = 250;

    /**
     * \brief base class to handle communications
     */
    class commManager : public baseManager<commManager> {
        friend class baseManager<commManager>;

    public:
        /**
         * \brief to be called at setup time
         */
        void setup() override;

        /**
         * \brief to be called every frame
         */
        void frame() override;

        /**
         * \brief send a message through all channels
         * \param message
         */
        void send(const String &message) const;
    private:
        /**
         * \brief base constructor
         */
        commManager(){
            managerName = F("Communication Manager");
        }

        /**
         * \brief destructor
         */
        ~commManager() = default;

        /**
         * \brief private copy constructor to avoid copy
         */
        commManager(const commManager &) = default;

        char buffer[maxBufferSize];///< buffer to store received messages
    };


}
