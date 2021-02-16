import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import display, font
from esphome.components import time as time_
from esphome.const import CONF_ID, CONF_TRIGGER_ID

CONF_ON_READY = 'on_ready'
CONF_DISPLAY_ID = 'display_id'
CONF_FONT_ID = 'font_id'
CONF_TIME_ID = 'time_id'

CONF_RENDER_BOOT = 'render_boot'
CONF_RENDER_TIME = 'render_time'
CONF_RENDER_OTA = 'render_ota'
CONF_RENDER_SHUTDOWN = 'render_shutdown'

zilloscope_ns = cg.esphome_ns.namespace('zilloscope')
ZilloScope = zilloscope_ns.class_('ZilloScope', cg.Component)
ReadyTrigger = zilloscope_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ZilloScope),
    cv.Required(CONF_DISPLAY_ID): cv.use_id(display.DisplayBuffer),
    cv.Required(CONF_FONT_ID): cv.use_id(font.Font),
    cv.Required(CONF_TIME_ID): cv.use_id(time_.RealTimeClock),
    cv.Required(CONF_RENDER_BOOT): cv.lambda_,
    cv.Required(CONF_RENDER_TIME): cv.lambda_,
    cv.Required(CONF_RENDER_OTA): cv.lambda_,
    cv.Required(CONF_RENDER_SHUTDOWN): cv.lambda_,
    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),

}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_display = yield cg.get_variable(config[CONF_DISPLAY_ID])
    wrapped_font = yield cg.get_variable(config[CONF_FONT_ID])
    wrapped_time = yield cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_display(wrapped_display))
    cg.add(var.set_font(wrapped_font))
    cg.add(var.set_time(wrapped_time))
    yield cg.register_component(var, config)

    render_boot_template_ = yield cg.process_lambda(config[CONF_RENDER_BOOT],[(display.DisplayBufferRef, 'it')],return_type=cg.void)
    render_time_template_ = yield cg.process_lambda(config[CONF_RENDER_TIME],[(display.DisplayBufferRef, 'it')],return_type=cg.void)
    render_ota_template_ = yield cg.process_lambda(config[CONF_RENDER_OTA],[(display.DisplayBufferRef, 'it')],return_type=cg.void)
    render_shutdown_template_ = yield cg.process_lambda(config[CONF_RENDER_SHUTDOWN],[(display.DisplayBufferRef, 'it')],return_type=cg.void)

    cg.add(var.set_render_boot(render_boot_template_))
    cg.add(var.set_render_time(render_time_template_))
    cg.add(var.set_render_ota(render_ota_template_))
    cg.add(var.set_render_shutdown(render_shutdown_template_))

    for conf in config.get(CONF_ON_READY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)