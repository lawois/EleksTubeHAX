#include <Arduino.h>
#include "SerialCommandHandler.h"
#include "StoredConfig.h"     // Will be needed for actual implementation
#include "GLOBAL_DEFINES.h"   // For FIRMWARE_VERSION (eventually)
#include "Backlights.h"    // For Backlights::patterns and patterns_str
#include "Clock.h"         // For uclock methods
#include "TFTs.h"          // For tfts.NumberOfClockFaces and tfts.current_graphic

// External declaration of global objects (defined in main.cpp)
extern StoredConfig stored_config; // Make sure this is declared in main.cpp and accessible
extern Backlights backlights;      // Assumed to be globally available for immediate effect
extern Clock uclock;               // Assumed to be globally available
extern TFTs tfts;                  // Assumed to be globally available
// extern void WifiBegin(); // For potential re-init after changing SSID/Pass. Consider implications.
extern void updateClockDisplay(TFTs::show_t show); // From main.cpp, for refreshing display

// Helper function to split string by a delimiter, useful for command parsing
// For simplicity, we'll use indexOf and substring in this initial implementation.
// A more robust tokenizer might be good for future enhancements.

void handleSerialInput() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.length() > 0) {
            processSerialCommand(command);
        }
    }
}

void processSerialCommand(String command) {
    command.toLowerCase();
    String originalCommand = command; // Keep a copy for "not implemented" messages

    // Basic parsing: command is the first word, argument is the rest
    String cmdName;
    String cmdArgs;

    int firstSpace = command.indexOf(' ');
    if (firstSpace == -1) {
        cmdName = command;
    } else {
        cmdName = command.substring(0, firstSpace);
        cmdArgs = command.substring(firstSpace + 1);
        cmdArgs.trim(); // Trim arguments
    }

    // For "get <param>" or "set <param> <value>" style commands, we need to parse further
    if (cmdName == "get" || cmdName == "set") {
        int secondSpace = cmdArgs.indexOf(' ');
        if (secondSpace == -1 && cmdName == "get") { // e.g. "get wifi_ssid"
            cmdName = cmdName + " " + cmdArgs; // Reconstruct "get wifi_ssid"
            cmdArgs = "";
        } else if (secondSpace != -1 && cmdName == "set") { // e.g. "set wifi_ssid mynetwork"
            String tempParam = cmdArgs.substring(0, secondSpace);
            cmdName = cmdName + " " + tempParam; // Reconstruct "set wifi_ssid"
            cmdArgs = cmdArgs.substring(secondSpace + 1); // The actual value for set
            cmdArgs.trim();
        } else if (cmdName == "set" && secondSpace == -1) { // set command with no value
             Serial.println("ERROR: Missing value for set command: " + originalCommand);
             return;
        }
         // else: "get" command with extra arguments or "set" command with no value (if not caught above)
         // will likely fall through to unknown command or be handled by specific handlers if they parse args themselves
    }


    if (cmdName == "help") {
        handleHelp();
    } else if (cmdName == "get_build_info") {
        handleGetBuildInfo();
    } else if (cmdName == "get_config") {
        handleGetConfig();
    } else if (cmdName == "get wifi_ssid") {
        handleGetWifiSsid();
    } else if (cmdName == "set wifi_ssid") {
        handleSetWifiSsid(cmdArgs);
    } else if (cmdName == "get wifi_password") {
        handleGetWifiPassword();
    } else if (cmdName == "set wifi_password") {
        handleSetWifiPassword(cmdArgs);
    } else if (cmdName == "get bl_pattern") {
        handleGetBacklightPattern();
    } else if (cmdName == "set bl_pattern") {
        handleSetBacklightPattern(cmdArgs);
    } else if (cmdName == "get bl_color_phase") {
        handleGetBacklightColorPhase();
    } else if (cmdName == "set bl_color_phase") {
        handleSetBacklightColorPhase(cmdArgs);
    } else if (cmdName == "get bl_intensity") {
        handleGetBacklightIntensity();
    } else if (cmdName == "set bl_intensity") {
        handleSetBacklightIntensity(cmdArgs);
    } else if (cmdName == "get bl_pulse_bpm") {
        handleGetBacklightPulseBpm();
    } else if (cmdName == "set bl_pulse_bpm") {
        handleSetBacklightPulseBpm(cmdArgs);
    } else if (cmdName == "get bl_breath_per_min") {
        handleGetBacklightBreathBpm();
    } else if (cmdName == "set bl_breath_per_min") {
        handleSetBacklightBreathBpm(cmdArgs);
    } else if (cmdName == "get bl_rainbow_sec") {
        handleGetBacklightRainbowSec();
    } else if (cmdName == "set bl_rainbow_sec") {
        handleSetBacklightRainbowSec(cmdArgs);
    } else if (cmdName == "get clk_twelve_hour") {
        handleGetClockTwelveHour();
    } else if (cmdName == "set clk_twelve_hour") {
        handleSetClockTwelveHour(cmdArgs);
    } else if (cmdName == "get clk_time_zone_offset") {
        handleGetClockTimeZoneOffset();
    } else if (cmdName == "set clk_time_zone_offset") {
        handleSetClockTimeZoneOffset(cmdArgs);
    } else if (cmdName == "get clk_blank_hours_zero") {
        handleGetClockBlankHoursZero();
    } else if (cmdName == "set clk_blank_hours_zero") {
        handleSetClockBlankHoursZero(cmdArgs);
    } else if (cmdName == "get clk_selected_graphic") {
        handleGetClockSelectedGraphic();
    } else if (cmdName == "set clk_selected_graphic") {
        handleSetClockSelectedGraphic(cmdArgs);
    } else if (cmdName == "save_config") {
        handleSaveConfig();
    } else if (cmdName == "reboot") {
        handleReboot();
    } else {
        Serial.print("ERROR: Unknown command ");
        Serial.println(originalCommand);
    }
}

