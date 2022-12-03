import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID
from .. import ZilloScope, zilloscope_ns

CONF_ZILLOSCOPE_ID = 'zilloscope_id'
CONF_VALUETYPE = 'valuetype'
DEPENDENCIES = ['zilloscope']

ZilloscopeTextSensor = zilloscope_ns.class_('ZilloscopeTextSensor', text_sensor.TextSensor, cg.Component)

CONFIG_SCHEMA = cv.All(text_sensor.TEXT_SENSOR_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ZilloscopeTextSensor),
    cv.GenerateID(CONF_ZILLOSCOPE_ID): cv.use_id(ZilloScope),
    cv.Required(CONF_VALUETYPE): cv.string,
}))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await text_sensor.register_text_sensor(var, config)
    zilloscope = await cg.get_variable(config[CONF_ZILLOSCOPE_ID])
    valuetype = config[CONF_VALUETYPE]
    cg.add(zilloscope.register_text_sensor(var, valuetype))
