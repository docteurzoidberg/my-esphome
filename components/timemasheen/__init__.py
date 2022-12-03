import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.components import time as time_, light

DEPENDENCIES = ["network"]

CONF_ON_READY = 'on_ready'
CONF_TIME_ID = 'time_id'
CONF_CLOCK_ADDRESSABLE_LIGHT_ID = 'clock_addressable_light_id'

light_ns = cg.esphome_ns.namespace("light")
LightState = light_ns.class_("LightState", cg.Component)
AddressableLightState = light_ns.class_("LightState", LightState)

timemasheen_ns = cg.esphome_ns.namespace('timemasheen')
TimeMasheen = timemasheen_ns.class_('TimeMasheen', cg.Component)
ReadyTrigger = timemasheen_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    #id
    cv.GenerateID(): cv.declare_id(TimeMasheen),
    #references
    cv.Required(CONF_TIME_ID): cv.use_id(time_.RealTimeClock),
    cv.Required(CONF_CLOCK_ADDRESSABLE_LIGHT_ID): cv.use_id(light.AddressableLightState),

    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_time = yield cg.get_variable(config[CONF_TIME_ID])
    wrapped_clock_leds = yield cg.get_variable(config[CONF_CLOCK_ADDRESSABLE_LIGHT_ID])
    cg.add(var.set_time(wrapped_time))
    cg.add(var.set_clock_addressable_lights(wrapped_clock_leds))
    yield cg.register_component(var, config)
