#include "ESP32_Simple_ELMulator.h"

/**
 * This example demonstrates a very simple implementation of ELMulator.
 * It handles all AT Commands, SUPPORTED_PIDS queries, etc. and rejectss unsupported requests.
 * All Mode 01 PID requests are provided a response of the correct data size with mock data values.
 * Typical use case is for testing custom applications to work with ELM327 devices. 
 */

const String deviceName = "ELMULATOR"; // Bluetooth device name to use (no pin)
ELMulator ELMulator;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ELMulator...");
    ELMulator.init(deviceName, true); // Initialize with support for all PIDs 0x00 to 0x65, providing mock data
    ELMulator.begin();                // ELMulator will run continuously, listening for and respondng to requests
}

void loop() {}
