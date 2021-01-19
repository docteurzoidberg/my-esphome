import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

alarm_keypad_component_ns = cg.esphome_ns.namespace('alarm_keypad_component')
AlarmKeypadComponent = alarm_keypad_component_ns.class_('AlarmKeypadComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AlarmKeypadComponent)
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)