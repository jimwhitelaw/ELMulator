#include "OBDSerialComm.h"
#include "definitions.h"


OBDSerialComm::OBDSerialComm() {
    
}

OBDSerialComm::~OBDSerialComm() {
    operator delete(serial);
}

void OBDSerialComm::init(const String& deviceName) {
    Serial.println("Starting BT . . .");
    serial = new BluetoothSerial();
    //delay(2000);
    serial->begin(deviceName, false);
    setToDefaults();

}

void OBDSerialComm::writeEnd() {

    // 1 - write carriage return
    writeTo("\r");

    // 2- (optional ) write linefeed
    if (lineFeedEnable) {
        writeTo("\n");
    }

    // 3 - Write prompt
    writeTo(">");

    serial->flush();
};


void OBDSerialComm::writeEndOK() {
    writeTo("OK");
    writeEnd();
}

void OBDSerialComm::writeEndERROR() {
    writeTo("ERROR");
    writeEnd();
}

void OBDSerialComm::writeEndNoData() {
    writeTo("NO DATA");
    writeEnd();
}

void OBDSerialComm::writeEndUnknown() {
    writeTo("?");
    writeEnd();
}

void OBDSerialComm::setToDefaults() {
    setEcho(true);
    setStatus(READY);
    setWhiteSpaces(true);
    setHeaders(false);
    setLineFeeds(true);
    setMemory(false);
    setUseCustomHeader(false);
    setCustomHeader(NULL);
}

void OBDSerialComm::printHeaderIfEnabled() {
    if (headersEnabled) {
        int headerToPrint = 0x7E7; // Default header
        if (useCustomHeader && customHeader != NULL) {
            headerToPrint = customHeader;
        }
        // Format header as 3 hex chars, zero-padded, with space
        char headerStr[5];
        snprintf(headerStr, sizeof(headerStr), "%03X ", headerToPrint);
        serial->print(headerStr);
    }
}

void OBDSerialComm::writeTo(char const *response) {
    printHeaderIfEnabled();
    serial->print(response);
}

void OBDSerialComm::writeTo(uint8_t cChar) {
    printHeaderIfEnabled();
    serial->print(cChar);
}

void OBDSerialComm::writeEndPidTo(char const *response) {
    if (whiteSpacesEnabled) {
        uint8_t len = strlen(response);
        char spacedResponse[len + len / 2 + 1];
        addSpacesToResponse(response, spacedResponse);
        writeTo(spacedResponse);
    } else {
        writeTo(response);
    }
    writeEnd();
}

void OBDSerialComm::readData(String& rxData) {
    serial->flush(); // temp remove this
    rxData = serial->readStringUntil(SERIAL_END_CHAR);
    if (isEchoEnable()) {
        writeTo(rxData.c_str());
    }
    //return rxData;
}

void OBDSerialComm::setBaudRate(uint32_t rate) {
    this->baudRate = rate;
}

long OBDSerialComm::getBaudRate() {
    return this->baudRate;
}

bool OBDSerialComm::isEchoEnable() {
    return this->echoEnable;
}

void OBDSerialComm::setEcho(bool echo) {
    this->echoEnable = echo;
}

void OBDSerialComm::setStatus(STATUS status) {
    this->status = status;
}

void OBDSerialComm::setLineFeeds(bool status) {
    this->lineFeedEnable = status;
}

void OBDSerialComm::setMemory(bool status) {
    this->memoryEnabled = status;
}

void OBDSerialComm::setWhiteSpaces(bool status) {
    this->whiteSpacesEnabled = status;
}

void OBDSerialComm::setHeaders(bool status) {
    this->headersEnabled = status;
}

void OBDSerialComm::addSpacesToResponse(const char *response, char spacedRes[]) {
    uint8_t len = strlen(response);
    int j = 0;
    for (int i = 0; i < len;) {
        *(spacedRes + j++) = *(response + i++);
        *(spacedRes + j++) = *(response + i++);
        if (i < len) {
            *(spacedRes + j++) = 0x20;
        }
    }
    *(spacedRes + j) = '\0';
}