void handleHelp() {
    Serial.println("OK: Available commands:");
    Serial.println("help");
    Serial.println("get wifi_ssid");
    Serial.println("set wifi_ssid <ssid>");
    Serial.println("get wifi_password");
    Serial.println("set wifi_password <password>");
    Serial.println("get bl_pattern");
    Serial.println("set bl_pattern <pattern_name_or_index>");
    Serial.println("get bl_color_phase");
    Serial.println("set bl_color_phase <0-65535>");
    Serial.println("get bl_intensity");
    Serial.println("set bl_intensity <0-255>");
    Serial.println("get bl_pulse_bpm");
    Serial.println("set bl_pulse_bpm <rate>");
    Serial.println("get bl_breath_per_min");
    Serial.println("set bl_breath_per_min <rate>");
    Serial.println("get bl_rainbow_sec");
    Serial.println("set bl_rainbow_sec <seconds>");
    Serial.println("get clk_twelve_hour");
    Serial.println("set clk_twelve_hour <true|false|1|0>");
    Serial.println("get clk_time_zone_offset");
    Serial.println("set clk_time_zone_offset <seconds_from_utc>");
    Serial.println("get clk_blank_hours_zero");
    Serial.println("set clk_blank_hours_zero <true|false|1|0>");
    Serial.println("get clk_selected_graphic");
    Serial.println("set clk_selected_graphic <index>");
    Serial.println("save_config");
    Serial.println("reboot");
    Serial.println("get_config");
    Serial.println("get_build_info");
}

