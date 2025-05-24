#ifndef SERIAL_COMMAND_HANDLER_H
#define SERIAL_COMMAND_HANDLER_H

#include <Arduino.h> // Required for String
#include "Backlights.h" // For Backlight::BacklightPattern_t
#include "Clock.h"      // For Clock object/methods (potentially)
#include "TFTs.h"       // For TFTs object/methods (potentially, e.g. NumberOfClockFaces)

// Processes a given command string.
void processSerialCommand(String command);

// Checks for available serial input and processes it.
void handleSerialInput();

// Placeholder/handler functions
void handleHelp();
void handleGetBuildInfo();
void handleGetConfig();
void handleGetWifiSsid();
void handleSetWifiSsid(String ssid);
void handleGetWifiPassword();
void handleSetWifiPassword(String password);
void handleGetBacklightPattern();
void handleSetBacklightPattern(String patternStr);
void handleGetBacklightColorPhase();
void handleSetBacklightColorPhase(String valueStr);
void handleGetBacklightIntensity();
void handleSetBacklightIntensity(String valueStr);
void handleGetBacklightPulseBpm();
void handleSetBacklightPulseBpm(String valueStr);
void handleGetBacklightBreathBpm();
void handleSetBacklightBreathBpm(String valueStr);
void handleGetBacklightRainbowSec();
void handleSetBacklightRainbowSec(String valueStr);
void handleGetClockTwelveHour();
void handleSetClockTwelveHour(String valueStr);
void handleGetClockTimeZoneOffset();
void handleSetClockTimeZoneOffset(String valueStr);
void handleGetClockBlankHoursZero();
void handleSetClockBlankHoursZero(String valueStr);
void handleGetClockSelectedGraphic();
void handleSetClockSelectedGraphic(String valueStr);
void handleSaveConfig();
void handleReboot();

#endif // SERIAL_COMMAND_HANDLER_H
