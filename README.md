This component facilitates use of the Bosch BME680 sensor with [ESPHome](https://esphome.io) via the closed source [Bosch BSEC library](https://github.com/BoschSensortec/BSEC-Arduino-library) providing additional calculated indoor air quality measurements not available with the [core component](https://esphome.io/components/sensor/bme680.html):

![Home Assistant Entities](ha-screenshot.png)

## Installation
As of ESPHome 1.18 this component is [included in the core distribution](https://esphome.io/components/sensor/bme680_bsec.html), however you may still wish to use this repository if a new release or experimental feature is available.

⚠️ **Note**: The Bosch BSEC library is only available for use after accepting its software license agreement. By enabling this component, you are explicitly agreeing to the terms of the [BSEC license agreement](https://www.bosch-sensortec.com/media/boschsensortec/downloads/bsec/2017-07-17_clickthrough_license_terms_environmentalib_sw_clean.pdf).

### As an external component
In ESPHome 1.18 and later you can use the [external components](https://esphome.io/components/external_components.html) system:
```yaml
external_components:
  - source: github://trvrnrth/esphome-bsec-bme680
```

### ESPHome 1.17 and earlier
Copy the `components/bme680_bsec` directory into your ESPHome `custom_components` directory (creating it if it does not exist). If you are running via Home Assistant this will be `/config/esphome/custom_components/bme680_bsec`.

## Dependencies
The [I2C Bus](https://esphome.io/components/i2c.html#i2c) must be set up in order for this component to work.

## Minimal configuration
The following configuration shows the minimal set up to read temperature and humidity from the sensor.
```yaml
i2c:

bme680_bsec:

sensor:
  - platform: bme680_bsec
    temperature:
      name: "BME680 Temperature"
    humidity:
      name: "BME680 Humidity"
```

## Advanced configuration
The following configuration shows all the available sensors and optional settings for the component. It also includes an example of filtering to guard against
outliers, limit the number of updates sent to home assistant and reduce storage requirements in other systems such as influxdb used to store historical data.

For each sensor all other options from [Sensor](https://esphome.io/components/sensor/index.html#config-sensor) and [TextSensor](https://esphome.io/components/text_sensor/index.html#base-text-sensor-configuration) are also available for filtering, automation and so on.

```yaml
bme680_bsec:
    # i2c address
    # -----------
    # Common values are:
    # - 0x76
    # - 0x77
    # Default: 0x76
    address: 0x76

    # Temperature offset
    # ------------------
    # Useful if device is in enclosure and reads too high
    # Default: 0
    temperature_offset: 0

    # IAQ calculation mode
    # --------------------
    # Available options:
    # - static (for fixed position devices)
    # - mobile (for on person or other moveable devices)
    # Default: static
    iaq_mode: static

    # Sample rate
    # -----------
    # This controls the sampling rate for gas-dependant sensors and will govern the interval
    # at which the sensor heater is operated.
    # By default this rate will also be used for temperature, pressure and humidity sensors
    # but these can be overridden on a per-sensor level if required.
    #
    # Available options:
    # - lp (low power - samples every 3 seconds)
    # - ulp (ultra low power - samples every 5 minutes)
    # Default: lp
    sample_rate: ulp

    # Interval at which to save BSEC state
    # ------------------------------------
    # Default: 6h
    state_save_interval: 6h

sensor:
  - platform: bme680_bsec
    temperature:
      # Temperature in °C
      name: "BME680 Temperature"
      sample_rate: lp
      filters:
        - median
    pressure:
      # Pressure in hPa
      name: "BME680 Pressure"
      sample_rate: lp
      filters:
        - median
    humidity:
      # Relative humidity %
      name: "BME680 Humidity"
      sample_rate: lp
      filters:
        - median
    gas_resistance:
      # Gas resistance in Ω
      name: "BME680 Gas Resistance"
      filters:
        - median
    iaq:
      # Indoor air quality value
      name: "BME680 IAQ"
      filters:
        - median
    iaq_accuracy:
      # IAQ accuracy as a numeric value of 0, 1, 2, 3
      name: "BME680 Numeric IAQ Accuracy"
    co2_equivalent:
      # CO2 equivalent estimate in ppm
      name: "BME680 CO2 Equivalent"
      filters:
        - median
    breath_voc_equivalent:
      # Volatile organic compounds equivalent estimate in ppm
      name: "BME680 Breath VOC Equivalent"
      filters:
        - median

text_sensor:
  - platform: bme680_bsec
    iaq_accuracy:
      # IAQ accuracy as a text value of Stabilizing, Uncertain, Calibrating, Calibrated
      name: "BME680 IAQ Accuracy"
```

## Indoor Air Quality (IAQ) Measurement
Indoor Air Quality measurements are expressed in the IAQ index scale with 25IAQ corresponding to typical good air and 250IAQ
indicating typical polluted air after calibration.

## IAQ Accuracy and Calibration
The BSEC algorithm automatically gathers data in order to calibrate the IAQ measurements. The IAQ Accuracy sensor will give one
of the following values:

- `Stabilizing`: The device has just started, and the sensor is stabilizing (this typically lasts 5 minutes)
- `Uncertain`: The background history of BSEC is uncertain. This typically means the gas sensor data was too
  stable for BSEC to clearly define its reference.
- `Calibrating`: BSEC found new calibration data and is currently calibrating.
- `Calibrated`: BSEC calibrated successfully.

Every `state_save_interval`, or as soon thereafter as full calibration is reached, the current algorithm state is saved to flash
so that the process does not have to start from zero on device restart.
