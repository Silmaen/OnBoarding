/**
 * \author argawaen
 * \date 16/04/2020
 *
 */

#include "commandParser.h"
#include "commManager.h"
#include "systemClock.h"

template<> ob::core::commandParser ob::baseManager<ob::core::commandParser>::instance{ob::core::commandParser()};

namespace ob::core {

    comm::commManager &communication = comm::commManager::get();
    systemClock &clock = systemClock::get();


    void commandParser::treatCommand(const String &cmd) {
        String basecmd(cmd);
        basecmd.trim();
        String pcmd = basecmd.substring(0u, basecmd.indexOf(" "));
        pcmd.trim();
        String parameters = basecmd.substring(basecmd.indexOf(" "));
        parameters.trim();
        if (pcmd == "uptime") {
            communication.send(uptime() + "\n");
        } else if (pcmd == "time") {
            communication.send(clock.getTimeStr() + "\n");
        } else if (pcmd == "settime") {
            if (setTime(parameters))
                communication.send(clock.getTimeStr() + "\n");
        } else {
            communication.send("Command not found.\n");
        }
    }


    String commandParser::uptime() {
        float seconds = millis() / 1000.0;
        if (seconds < 60.0)
            return String(seconds);
        int minutes = seconds / 60;
        seconds -= minutes * 60;
        if (minutes < 60)
            return String(minutes) + ":" + String(seconds);
        int hours = minutes / 60;
        minutes -= hours * 60;
        if (hours < 24)
            return String(hours) + ":" + String(minutes) + ":" + String(seconds);
        int days = hours / 24;
        hours -= days * 24;
        return String(days) + "d " + String(hours) + ":" + String(minutes) + ":" + String(seconds);
    }

    bool commandParser::setTime(const String &parameter) {
        int first = parameter.indexOf(":");
        int last = parameter.lastIndexOf(":");
        if (first == last) {
            communication.send(F("Error: bad time format, must be hh:mm:ss.\n"));
            return false;
        }
        int hours = parameter.substring(0u, first).toInt();
        int minutes = parameter.substring(first + 1, last).toInt();
        int seconds = parameter.substring(last + 1).toInt();
        clock.setTime(hours, minutes, seconds);
        return true;
    }

}
