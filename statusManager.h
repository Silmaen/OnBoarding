/**
 * \author argawaen
 * \date 08/04/2020
 */
#pragma once
#include "baseManager.h"

/**
 * \brief namespace for core operations
 */
namespace ob::core {
        /**
         * \brief error codes
         */
        enum class statusCode {
            Running,         ///<  no error... Solid light
            ClockError,      ///<  the clock module is in error
            NonBlockingError,///<  not a fatal error: slow blinking light
            BlockingError,   ///<  Fatal Error (blocking): fast blinking light
        };

        /**
         * \brief manager for error and blocking system
         */
        class statusManager : public baseManager<statusManager> {
            friend class baseManager<statusManager>;

        public:
            /**
             * \brief generate error and block execution
             * \param[in] code the error code
             */
            void fallInError(statusCode code);

            /**
             * \brief to be called every loop
             */
            void frame() override;

            /**
             * \brief return the current execution status
             * \return the execution status
             */
            [[nodiscard]] statusCode getStatus() const { return code; }

            /**
             * \brief return the current status as string
             * \return a string of the current status
             */
            [[nodiscard]] String getStatusName() const;
        private:
            /**
             * \brief base constructor
             */
            statusManager() :  loopStart{0}, code{statusCode::Running}  {
                // setup lighting
                pinMode(LED_BUILTIN, OUTPUT);
                digitalWrite(LED_BUILTIN, HIGH); // setTime builtin led to light: no error
                managerName=F("StatusManager");
            }

            /**
             * \brief destructor
             */
            ~statusManager() = default;

            /**
             * \brief private copy constructor to avoid copy
             */
            statusManager(const statusManager &) = default;

            /**
             * \brief make the blinking of the builtin LED for standard blocking error
             */
            static void slowBlinkingFrame(uint64_t time);

            /**
             * \brief make the blinking of the builtin LED for unknwon error
             */
            static void fastBlinkingFrame(uint64_t time);

            uint64_t loopStart; ///< reference time point
            statusCode code; ///< current execution Status
        };
    }
