/**
 * \author argawaen
 * \date 08/04/2020
 */
#include "statusManager.h"
#include "allManager.h"

// instantiation of the manager
template<> ob::core::statusManager ob::baseManager<ob::core::statusManager>::instance{ob::core::statusManager()};

namespace ob::core {
    // -----------------------------------------------------------------------------------------------------------------
    void statusManager::fallInError(statusCode status) {
        loopStart = millis();
        code = status;
    }
    // -----------------------------------------------------------------------------------------------------------------
    void statusManager::frame() {
        //baseManager::frame();
        uint64_t loopCurrent = millis() - loopStart;
        switch (code){
            case statusCode::Running:
                digitalWrite(LED_BUILTIN, HIGH);
                return;
            case statusCode::ClockError:
            case statusCode::LoggerError:
            case statusCode::NonBlockingError:
            case statusCode::SDCardError:
                slowBlinkingFrame(loopCurrent);
                break;
            case statusCode::BlockingError:
                while (1) {
                    fastBlinkingFrame(loopCurrent);
                }
                break;
        }
        if (loopCurrent > 4000 ) {
            // reset timer after 4 second
            loopStart = millis();
        }
    }
    // -----------------------------------------------------------------------------------------------------------------
    void statusManager::slowBlinkingFrame(uint64_t time){
        // slow continuous blink 1000ms / 1000ms
        if (time < 1000 || (time > 2000 && time < 3000) )
            digitalWrite(LED_BUILTIN,HIGH);
        else
            digitalWrite(LED_BUILTIN,LOW);
    }
    // -----------------------------------------------------------------------------------------------------------------
    void statusManager::fastBlinkingFrame(uint64_t time){
        // fast continuous blink 500ms / 500ms
        if (time < 500 || (time > 1000 && time < 1500) || (time > 2000 && time < 2500) || (time > 3000 && time < 3500) )
            digitalWrite(LED_BUILTIN,HIGH);
        else
            digitalWrite(LED_BUILTIN,LOW);
    }

    // -----------------------------------------------------------------------------------------------------------------
    String statusManager::getStatusName() const {
        switch (code) {
            case statusCode::Running:
                return F("Running");
            case statusCode::ClockError:
                return F("Clock Error");
            case statusCode::LoggerError:
                return F("Logger Error");
            case statusCode::SDCardError:
                return F("SD Card Error");
            case statusCode::NonBlockingError:
                return F("Non Blocking Error");
            case statusCode::BlockingError:
                return F("Blocking Error");
        }
    }
    // -----------------------------------------------------------------------------------------------------------------
}
