#include <esphome.h>
#include <bsec.h>
#include <Wire.h>
#include <EEPROM.h>
const uint8_t bsec_config_iaq[] = {
    #include "config/generic_33v_3s_28d/bsec_iaq.txt"
};

#define STATE_SAVE_PERIOD   UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

static const char *TAG = "bsec";

class BSECSensor : public Component {
    private:
        Bsec iaqSensor;
        bsec_library_return_t bsecStatus = BSEC_OK;
        int8_t bme680Status = BME680_OK;
        uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
        uint16_t stateUpdateCounter = 0;
        uint8_t sensorPushNum = 0;

        bool checkIaqSensorStatus(void) {
            if (iaqSensor.status != BSEC_OK && iaqSensor.status != bsecStatus) {
                if (iaqSensor.status < BSEC_OK) {
                    ESP_LOGE(TAG, "BSEC error code: %d", iaqSensor.status);
                    status_set_error();
                } else {
                    ESP_LOGW(TAG, "BSEC warning code: %d", iaqSensor.status);
                    status_set_warning();
                }
            }

            if (iaqSensor.bme680Status != BME680_OK && iaqSensor.bme680Status != bme680Status) {
                if (iaqSensor.bme680Status < BME680_OK) {
                    ESP_LOGE(TAG, "BME680 error code: %d", iaqSensor.bme680Status);
                    status_set_error();
                } else {
                    ESP_LOGW(TAG, "BME680 warning code: %d", iaqSensor.bme680Status);
                    status_set_warning();
                }
            }

            bsecStatus = iaqSensor.status;
            bme680Status = iaqSensor.bme680Status;

            if (bsecStatus < BSEC_OK || bme680Status < BME680_OK) {
                return false;
            }

            status_clear_error();
            status_clear_warning();
            return true;
        }

        void loadState(void) {
            if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
                // Existing state in EEPROM
                ESP_LOGI(TAG, "Reading state from EEPROM");
                for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
                    bsecState[i] = EEPROM.read(i + 1);
                }

                iaqSensor.setState(bsecState);
                checkIaqSensorStatus();

                // Don't immediately update after initial load
                stateUpdateCounter++;
            } else {
                // Erase the EEPROM with zeroes
                ESP_LOGI(TAG, "Erasing EEPROM");
                for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++) {
                    EEPROM.write(i, 0);
                }
                EEPROM.commit();
            }
        }

        void updateState(void) {
            if (
                // First state update when IAQ accuracy is >= 3
                (stateUpdateCounter == 0 && iaqSensor.staticIaqAccuracy >= 3)
                // Then update every STATE_SAVE_PERIOD minutes
                || ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis())
            ) {
                stateUpdateCounter++;
            } else {
                return;
            }

            iaqSensor.getState(bsecState);
            if (!checkIaqSensorStatus()) {
                return;
            }

            ESP_LOGI(TAG, "Writing state to EEPROM");
            for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
                EEPROM.write(i + 1, bsecState[i]);
            }

            EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
            EEPROM.commit();
        }

    public:
        Sensor *temperature = new Sensor();
        Sensor *humidity = new Sensor();
        Sensor *pressure = new Sensor();
        Sensor *gasResistance = new Sensor();
        Sensor *staticIaq = new Sensor();
        Sensor *staticIaqAccuracy = new Sensor();
        Sensor *co2Equivalent = new Sensor();
        Sensor *breathVocEquivalent = new Sensor();

        void setup() override {
            EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1);
            iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);

            ESP_LOGI(
                TAG, "BSEC Version: %d.%d.%d.%d",
                iaqSensor.version.major, iaqSensor.version.minor,
                iaqSensor.version.major_bugfix, iaqSensor.version.minor_bugfix
            );

            iaqSensor.setConfig(bsec_config_iaq);

            // Load state from EEPROM
            loadState();

            // Subscribe to sensor values
            bsec_virtual_sensor_t sensorList[7] = {
                BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
                BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
                BSEC_OUTPUT_RAW_PRESSURE,
                BSEC_OUTPUT_RAW_GAS,
                BSEC_OUTPUT_STATIC_IAQ,
                BSEC_OUTPUT_CO2_EQUIVALENT,
                BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
            };
            iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
        }

        void setTemperatureOffset(float temp) {
            iaqSensor.setTemperatureOffset(temp);
        }

        void loop() override {
            if (checkIaqSensorStatus() && iaqSensor.run()) {
                sensorPushNum = 1;
                updateState();
            }

            // In order not to block here, spread the sensor state pushes
            // across subsequent calls otherwise we end up with API disconnects
            if (sensorPushNum > 0 && sensorPushNum <= 8) {
                switch (sensorPushNum++) {
                    case 1: temperature->publish_state(iaqSensor.temperature); break;
                    case 2: humidity->publish_state(iaqSensor.humidity); break;
                    case 3: pressure->publish_state(iaqSensor.pressure); break;
                    case 4: gasResistance->publish_state(iaqSensor.gasResistance); break;
                    case 5: staticIaq->publish_state(iaqSensor.staticIaq); break;
                    case 6: staticIaqAccuracy->publish_state(iaqSensor.staticIaqAccuracy); break;
                    case 7: co2Equivalent->publish_state(iaqSensor.co2Equivalent); break;
                    case 8: breathVocEquivalent->publish_state(iaqSensor.breathVocEquivalent); break;
                }
            }
        }
};