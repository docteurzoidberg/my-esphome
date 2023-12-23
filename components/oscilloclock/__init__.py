import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.components import time as time_

DEPENDENCIES = ["network"]

CONF_ON_READY = 'on_ready'
CONF_TIME_ID = 'time_id'

oscilloclock_ns = cg.esphome_ns.namespace('oscilloclock')
OscilloClock = oscilloclock_ns.class_('OscilloClock', cg.Component)
ReadyTrigger = oscilloclock_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    #id
    cv.GenerateID(): cv.declare_id(OscilloClock),
    #references
    cv.Required(CONF_TIME_ID): cv.use_id(time_.RealTimeClock),
    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_time = yield cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time(wrapped_time))
    yield cg.register_component(var, config)