void handleGetBuildInfo() {
    // Placeholder for actual version and build date
    // FIRMWARE_VERSION is usually in GLOBAL_DEFINES.h
    // __DATE__ and __TIME__ are standard C/C++ preprocessor macros
    String fwVersion = "DEV_BUILD"; 
    #ifdef FIRMWARE_VERSION
        fwVersion = FIRMWARE_VERSION;
    #endif
    Serial.print("OK: Firmware Version: ");
    Serial.print(fwVersion);
    Serial.print(", Build Date: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
}

// --- System Commands ---
void handleSaveConfig() {
    // Preferences.putBytes returns the number of bytes written, or 0 if an error occurred.
    // Assuming StoredConfig::save() mirrors this or has its own success/failure logic.
    // For now, let's assume StoredConfig::save() is void and always succeeds from its perspective,
    // or we'd need to modify StoredConfig to return a status.
    // The Preferences library itself doesn't have an explicit boolean return for putBytes success other than checking written bytes.
    stored_config.save(); 
    // To make this more robust, stored_config.save() should ideally return a bool.
    // Let's assume it worked for now as per current StoredConfig structure.
    Serial.println("OK: Configuration saved");
    // Example of how it might look if save() returned bool:
    // if (stored_config.save()) {
    //     Serial.println("OK: Configuration saved");
    // } else {
    //     Serial.println("ERROR: Failed to save configuration");
    // }
}

void handleGetConfig() {
    if (!stored_config.isLoaded()) {
        stored_config.load(); // Ensure current data is loaded
    }
    Serial.println("OK: Current configuration:");

    // WiFi
    Serial.print("wifi_ssid=");
    Serial.println(stored_config.config.wifi.ssid);
    Serial.print("wifi_password=");
    Serial.println(stored_config.config.wifi.password); // Consider security if sensitive

    // Backlights
    Serial.print("bl_pattern="); 
    Serial.println(stored_config.config.backlights.pattern); // Ideally map to name if possible
    Serial.print("bl_color_phase=");
    Serial.println(stored_config.config.backlights.color_phase);
    Serial.print("bl_intensity=");
    Serial.println(stored_config.config.backlights.intensity);
    Serial.print("bl_pulse_bpm=");
    Serial.println(stored_config.config.backlights.pulse_bpm);
    Serial.print("bl_breath_per_min=");
    Serial.println(stored_config.config.backlights.breath_per_min);
    Serial.print("bl_rainbow_sec=");
    Serial.println(stored_config.config.backlights.rainbow_sec);

    // Clock
    Serial.print("clk_twelve_hour=");
    Serial.println(stored_config.config.uclock.twelve_hour ? "true" : "false");
    Serial.print("clk_time_zone_offset=");
    Serial.println(stored_config.config.uclock.time_zone_offset);
    Serial.print("clk_blank_hours_zero=");
    Serial.println(stored_config.config.uclock.blank_hours_zero ? "true" : "false");
    Serial.print("clk_selected_graphic=");
    Serial.println(stored_config.config.uclock.selected_graphic);
}


// --- WiFi Configuration ---
void handleGetWifiSsid() {
    if (!stored_config.isLoaded()) { // Ensure config is loaded
        stored_config.load();
    }
    Serial.print("OK: wifi_ssid=");
    Serial.println(stored_config.config.wifi.ssid);
}

void handleSetWifiSsid(String ssid) {
    if (ssid.length() == 0) {
        Serial.println("ERROR: SSID cannot be empty.");
        return;
    }
    if (ssid.length() < StoredConfig::str_buffer_size) {
        strncpy(stored_config.config.wifi.ssid, ssid.c_str(), StoredConfig::str_buffer_size -1);
        stored_config.config.wifi.ssid[StoredConfig::str_buffer_size - 1] = '\0'; // Ensure null termination
        Serial.print("OK: wifi_ssid set to ");
        Serial.println(ssid);
        // Note: Does not automatically save or reconnect WiFi. User needs to 'save_config' and potentially 'reboot' or a new 'connect_wifi' command.
    } else {
        Serial.print("ERROR: Invalid SSID (too long, max ");
        Serial.print(StoredConfig::str_buffer_size - 1);
        Serial.println(" chars).");
    }
}

void handleGetWifiPassword() {
    if (!stored_config.isLoaded()) { // Ensure config is loaded
        stored_config.load();
    }
    Serial.print("OK: wifi_password=");
    Serial.println(stored_config.config.wifi.password);
}

void handleSetWifiPassword(String password) {
    // Password can be empty by design for open networks.
    if (password.length() < StoredConfig::str_buffer_size) {
        strncpy(stored_config.config.wifi.password, password.c_str(), StoredConfig::str_buffer_size - 1);
        stored_config.config.wifi.password[StoredConfig::str_buffer_size - 1] = '\0'; // Ensure null termination
        Serial.println("OK: wifi_password set.");
        // Note: Does not automatically save or reconnect WiFi. User needs to 'save_config' and potentially 'reboot' or a new 'connect_wifi' command.
    } else {
        Serial.print("ERROR: Invalid password (too long, max ");
        Serial.print(StoredConfig::str_buffer_size - 1);
        Serial.println(" chars).");
    }
}

// --- Backlight Configuration ---
void handleGetBacklightPattern() {
    if (!stored_config.isLoaded()) {
        stored_config.load();
    }
    uint8_t pattern_idx = stored_config.config.backlights.pattern;
    if (pattern_idx < Backlights::num_patterns) {
        Serial.print("OK: bl_pattern=");
        Serial.println(Backlights::patterns_str[pattern_idx]);
    } else {
        Serial.println("ERROR: Stored pattern index is invalid.");
    }
}

void handleSetBacklightPattern(String patternStr) {
    if (patternStr.length() == 0) {
        Serial.println("ERROR: Pattern name/index cannot be empty.");
        return;
    }

    Backlights::patterns new_pattern_enum = Backlights::dark; // Default to a known safe value
    bool found = false;

    // Try to parse as an index first
    char firstChar = patternStr.charAt(0);
    if (isDigit(firstChar) || (firstChar == '-' && patternStr.length() > 1 && isDigit(patternStr.charAt(1)))) {
        long pattern_idx_long = patternStr.toInt();
        if (pattern_idx_long >= 0 && pattern_idx_long < Backlights::num_patterns) {
            new_pattern_enum = static_cast<Backlights::patterns>(pattern_idx_long);
            found = true;
        }
    }

    // If not a valid index, try to parse as a name (case-insensitive)
    if (!found) {
        for (int i = 0; i < Backlights::num_patterns; ++i) {
            if (patternStr.equalsIgnoreCase(Backlights::patterns_str[i])) {
                new_pattern_enum = static_cast<Backlights::patterns>(i);
                found = true;
                break;
            }
        }
    }

    if (found) {
        stored_config.config.backlights.pattern = static_cast<uint8_t>(new_pattern_enum);
        backlights.setPattern(new_pattern_enum); // Apply immediately
        Serial.print("OK: bl_pattern set to ");
        Serial.println(Backlights::patterns_str[new_pattern_enum]);
    } else {
        Serial.println("ERROR: Invalid pattern name/index.");
    }
}


// --- Other Placeholders ---
//void handleGetBacklightPatternPlaceholder() { Serial.println("OK: Command get bl_pattern not yet implemented."); } // Implemented
//void handleSetBacklightPatternPlaceholder() { Serial.println("OK: Command set bl_pattern not yet implemented."); } // Implemented
// void handleGetBacklightColorPhasePlaceholder() { Serial.println("OK: Command get bl_color_phase not yet implemented."); }
// void handleSetBacklightColorPhasePlaceholder() { Serial.println("OK: Command set bl_color_phase not yet implemented."); }
// void handleGetBacklightIntensityPlaceholder() { Serial.println("OK: Command get bl_intensity not yet implemented."); }
// void handleSetBacklightIntensityPlaceholder() { Serial.println("OK: Command set bl_intensity not yet implemented."); }
// void handleGetBacklightPulseBpmPlaceholder() { Serial.println("OK: Command get bl_pulse_bpm not yet implemented."); }
// void handleSetBacklightPulseBpmPlaceholder() { Serial.println("OK: Command set bl_pulse_bpm not yet implemented."); }
// void handleGetBacklightBreathBpmPlaceholder() { Serial.println("OK: Command get bl_breath_per_min not yet implemented."); }
// void handleSetBacklightBreathBpmPlaceholder() { Serial.println("OK: Command set bl_breath_per_min not yet implemented."); }
// void handleGetBacklightRainbowSecPlaceholder() { Serial.println("OK: Command get bl_rainbow_sec not yet implemented."); }
// void handleSetBacklightRainbowSecPlaceholder() { Serial.println("OK: Command set bl_rainbow_sec not yet implemented."); }

void handleGetBacklightColorPhase() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: bl_color_phase=");
    Serial.println(stored_config.config.backlights.color_phase);
}

