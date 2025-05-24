**Serial Terminal Interface Design**

**1. General Principles:**

*   **Case Insensitive:** Commands and parameters (where appropriate, e.g., "true"/"false") should be case insensitive to improve usability.
*   **Whitespace:** Commands and arguments are separated by single spaces. Multiple spaces should be treated as one. SSID or passwords with spaces might need special handling (e.g., quoting, though this adds complexity; for now, assume SSIDs/passwords don't contain spaces or require users to handle that if their OS allows it).
*   **Responses:**
    *   Success: `OK: <details>` or simply `OK`
    *   Error: `ERROR: <description of error>`
    *   Getters: `OK: <parameter_name>=<value>`
*   **Persistence:** `set` commands will modify the runtime configuration. The `save_config` command must be used to persist these changes to NVS (flash). A reboot without saving will revert to the last saved configuration.

**2. Command Definitions:**

Here's a list of commands with their syntax and expected behavior:

*   **`help`**
    *   **Syntax:** `help`
    *   **Behavior:** Displays all available commands, their syntax, and a brief description.
    *   **Response:**
        ```
        OK: Available commands:
        help
        get wifi_ssid
        set wifi_ssid <ssid>
        get wifi_password
        set wifi_password <password>
        get bl_pattern
        set bl_pattern <pattern_name_or_index>
        get bl_color_phase
        set bl_color_phase <0-65535>
        get bl_intensity
        set bl_intensity <0-255>
        get bl_pulse_bpm
        set bl_pulse_bpm <rate>
        get bl_breath_per_min
        set bl_breath_per_min <rate>
        get bl_rainbow_sec
        set bl_rainbow_sec <seconds>
        get clk_twelve_hour
        set clk_twelve_hour <true|false|1|0>
        get clk_time_zone_offset
        set clk_time_zone_offset <seconds_from_utc>
        get clk_blank_hours_zero
        set clk_blank_hours_zero <true|false|1|0>
        get clk_selected_graphic
        set clk_selected_graphic <index>
        save_config
        reboot
        get_config
        get_build_info
        ```

*   **WiFi Credentials:**
    *   **`get wifi_ssid`**
        *   Syntax: `get wifi_ssid`
        *   Behavior: Retrieves the current WiFi SSID from `StoredConfig`.
        *   Response: `OK: wifi_ssid=<current_ssid>` or `OK: wifi_ssid=` if not set.
    *   **`set wifi_ssid <ssid>`**
        *   Syntax: `set wifi_ssid <new_ssid>` (e.g., `set wifi_ssid MyNetwork`)
        *   Behavior: Sets the WiFi SSID in `StoredConfig`. SSID max length is 31 chars.
        *   Response: `OK: wifi_ssid set to <new_ssid>` or `ERROR: Invalid SSID (e.g., too long)`.
    *   **`get wifi_password`**
        *   Syntax: `get wifi_password`
        *   Behavior: Retrieves the current WiFi password from `StoredConfig`.
        *   Response: `OK: wifi_password=<current_password>` or `OK: wifi_password=` if not set.
    *   **`set wifi_password <password>`**
        *   Syntax: `set wifi_password <new_password>` (e.g., `set wifi_password mySecretPa$$`)
        *   Behavior: Sets the WiFi password in `StoredConfig`. Password max length is 31 chars.
        *   Response: `OK: wifi_password set` or `ERROR: Invalid password (e.g., too long)`.

*   **Backlight Options (`bl_` prefix):**
    *   **`get bl_pattern`**
        *   Syntax: `get bl_pattern`
        *   Behavior: Retrieves the current backlight pattern name/index.
        *   Response: `OK: bl_pattern=<current_pattern_name_or_index>`
    *   **`set bl_pattern <pattern_name_or_index>`**
        *   Syntax: `set bl_pattern <pattern_name_or_index>`
        *   Behavior: Sets the backlight pattern.
        *   Response: `OK: bl_pattern set to <pattern_name_or_index>` or `ERROR: Invalid pattern name/index`.
    *   **`get bl_color_phase`**
        *   Syntax: `get bl_color_phase`
        *   Behavior: Retrieves the current backlight color phase.
        *   Response: `OK: bl_color_phase=<0-65535>`
    *   **`set bl_color_phase <value>`**
        *   Syntax: `set bl_color_phase <0-65535>`
        *   Behavior: Sets the backlight color phase.
        *   Response: `OK: bl_color_phase set to <value>` or `ERROR: Invalid value (must be 0-65535)`.
    *   **`get bl_intensity`**
        *   Syntax: `get bl_intensity`
        *   Behavior: Retrieves the current backlight intensity.
        *   Response: `OK: bl_intensity=<0-255>`
    *   **`set bl_intensity <value>`**
        *   Syntax: `set bl_intensity <0-255>`
        *   Behavior: Sets the backlight intensity.
        *   Response: `OK: bl_intensity set to <value>` or `ERROR: Invalid value (must be 0-255)`.
    *   **`get bl_pulse_bpm`**
        *   Syntax: `get bl_pulse_bpm`
        *   Behavior: Retrieves the pulse BPM for backlights.
        *   Response: `OK: bl_pulse_bpm=<rate>`
    *   **`set bl_pulse_bpm <rate>`**
        *   Syntax: `set bl_pulse_bpm <uint8_t_value>`
        *   Behavior: Sets the pulse BPM.
        *   Response: `OK: bl_pulse_bpm set to <rate>` or `ERROR: Invalid value`.
    *   **`get bl_breath_per_min`**
        *   Syntax: `get bl_breath_per_min`
        *   Behavior: Retrieves the breaths per minute for backlights.
        *   Response: `OK: bl_breath_per_min=<rate>`
    *   **`set bl_breath_per_min <rate>`**
        *   Syntax: `set bl_breath_per_min <uint8_t_value>`
        *   Behavior: Sets the breaths per minute.
        *   Response: `OK: bl_breath_per_min set to <rate>` or `ERROR: Invalid value`.
    *   **`get bl_rainbow_sec`**
        *   Syntax: `get bl_rainbow_sec`
        *   Behavior: Retrieves the duration for a full rainbow cycle in seconds.
        *   Response: `OK: bl_rainbow_sec=<seconds>`
    *   **`set bl_rainbow_sec <seconds>`**
        *   Syntax: `set bl_rainbow_sec <float_value>`
        *   Behavior: Sets the rainbow cycle duration.
        *   Response: `OK: bl_rainbow_sec set to <seconds>` or `ERROR: Invalid value`.

*   **Clock Options (`clk_` prefix):**
    *   **`get clk_twelve_hour`**
        *   Syntax: `get clk_twelve_hour`
        *   Behavior: Retrieves whether 12-hour format is enabled.
        *   Response: `OK: clk_twelve_hour=<true|false>`
    *   **`set clk_twelve_hour <true|false|1|0>`**
        *   Syntax: `set clk_twelve_hour <value>`
        *   Behavior: Sets 12-hour format. Accepts `true`, `false`, `1`, `0`.
        *   Response: `OK: clk_twelve_hour set to <true|false>` or `ERROR: Invalid value (must be true, false, 1, or 0)`.
    *   **`get clk_time_zone_offset`**
        *   Syntax: `get clk_time_zone_offset`
        *   Behavior: Retrieves the time zone offset in seconds from UTC.
        *   Response: `OK: clk_time_zone_offset=<seconds>`
    *   **`set clk_time_zone_offset <seconds_from_utc>`**
        *   Syntax: `set clk_time_zone_offset <integer_value>`
        *   Behavior: Sets the time zone offset.
        *   Response: `OK: clk_time_zone_offset set to <seconds>` or `ERROR: Invalid value`.
    *   **`get clk_blank_hours_zero`**
        *   Syntax: `get clk_blank_hours_zero`
        *   Behavior: Retrieves whether blanking leading zero for hours is enabled.
        *   Response: `OK: clk_blank_hours_zero=<true|false>`
    *   **`set clk_blank_hours_zero <true|false|1|0>`**
        *   Syntax: `set clk_blank_hours_zero <value>`
        *   Behavior: Sets blanking leading zero for hours. Accepts `true`, `false`, `1`, `0`.
        *   Response: `OK: clk_blank_hours_zero set to <true|false>` or `ERROR: Invalid value (must be true, false, 1, or 0)`.
    *   **`get clk_selected_graphic`**
        *   Syntax: `get clk_selected_graphic`
        *   Behavior: Retrieves the index of the selected clock graphic/face.
        *   Response: `OK: clk_selected_graphic=<index>`
    *   **`set clk_selected_graphic <index>`**
        *   Syntax: `set clk_selected_graphic <int8_t_value>`
        *   Behavior: Sets the selected clock graphic. Needs to validate against `tfts.NumberOfClockFaces`.
        *   Response: `OK: clk_selected_graphic set to <index>` or `ERROR: Invalid index`.

*   **System Commands:**
    *   **`save_config`**
        *   Syntax: `save_config`
        *   Behavior: Saves all current runtime settings from `StoredConfig` to NVS (flash).
        *   Response: `OK: Configuration saved` or `ERROR: Failed to save configuration`.
    *   **`reboot`**
        *   Syntax: `reboot`
        *   Behavior: Reboots the ESP32 device.
        *   Response: `OK: Rebooting...`
    *   **`get_config`**
        *   Syntax: `get_config`
        *   Behavior: Displays all current runtime configuration values.
        *   Response: (Multiline, similar to help, listing each param and value)
            ```
            OK: Current configuration:
            wifi_ssid=<value>
            ...
            clk_selected_graphic=<value>
            ```
    *   **`get_build_info`**
        *   Syntax: `get_build_info`
        *   Behavior: Displays firmware version and build date/time.
        *   Response: `OK: Firmware Version: <version>, Build Date: <date> <time>`

**3. Data Type Handling for `set` commands:**

*   **Strings:** `wifi_ssid`, `wifi_password`. Max length 31 chars.
*   **Integers:** `bl_color_phase` (uint16_t), `bl_intensity` (uint8_t), `bl_pulse_bpm` (uint8_t), `bl_breath_per_min` (uint8_t), `clk_time_zone_offset` (time_t, likely int32_t), `clk_selected_graphic` (int8_t). Parsed and range checked.
*   **Floats:** `bl_rainbow_sec` (float). Parsed.
*   **Booleans:** `clk_twelve_hour`, `clk_blank_hours_zero`. Accept `true`, `false`, `1`, `0`.
*   **Enums/Names:** `bl_pattern`. Needs mapping from string/index to internal enum.

This document outlines the design for the serial terminal interface.
