
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import homeassistant, text_sensor, ht16k33_alpha
from esphome.const import CONF_ID, CONF_TRIGGER_ID

DEPENDENCIES = ['display','light','api', 'text_sensor']

CONF_ON_READY = 'on_ready'
CONF_ON_CODE_CHECK = 'on_code_check'
CONF_ON_CODE_OK = 'on_code_ok'
CONF_ON_CODE_KO = 'on_code_ko'

CONF_DISPLAY_ID = 'display_id'
CONF_ALARM_STATUS_ID = 'alarm_status_id'
CONF_KEYPAD_TEXT_ID = 'keypad_text_id'


alarm_keypad_ns = cg.esphome_ns.namespace('alarm_keypad')
AlarmKeypad = alarm_keypad_ns.class_('AlarmKeypad', cg.Component)

ReadyTrigger = alarm_keypad_ns.class_('ReadyTrigger', automation.Trigger.template())
CodeCheckTrigger = alarm_keypad_ns.class_('CodeCheckTrigger', automation.Trigger.template())
CodeOKTrigger = alarm_keypad_ns.class_('CodeOKTrigger', automation.Trigger.template())
CodeKOTrigger = alarm_keypad_ns.class_('CodeKOTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AlarmKeypad),
     #references
    cv.Required(CONF_DISPLAY_ID): cv.use_id(ht16k33_alpha.display.HT16K33AlphaDisplay),
    cv.Required(CONF_ALARM_STATUS_ID): cv.use_id(homeassistant.text_sensor.HomeassistantTextSensor),
    cv.Required(CONF_KEYPAD_TEXT_ID): cv.use_id(text_sensor.TextSensor),

    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),
    cv.Optional(CONF_ON_CODE_CHECK): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CodeCheckTrigger),
    }),
    cv.Optional(CONF_ON_CODE_OK): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CodeOKTrigger),
    }),
    cv.Optional(CONF_ON_CODE_KO): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CodeKOTrigger),
    }),

}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_display = yield cg.get_variable(config[CONF_DISPLAY_ID])
    wrapped_alarm_status = yield cg.get_variable(config[CONF_ALARM_STATUS_ID])
    wrapped_keypad_text = yield cg.get_variable(config[CONF_KEYPAD_TEXT_ID])
    cg.add(var.set_display(wrapped_display))
    cg.add(var.set_alarmstatusentity(wrapped_alarm_status))
    cg.add(var.set_keypadtext(wrapped_keypad_text))
    yield cg.register_component(var, config)

    for conf in config.get(CONF_ON_READY, []):
        readytrigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(readytrigger, [], conf)

    for conf in config.get(CONF_ON_CODE_CHECK, []):
        codechecktrigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(codechecktrigger, [], conf)

    for conf in config.get(CONF_ON_CODE_OK, []):
        codeoktrigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(codeoktrigger, [], conf)

    for conf in config.get(CONF_ON_CODE_KO, []):
        codekotrigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(codekotrigger, [], conf)
