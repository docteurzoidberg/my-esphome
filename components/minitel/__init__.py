
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ['uart']

CONF_ON_READY = 'on_ready'

minitel_ns = cg.esphome_ns.namespace('minitel')
Minitel = minitel_ns.class_('Minitel', cg.Component)
ReadyTrigger = minitel_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Minitel),
    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    for conf in config.get(CONF_ON_READY, []):
        readytrigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(readytrigger, [], conf)
