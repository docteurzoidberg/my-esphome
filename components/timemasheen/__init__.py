from typing import Optional
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import time as time_, light
from esphome.const import CONF_ID

DEPENDENCIES = ["network"]

CONF_TIME_ID = 'time_id'
CONF_CLOCK_ADDRESSABLE_LIGHT_ID = 'clock_addressable_light_id'

light_ns = cg.esphome_ns.namespace("light")
LightState = light_ns.class_("LightState", cg.Nameable, cg.Component)
AddressableLightState = light_ns.class_("LightState", LightState)

timemasheen_ns = cg.esphome_ns.namespace('timemasheen')
TimeMasheen = timemasheen_ns.class_('TimeMasheen', cg.Component)


CONFIG_SCHEMA = cv.Schema({
    #id
    cv.GenerateID(): cv.declare_id(TimeMasheen),
    #references
    cv.Required(CONF_TIME_ID): cv.use_id(time_.RealTimeClock),
    cv.Required(CONF_CLOCK_ADDRESSABLE_LIGHT_ID): cv.use_id(light.AddressableLightState),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_time = yield cg.get_variable(config[CONF_TIME_ID])
    wrapped_clock_leds = yield cg.get_variable(config[CONF_CLOCK_ADDRESSABLE_LIGHT_ID])
    cg.add(var.set_time(wrapped_time))
    cg.add(var.set_clock_addressable_lights(wrapped_clock_leds))
    yield cg.register_component(var, config)
