#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEServer.h>
#include <ArduinoJson.h>

BLEScan* pBLEScan;
BLEServer* pServer;
BLECharacteristic* pCharacteristic;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
       // Serial.printf("BLE Device found: %s, RSSI=%d\n", advertisedDevice.getAddress().toString().c_str(), advertisedDevice.getRSSI());

        // Create JSON
        StaticJsonDocument<100> doc; // Adjust size as needed

        doc["macAddress"] = advertisedDevice.getAddress().toString().c_str();
        doc["rssi"] = advertisedDevice.getRSSI();

        char jsonBuffer[100];
        serializeJson(doc, jsonBuffer);

        // Send JSON data via Bluetooth
        pCharacteristic->setValue(jsonBuffer);
        pCharacteristic->notify();
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Device connected!");
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Device disconnected!");
    }
};

void setup() {
    Serial.begin(115200);
    BLEDevice::init("ESP32");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(BLEUUID((uint16_t)0x1101)); // Use the default UART service UUID (0x1101)
    pCharacteristic = pService->createCharacteristic(
                        BLEUUID((uint16_t)0x2101), // Use the default UART characteristic UUID (0x2101)
                        BLECharacteristic::PROPERTY_NOTIFY
                    );

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();

    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
}

void loop() {
    BLEScanResults* pResults = pBLEScan->start(5, false); // Scan for 5 seconds

    // Handle results
    if (pResults) {
        for (int i = 0; i < pResults->getCount(); i++) {
            BLEAdvertisedDevice device = pResults->getDevice(i);
            
            Serial.printf("Device found: %s, RSSI=%d\n", device.getAddress().toString().c_str(), device.getRSSI());
            
            

            // Create JSON
            StaticJsonDocument<100> doc; // Adjust size as needed

            doc["macAddress"] = device.getAddress().toString().c_str();
            doc["rssi"] = device.getRSSI();

            char jsonBuffer[100];
            serializeJson(doc, jsonBuffer);

            // Send JSON data via Bluetooth
            pCharacteristic->setValue(jsonBuffer);
            pCharacteristic->notify();
        }
        Serial.printf("*******************************************************************************************************************");
    }

    pBLEScan->clearResults(); // Delete results from BLEScan buffer to release memory
    delay(5000); // Delay before next scan
}