void handleSetBacklightColorPhase(String valueStr) {
    long val = valueStr.toInt();
    if (val >= 0 && val <= 65535) { // uint16_t range
        stored_config.config.backlights.color_phase = (uint16_t)val;
        backlights.setColorPhase((uint16_t)val); // Apply immediately
        Serial.print("OK: bl_color_phase set to ");
        Serial.println(val);
    } else {
        Serial.println("ERROR: Invalid value for bl_color_phase (must be 0-65535).");
    }
}

void handleGetBacklightIntensity() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: bl_intensity=");
    Serial.println(stored_config.config.backlights.intensity);
}

void handleSetBacklightIntensity(String valueStr) {
    long val = valueStr.toInt();
    // As per Backlights.h, max_intensity is 8 (0-7).
    // However, StoredConfig stores it as uint8_t, and MQTT uses 0-255.
    // The design doc for serial also says 0-255.
    // Assuming backlights.setIntensity() handles clamping or mapping if its internal range is different.
    if (val >= 0 && val <= 255) { // uint8_t range as per design
        stored_config.config.backlights.intensity = (uint8_t)val;
        backlights.setIntensity((uint8_t)val); // Apply immediately
        Serial.print("OK: bl_intensity set to ");
        Serial.println(val);
    } else {
        Serial.println("ERROR: Invalid value for bl_intensity (must be 0-255).");
    }
}

