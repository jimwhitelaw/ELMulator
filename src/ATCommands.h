#ifndef ELMulator_ATCommands_h
#define ELMulator_ATCommands_h

#include <Arduino.h>
#include "definitions.h"
#if USE_WIFI
#include "OBDWiFiComm.h"
#else
#include "OBDSerialComm.h"
#endif

class ATCommands
{

public:
#if USE_WIFI
    ATCommands(OBDWiFiComm *connection);
#else
    ATCommands(OBDSerialComm *connection);
#endif
    ~ATCommands();

    bool process(const String &string);

private:
    // Variables
#if USE_WIFI
    OBDWiFiComm *connection;
#else
    OBDSerialComm *connection;
#endif
    void ATD();

    void ATZ();

    void ATI();

    void ATEx(String &x);

    void ATMx(String &x);

    void ATLx(String &x);

    void ATSx(String &x);
    void ATSHx(String &x);
    void ATSPx(String &x);
    
    void ATHx(String &x);

    void ATATx(String &cmd);

    void ATPC();

    void ATDPN();

    void ATDESC();

    void ATRV();

    void processCommand(const String &command);

    bool isATCommand(const String &command);
};

#endif
