#include "ESP32_WiFi_Custom_ELMulator.h"

const String deviceName = "OBDII"; 

// Set up hardcoded responses to non-standard PID requests for MIL, DTC, ODO data
const String milResponse = "4101830000";                       // MIL response code indicating 3 current DTC
const String dtcResponse = "43010341\r\n43010123\r\n43010420"; // DTC response returning 3 DTC codes (multiline response)
const uint32_t odoResponse = 1234567;                          // Hardcode an odometer reading of 1234567
const String ethPercentResponse = "620052C6";
const String dpfCloggingResponse = "6218E4C6";

ELMulator ELMulator;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ELMulator...");

    ELMulator.init(deviceName);
    // Register the specific PIDs we are going to handle.
    // ELMulator will respond to SUPPORTED_PIDS request ("0101") with the appropriate value
    // indicating which PIDs it can respond to.
    ELMulator.registerMode01Pid(ODOMETER);
    ELMulator.registerMode01Pid(ENGINE_COOLANT_TEMP);
    ELMulator.registerMode01Pid(ENGINE_RPM);
    ELMulator.registerMode01Pid(MONITOR_STATUS_SINCE_DTC_CLEARED);
    ELMulator.registerMode01Pid(INTAKE_MANIFOLD_ABS_PRESSURE);
    ELMulator.registerMode01Pid(ENGINE_LOAD);
    ELMulator.registerMode01Pid(VEHICLE_SPEED);
    ELMulator.registerMode01Pid(INTAKE_AIR_TEMP);
    ELMulator.registerMode01Pid(THROTTLE_POSITION);
}

void loop()
{
    /**
     * Waits for a new request from an ELM client and does some pre-processing to validate the PID and request.
     * Non-PID requests like AT commands and SUPPORTED_PIDS queries will be handled by ELMulator automatically.
     * PID requests will be verified as supported (registered); unsupported PIDs responded to with NO DATA response.
     * Returns true if the request is for one of the PIDS we have registered above so we can respond to it.
     */
    if (ELMulator.readELMRequest())
    {
        handlePIDRequest(ELMulator.elmRequest);
    }
}

/**
 * After receiving a supported PID(sensor) request, we prepare the response here.
 * Response can come from a hardcoded value, a real hardware sensor, or a mock value.
 * Default response is a mock value if no other handling is provided.
 */
void handlePIDRequest(const String &request)
{
    // Handle special case requests like MIL and DTC checks
    if (ELMulator.isMode03(request)) // Mode 03 request == Returns (hardcoded) list current DTC codes
    {
        if (dtcResponse.length())
        {
            ELMulator.writeResponse(dtcResponse);
            return;
        }
        else
        {
            DEBUG("DTC response is empty.");
            ELMulator.writePidNotSupported();
            return;
        }
    }

    else if (ELMulator.isMode01MIL(request)) // Mode 0101 MIL request == Returns (hardcoded) number of current DTC codes
    {
        if (milResponse.length())
        {
            ELMulator.writeResponse(milResponse);
            return;
        }
        else
        {
            DEBUG("MIL response is empty.");
            ELMulator.writePidNotSupported();
            return;
        }
    }

    else if (ELMulator.isMode22(request)) // Mode 0x22 (extended info) service - Mfg-dependent and not defined in OBDII spec
    {
        
        Serial.print("Mode 22 PID: "); Serial.println(request.substring(2));

        if (request.substring(2).compareTo("52") == 0) // Ethanol percent
        {
            if (ethPercentResponse.length())
            {
                ELMulator.writeResponse(ethPercentResponse);
                return;
            }
            else
            {
                DEBUG("ETH % response is empty.");
                ELMulator.writePidNotSupported();
                return;
            }
        }

        if (request.substring(2).compareTo("18E4") == 0) // DPF Clogging A-R Giulia
        {
            if (dpfCloggingResponse.length())
            {
                ELMulator.writeResponse(dpfCloggingResponse);
                return;
            }
            else
            {
                DEBUG("DPF Clogging response is empty.");
                ELMulator.writePidNotSupported();
                return;
            }
        }
        ELMulator.writePidNotSupported();
        return;
    }
    else // Handle any other supported PID request
    {
        uint8_t pidCode = ELMulator.getPidCode(request); // Extract the specific PID code from the request

        // Example response for 0x05 (Engine Coolant Temp) - returning a mock data value
        if (pidCode == ENGINE_COOLANT_TEMP)
        {
            uint32_t sensorValue = ELMulator.getMockSensorValue();

            /**
             * Response parameters:
             * request - specifies the original PID request being responded to.
             * numberOfBytes - the number of bytes this PID value returns (per OBDII spec), using lookup table responseBytes.
             * sensorValue - the value to return in our response; a mock value in this example.
             */
            ELMulator.writePidResponse(request, responseBytes[pidCode], sensorValue);
            return;
        }

        // Example response for PID 0x0C (Engine RPM) - returns a modified mock sensor value
        else if (pidCode == ENGINE_RPM)
        {
            uint16_t rpmValue = ELMulator.getMockSensorValue() * 100; // Here we multiply the mock value provided, for a more realistic RPM number
            ELMulator.writePidResponse(request, responseBytes[pidCode], rpmValue);
            return;
        }

        // Example response for PID 0xA6 (Odometer) - returns our hardcoded odometer response string
        else if (pidCode == ODOMETER)
        {
            ELMulator.writePidResponse(request, responseBytes[pidCode], odoResponse);
            return;
        }

        // // Example response for PID 0xOD (Vehicle speed) - returns a value from external GPS
        // else if (pidCode == VEHICLE_SPEED)
        // {
        //     uint32_t gpsSpeed = myGPS.speed();
        //     ELMulator.writePidResponse(request, responseBytes[pidCode], gpsSpeed);
        //     return;
        // }

        // Default response for any other supported PID request - returns an unmodified mock sensor value
        else
        {
            ELMulator.writePidResponse(request, responseBytes[pidCode], ELMulator.getMockSensorValue());
            return;
        }
    }
}