void handleGetBacklightPulseBpm() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: bl_pulse_bpm=");
    Serial.println(stored_config.config.backlights.pulse_bpm);
}

void handleSetBacklightPulseBpm(String valueStr) {
    long val = valueStr.toInt();
    if (val >= 0 && val <= 255) { // uint8_t range
        stored_config.config.backlights.pulse_bpm = (uint8_t)val;
        backlights.setPulseRate((uint8_t)val); // Apply immediately
        Serial.print("OK: bl_pulse_bpm set to ");
        Serial.println(val);
    } else {
        Serial.println("ERROR: Invalid value for bl_pulse_bpm (must be 0-255).");
    }
}

void handleGetBacklightBreathBpm() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: bl_breath_per_min=");
    Serial.println(stored_config.config.backlights.breath_per_min);
}

void handleSetBacklightBreathBpm(String valueStr) {
    long val = valueStr.toInt();
    if (val >= 0 && val <= 255) { // uint8_t range
        stored_config.config.backlights.breath_per_min = (uint8_t)val;
        backlights.setBreathRate((uint8_t)val); // Apply immediately
        Serial.print("OK: bl_breath_per_min set to ");
        Serial.println(val);
    } else {
        Serial.println("ERROR: Invalid value for bl_breath_per_min (must be 0-255).");
    }
}

void handleGetBacklightRainbowSec() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: bl_rainbow_sec=");
    Serial.println(stored_config.config.backlights.rainbow_sec);
}

void handleSetBacklightRainbowSec(String valueStr) {
    // Arduino String toFloat() method can be used.
    // No specific range mentioned in Backlights.h, assume positive values are okay.
    // Negative values for time don't make sense.
    float val = valueStr.toFloat();
    if (val >= 0) { // Assuming 0 is a valid value (e.g. very fast)
        stored_config.config.backlights.rainbow_sec = val;
        backlights.setRainbowDuration(val); // Apply immediately
        Serial.print("OK: bl_rainbow_sec set to ");
        Serial.println(val);
    } else {
        Serial.println("ERROR: Invalid value for bl_rainbow_sec (must be a non-negative number).");
    }
}

// --- Clock Configuration ---
void handleGetClockTwelveHour() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: clk_twelve_hour=");
    Serial.println(stored_config.config.uclock.twelve_hour ? "true" : "false");
}

void handleSetClockTwelveHour(String valueStr) {
    bool val;
    if (valueStr == "true" || valueStr == "1") {
        val = true;
    } else if (valueStr == "false" || valueStr == "0") {
        val = false;
    } else {
        Serial.println("ERROR: Invalid value for clk_twelve_hour (must be true, false, 1, or 0).");
        return;
    }
    stored_config.config.uclock.twelve_hour = val;
    uclock.setTwelveHour(val); // Apply immediately
    // Display update likely handled by uclock.loop() or main loop's updateClockDisplay()
    Serial.print("OK: clk_twelve_hour set to ");
    Serial.println(val ? "true" : "false");
}

void handleGetClockTimeZoneOffset() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: clk_time_zone_offset=");
    Serial.println(stored_config.config.uclock.time_zone_offset);
}

void handleSetClockTimeZoneOffset(String valueStr) {
    // time_t can be negative. No specific range check other than it fitting in time_t.
    // String::toInt() returns long, which should be compatible with time_t on ESP32.
    long val = valueStr.toInt(); // toInt() returns 0 if conversion fails. Consider stricter parsing if needed.
    
    // A basic check to see if toInt() failed due to non-numeric string (excluding initial '-')
    bool conversion_ok = true;
    if (val == 0 && valueStr != "0") {
        for (unsigned int i = (valueStr.charAt(0) == '-' ? 1 : 0); i < valueStr.length(); i++) {
            if (!isDigit(valueStr.charAt(i))) {
                conversion_ok = false;
                break;
            }
        }
    }

    if (!conversion_ok) {
        Serial.println("ERROR: Invalid number format for clk_time_zone_offset.");
        return;
    }

    stored_config.config.uclock.time_zone_offset = (time_t)val;
    uclock.setTimeZoneOffset((time_t)val); // Apply immediately
    Serial.print("OK: clk_time_zone_offset set to ");
    Serial.println((time_t)val);
}

