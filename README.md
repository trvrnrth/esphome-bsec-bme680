This component allows use of the Bosch BME680 sensor with ESPHome via the closed source [Bosch BSEC library](https://github.com/BoschSensortec/BSEC-Arduino-library) which provides additional calculated indoor air quality measurements:

![Home Assistant Entities](ha-screenshot.png)

## Example node configuration
```
substitutions:
  location_id: example
  location: Example
  temp_offset: '0'

<<: !include .bsecsensor.yaml
```
