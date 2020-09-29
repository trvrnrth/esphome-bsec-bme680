This component facilitates use of the Bosch BME680 sensor with [ESPHome](https://esphome.io) via the closed source [Bosch BSEC library](https://github.com/BoschSensortec/BSEC-Arduino-library) providing additional calculated indoor air quality measurements not available with the [core component](https://esphome.io/components/sensor/bme680.html):

![Home Assistant Entities](ha-screenshot.png)

## Installation
Copy the `bme680_bsec` directory into your ESPHome `custom_components` directory (creating it if it does not exist).

## Dependencies
The [I2C Bus](https://esphome.io/components/i2c.html#i2c) must be set up in order for this component to work.

The Bosch BSEC library must be included as shown below. Currently this relies on a forked version which includes a repackaged library for ESP8266 but there is an [upstream PR](https://github.com/BoschSensortec/BSEC-Arduino-library/pull/51) open to get this incorporated into the next official release.
```yaml
esphome:
  libraries:
    - bsec=https://github.com/trvrnrth/BSEC-Arduino-library
```

## Example configuration
The following configuration shows the basic sensors and the available optional settings for the component. For each sensor all other options from [Sensor](https://esphome.io/components/sensor/index.html#config-sensor) are also available for filtering, automation and so on.

```yaml
sensor:
  - platform: bme680_bsec
    temperature:
      name: "BME680 Temperature"
    pressure:
      name: "BME680 Pressure"
    humidity:
      name: "BME680 Humidity"
    gas_resistance:
      name: "BME680 Gas Resistance"
    iaq:
      name: "BME680 IAQ"
    co2_equivalent:
      name: "BME680 CO2 Equivalent"
    breath_voc_equivalent:
      name: "BME680 Breath VOC Equivalent"

    # i2c address override (default is 0x76)
    address: 0x77

    # Temperature offset if device is in enclosure and reads too high (default is 0)
    temperature_offset: 0.9

    # Mode for IAQ sensors if device is mobile (default is static)
    iaq_mode: mobile

    # Interval at which to save BSEC state (default is 6 hours)
    state_save_interval: 4h
```

### IAQ Accuracy
It is also possible to include the IAQ accuracy sensor which provides a value between 0 and 3. Typically it's more desireable to have these exposed as the textual values of Stabilizing, Uncertain, Calibrating and Calibrated so the component provides a helper method for this which can be used as shown here:
```yaml
sensor:
  - platform: bme680_bsec
    id: example_bme680_bsec
    iaq_accuracy:
      name: "BME680 Raw IAQ Accuracy"
      internal: True
      on_value:
        - text_sensor.template.publish:
            id: bme680_iaq_accuracy
            state: !lambda |-
              return { id(example_bme680_bsec).calc_iaq_accuracy_text(x) };

text_sensor:
  - platform: template
    id: bme680_iaq_accuracy
    name: "BME680 IAQ Accuracy"
    icon: mdi:checkbox-marked-circle-outline
```