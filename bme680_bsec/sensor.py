import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core
from esphome.components import i2c, sensor, text_sensor
from esphome.const import CONF_ID, \
    CONF_TEMPERATURE, CONF_PRESSURE, CONF_HUMIDITY, CONF_GAS_RESISTANCE, \
    CONF_ICON, \
    UNIT_CELSIUS, UNIT_HECTOPASCAL, UNIT_PERCENT, UNIT_OHM, UNIT_PARTS_PER_MILLION, UNIT_EMPTY, \
    ICON_THERMOMETER, ICON_GAUGE, ICON_WATER_PERCENT, ICON_GAS_CYLINDER

CONF_IAQ = 'iaq'
CONF_IAQ_ACCURACY = 'iaq_accuracy'
CONF_IAQ_MODE = 'iaq_mode'
CONF_CO2_EQUIVALENT = 'co2_equivalent'
CONF_BREATH_VOC_EQUIVALENT = 'breath_voc_equivalent'
CONF_STATE_SAVE_INTERVAL = 'state_save_interval'
CONF_TEMP_OFFSET = 'temperature_offset'
UNIT_IAQ = 'IAQ'
ICON_TEST_TUBE = 'mdi:test-tube'
ICON_ACCURACY = 'mdi:checkbox-marked-circle-outline'

DEPENDENCIES = ['i2c']

bme680_bsec_ns = cg.esphome_ns.namespace('bme680_bsec')

BME680IAQMode = bme680_bsec_ns.enum('BME680IAQMode')
IAQ_MODE_OPTIONS = {
    'STATIC': BME680IAQMode.BME680_IAQ_MODE_STATIC,
    'MOBILE': BME680IAQMode.BME680_IAQ_MODE_MOBILE,
}

BME680BSECComponent = bme680_bsec_ns.class_('BME680BSECComponent', cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BME680BSECComponent),
    cv.Optional(CONF_TEMP_OFFSET, default=0): cv.float_range(min=-100, max=100),
    cv.Optional(CONF_IAQ_MODE, default='STATIC'):
        cv.enum(IAQ_MODE_OPTIONS, upper=True),
    cv.Optional(CONF_STATE_SAVE_INTERVAL, default='6hours'): cv.positive_time_period_minutes,
    cv.Optional(CONF_TEMPERATURE):
        sensor.sensor_schema(UNIT_CELSIUS, ICON_THERMOMETER, 1),
    cv.Optional(CONF_PRESSURE):
        sensor.sensor_schema(UNIT_HECTOPASCAL, ICON_GAUGE, 1),
    cv.Optional(CONF_HUMIDITY):
        sensor.sensor_schema(UNIT_PERCENT, ICON_WATER_PERCENT, 1),
    cv.Optional(CONF_GAS_RESISTANCE):
        sensor.sensor_schema(UNIT_OHM, ICON_GAS_CYLINDER, 1),
    cv.Optional(CONF_IAQ):
        sensor.sensor_schema(UNIT_IAQ, ICON_GAUGE, 1),
    cv.Optional(CONF_IAQ_ACCURACY):
        sensor.sensor_schema(UNIT_EMPTY, ICON_GAUGE, 0),
    cv.Optional(CONF_CO2_EQUIVALENT):
        sensor.sensor_schema(UNIT_PARTS_PER_MILLION , ICON_TEST_TUBE, 1),
    cv.Optional(CONF_BREATH_VOC_EQUIVALENT):
        sensor.sensor_schema(UNIT_PARTS_PER_MILLION , ICON_TEST_TUBE, 1),
}).extend(i2c.i2c_device_schema(0x76))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    cg.add(var.set_temperature_offset(config[CONF_TEMP_OFFSET]))
    cg.add(var.set_iaq_mode(config[CONF_IAQ_MODE]))
    cg.add(var.set_state_save_interval(config[CONF_STATE_SAVE_INTERVAL]))

    if CONF_TEMPERATURE in config:
        conf = config[CONF_TEMPERATURE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_temperature_sensor(sens))

    if CONF_PRESSURE in config:
        conf = config[CONF_PRESSURE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_pressure_sensor(sens))

    if CONF_HUMIDITY in config:
        conf = config[CONF_HUMIDITY]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_humidity_sensor(sens))

    if CONF_GAS_RESISTANCE in config:
        conf = config[CONF_GAS_RESISTANCE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_gas_resistance_sensor(sens))

    if CONF_IAQ in config:
        conf = config[CONF_IAQ]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_iaq_sensor(sens))

    if CONF_IAQ_ACCURACY in config:
        conf = config[CONF_IAQ_ACCURACY]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_iaq_accuracy_sensor(sens))

    if CONF_CO2_EQUIVALENT in config:
        conf = config[CONF_CO2_EQUIVALENT]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_co2_equivalent_sensor(sens))

    if CONF_BREATH_VOC_EQUIVALENT in config:
        conf = config[CONF_BREATH_VOC_EQUIVALENT]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_breath_voc_equivalent_sensor(sens))