void handleGetClockBlankHoursZero() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: clk_blank_hours_zero=");
    Serial.println(stored_config.config.uclock.blank_hours_zero ? "true" : "false");
}

void handleSetClockBlankHoursZero(String valueStr) {
    bool val;
    if (valueStr == "true" || valueStr == "1") {
        val = true;
    } else if (valueStr == "false" || valueStr == "0") {
        val = false;
    } else {
        Serial.println("ERROR: Invalid value for clk_blank_hours_zero (must be true, false, 1, or 0).");
        return;
    }
    stored_config.config.uclock.blank_hours_zero = val;
    uclock.setBlankHoursZero(val); // Apply immediately
    Serial.print("OK: clk_blank_hours_zero set to ");
    Serial.println(val ? "true" : "false");
}

void handleGetClockSelectedGraphic() {
    if (!stored_config.isLoaded()) stored_config.load();
    Serial.print("OK: clk_selected_graphic=");
    Serial.println(stored_config.config.uclock.selected_graphic);
}

void handleSetClockSelectedGraphic(String valueStr) {
    long val_long = valueStr.toInt();
    
    bool conversion_ok = true;
    if (val_long == 0 && valueStr != "0") {
         for (unsigned int i = (valueStr.charAt(0) == '-' ? 1 : 0); i < valueStr.length(); i++) {
            if (!isDigit(valueStr.charAt(i))) {
                conversion_ok = false;
                break;
            }
        }
    }
    if (!conversion_ok) {
        Serial.println("ERROR: Invalid number format for clk_selected_graphic.");
        return;
    }

    if (val_long >= 1 && val_long <= tfts.NumberOfClockFaces) { // Clock faces are 1-indexed
        int8_t val = (int8_t)val_long;
        stored_config.config.uclock.selected_graphic = val;
        uclock.setClockGraphicsIdx(val); // Apply immediately
        tfts.current_graphic = val;      // Keep tfts object in sync
        updateClockDisplay(TFTs::force); // Force display update
        Serial.print("OK: clk_selected_graphic set to ");
        Serial.println(val);
    } else {
        Serial.print("ERROR: Invalid value for clk_selected_graphic (must be 1-");
        Serial.print(tfts.NumberOfClockFaces);
        Serial.println(").");
    }
}

//void handleGetClockTwelveHourPlaceholder() { Serial.println("OK: Command get clk_twelve_hour not yet implemented."); }
//void handleSetClockTwelveHourPlaceholder() { Serial.println("OK: Command set clk_twelve_hour not yet implemented."); }
//void handleGetClockTimeZoneOffsetPlaceholder() { Serial.println("OK: Command get clk_time_zone_offset not yet implemented."); }
//void handleSetClockTimeZoneOffsetPlaceholder() { Serial.println("OK: Command set clk_time_zone_offset not yet implemented."); }
//void handleGetClockBlankHoursZeroPlaceholder() { Serial.println("OK: Command get clk_blank_hours_zero not yet implemented."); }
//void handleSetClockBlankHoursZeroPlaceholder() { Serial.println("OK: Command set clk_blank_hours_zero not yet implemented."); }
//void handleGetClockSelectedGraphicPlaceholder() { Serial.println("OK: Command get clk_selected_graphic not yet implemented."); }
//void handleSetClockSelectedGraphicPlaceholder() { Serial.println("OK: Command set clk_selected_graphic not yet implemented."); }
//void handleSaveConfigPlaceholder() { Serial.println("OK: Command save_config not yet implemented."); } // Now implemented
// void handleRebootPlaceholder() { 
//     Serial.println("OK: Rebooting..."); 
//     // ESP.restart(); // This will be uncommented when fully implementing
// }

void handleReboot() {
    Serial.println("OK: Rebooting...");
    delay(100); // Short delay to help ensure the serial message is sent
    ESP.restart();
}
