from typing import Optional
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import display
from esphome.components import time as time_
from esphome.const import CONF_ID, CONF_TRIGGER_ID

from .modes import validate_modes, ZILLO_MODES, ZILLO_MODES_REGISTRY

DEPENDENCIES = ["network"]
AUTO_LOAD = ["web_server_base"]

CONF_ON_BOOT = 'on_boot'
CONF_ON_SPLASH = 'on_splash'
CONF_ON_READY = 'on_ready'

CONF_DISPLAY_ID = 'display_id'
CONF_TIME_ID = 'time_id'

CONF_USE_SPLASH = 'use_splash'
CONF_DEFAULT_MODE = 'default_mode'
CONF_MODES = 'modes'

CONF_RENDER_BOOT = 'render_boot'
CONF_RENDER_SPLASH = 'render_splash'
CONF_RENDER_TIME = 'render_time'
CONF_RENDER_OTA = 'render_ota'
CONF_RENDER_SHUTDOWN = 'render_shutdown'
CONF_RENDER_NOTIFICATION = 'render_notification'

zilloscope_ns = cg.esphome_ns.namespace('zilloscope')
ZilloScope = zilloscope_ns.class_('ZilloScope', cg.Component)

BootTrigger = zilloscope_ns.class_('BootTrigger', automation.Trigger.template())
SplashTrigger = zilloscope_ns.class_('SplashTrigger', automation.Trigger.template())
ReadyTrigger = zilloscope_ns.class_('ReadyTrigger', automation.Trigger.template())

CONFIG_SCHEMA = cv.Schema({
    #id
    cv.GenerateID(): cv.declare_id(ZilloScope),

    #references
    cv.Required(CONF_DISPLAY_ID): cv.use_id(display.DisplayBuffer),
    cv.Required(CONF_TIME_ID): cv.use_id(time_.RealTimeClock),

    #config options
    cv.Optional(CONF_DEFAULT_MODE, default=""): cv.string_strict,
    cv.Optional(CONF_USE_SPLASH, default=False): cv.boolean,

    #display render lambdas
    cv.Required(CONF_RENDER_BOOT): cv.lambda_,
    cv.Required(CONF_RENDER_OTA): cv.lambda_,
    cv.Required(CONF_RENDER_SHUTDOWN): cv.lambda_,
    cv.Required(CONF_RENDER_NOTIFICATION): cv.lambda_,
    cv.Optional(CONF_RENDER_SPLASH): cv.lambda_,

    #Todo: validate at least one mode in [...]
    cv.Required(CONF_MODES): validate_modes(ZILLO_MODES),

    #Todo: validate at least one effect
    #Todo: validate mode effect present in modes?
    #cv.Optional(CONF_EFFECTS): validate_effects(ADDRESSABLE_DISPLAY_EFFECTS),

    #optional automation triggers
    cv.Optional(CONF_ON_BOOT): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(BootTrigger),
    }),
    cv.Optional(CONF_ON_SPLASH): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(SplashTrigger),
    }),
    cv.Optional(CONF_ON_READY): automation.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ReadyTrigger),
    }),

}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    wrapped_display = yield cg.get_variable(config[CONF_DISPLAY_ID])
    wrapped_time = yield cg.get_variable(config[CONF_TIME_ID])
    default_mode = yield cg.std_string(config[CONF_DEFAULT_MODE])

    cg.add(var.set_display(wrapped_display))
    cg.add(var.set_time(wrapped_time))
    yield cg.register_component(var, config)

    modes = yield cg.build_registry_list(ZILLO_MODES_REGISTRY, config.get(CONF_MODES, []))
    cg.add(var.add_modes(modes))
    cg.add(var.set_config_default_mode(default_mode))

    render_boot_template_ = yield cg.process_lambda(config[CONF_RENDER_BOOT],[(display.DisplayBufferRef, 'it'),(cg.uint32,'frame')],return_type=cg.bool_)
    render_notification_template_ = yield cg.process_lambda(config[CONF_RENDER_NOTIFICATION],[(display.DisplayBufferRef, 'it'),(cg.uint32,'frame'),(cg.std_string,'text'),(cg.uint32, 'type')],return_type=cg.bool_)
    render_ota_template_ = yield cg.process_lambda(config[CONF_RENDER_OTA],[(display.DisplayBufferRef, 'it'),(cg.uint32,'frame')],return_type=cg.bool_)
    render_shutdown_template_ = yield cg.process_lambda(config[CONF_RENDER_SHUTDOWN],[(display.DisplayBufferRef, 'it'),(cg.uint32,'frame')],return_type=cg.bool_)

    cg.add(var.set_render_boot(render_boot_template_))
    cg.add(var.set_render_notification(render_notification_template_))
    cg.add(var.set_render_ota(render_ota_template_))
    cg.add(var.set_render_shutdown(render_shutdown_template_))

    if CONF_USE_SPLASH in config:
        wrapped_usesplash = yield cg.bool_(config[CONF_USE_SPLASH])
        cg.add(var.set_config_use_splash(wrapped_usesplash))
        #TODO: how to raise error if use splash is set to true but no lambda specified?
        render_splash_template_ = yield cg.process_lambda(config[CONF_RENDER_SPLASH],[(display.DisplayBufferRef, 'it'),(cg.uint32,'frame')],return_type=cg.bool_)
        cg.add(var.set_render_splash(render_splash_template_))

    for conf in config.get(CONF_ON_BOOT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_SPLASH, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)
    for conf in config.get(CONF_ON_READY, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield automation.build_automation(trigger, [], conf)