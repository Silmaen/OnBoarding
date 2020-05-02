/**
 * \author argawaen
 * \date 16/04/2020
 *
 */

#include "commandParser.h"
#include "allManager.h"

template<> ob::core::commandParser ob::baseManager<ob::core::commandParser>::instance{ob::core::commandParser()};

namespace ob::core {

    void commandParser::treatCommand(const String &cmd) {
        String basecmd(cmd);
        basecmd.trim();
        String pcmd = basecmd.substring(0u, basecmd.indexOf(" "));
        pcmd.trim();
        String parameters = basecmd.substring(basecmd.indexOf(" "));
        parameters.trim();
        if (pcmd == F("uptime")) {
            communicator.send(uptime());
        } else if (pcmd == F("time")) {
            communicator.send(clock.getTimeStr());
        } else if (pcmd == F("settime")) {
            if (setTime(parameters))
                communicator.send(clock.getTimeStr());
        } else if (pcmd == F("setdate")) {
            if (setDate(parameters))
                communicator.send(clock.getTimeStr());
        } else if (pcmd == F("status")) {
            communicator.send(status.getStatusName());
        } else {
            communicator.send(F("Command not found."));
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
            communicator.send(F("Error: bad time format, must be hh:mm:ss."));
            return false;
        }
        int hours = parameter.substring(0u, first).toInt();
        int minutes = parameter.substring(first + 1, last).toInt();
        int seconds = parameter.substring(last + 1).toInt();
        clock.setTime(hours, minutes, seconds);
        return true;
    }

    bool commandParser::setDate(const String &parameter) {
        int first = parameter.indexOf("-");
        int last = parameter.lastIndexOf("-");
        if (first == last) {
            communicator.send(F("Error: bad date format, must be yyyy-mm-dd."));
            return false;
        }
        int year = parameter.substring(0u, first).toInt();
        int month = parameter.substring(first + 1, last).toInt();
        int day = parameter.substring(last + 1).toInt();
        clock.setDate(year, month, day);
        return true;
    }
}
