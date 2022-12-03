
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ['display', 'api', 'text_sensor']

CONF_ON_READY = 'on_ready'
CONF_TAG1_ID = 'tag1_id'
CONF_TAG2_ID = 'tag2_id'

plantiquette_ns = cg.esphome_ns.namespace('plantiquette')
Plantiquette = plantiquette_ns.class_('Plantiquette', cg.Component)

ReadyTrigger = plantiquette_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Plantiquette),
    cv.Required(CONF_TAG1_ID): cv.use_id(text_sensor.TextSensor),
    cv.Required(CONF_TAG2_ID): cv.use_id(text_sensor.TextSensor),
    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_tag1 = yield cg.get_variable(config[CONF_TAG1_ID])
    wrapped_tag2 = yield cg.get_variable(config[CONF_TAG2_ID])
    cg.add(var.set_tag1(wrapped_tag1))
    cg.add(var.set_tag2(wrapped_tag2))
    yield cg.register_component(var, config)
    for conf in config.get(CONF_ON_READY, []):
        readytrigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(readytrigger, [], conf)
