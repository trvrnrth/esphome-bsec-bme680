This component facilitates use of the Bosch BME680 sensor with [ESPHome](https://esphome.io) via the closed source [Bosch BSEC library](https://github.com/BoschSensortec/BSEC-Arduino-library) providing additional calculated indoor air quality measurements not available with the [core component](https://esphome.io/components/sensor/bme680.html):

![Home Assistant Entities](ha-screenshot.png)

## Implementation notes
The base config you see here is specifically for a Wemos D1 Mini ESP8266 board being used in a static location, with median filtered readings to reduce timeline data storage requirements.

With the appropriate changes to the platform it should also work fine on the likes of an ESP32. Similarly the reading accuracy could be increased to 2 decimal places with no filtering, and the component amended to read IAQ readings intended for mobile applications depending on your requirements.

## Example node configuration
```
substitutions:
  location_id: example
  location: Example
  temp_offset: '0'

<<: !include .bsecsensor.yaml
```
