import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_OUTPUT, CONF_TRIGGER_ID

CONF_ON_READY = 'on_ready'

alarm_keypad_component_ns = cg.esphome_ns.namespace('alarm_keypad_component')
AlarmKeypadComponent = alarm_keypad_component_ns.class_('AlarmKeypadComponent', cg.Component)
ReadyTrigger = alarm_keypad_component_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AlarmKeypadComponent),
    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    for conf in config.get(CONF_ON_READY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)