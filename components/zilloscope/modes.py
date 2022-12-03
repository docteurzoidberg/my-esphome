import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_NAME, CONF_EFFECTS
from esphome.util import Registry
from esphome.components import web_server_base
from esphome.components import time as time_
from esphome.components.web_server_base import CONF_WEB_SERVER_BASE_ID
from .types import DisplayBufferRef, ModeTime, ModeEffects, ModePaint, ModeMeteo, ModeLambda
from .effects import validate_effects, ADDRESSABLE_DISPLAY_EFFECTS, ADDRESSABLE_DISPLAY_EFFECTS_REGISTRY


CONF_RENDER_LAMBDA = 'render_lambda'
CONF_UPDATE_INTERVAL = 'update_interval'
CONF_DEFAULT_EFFECT = 'default_effect'
CONF_TIME_ID = 'homeassistant_time_id'

ZILLO_MODES = []
ZILLO_MODES_REGISTRY = Registry()

def register_mode(name, mode_type, default_name, schema, *extra_validators):
    ZILLO_MODES.append(name)
    #schema = cv.Schema(schema)
    schema = cv.Schema(schema).extend({
        cv.Optional(CONF_NAME, default=default_name): cv.string_strict,
        cv.Optional(CONF_UPDATE_INTERVAL, default='0ms'): cv.positive_time_period_milliseconds,
    })
    validator = cv.All(schema, *extra_validators)
    return ZILLO_MODES_REGISTRY.register(name,mode_type, validator)



@register_mode(
    'mode_time', ModeTime, "time", {
        cv.GenerateID(): cv.declare_id(ModeTime),
        cv.Required(CONF_TIME_ID): cv.use_id(time_.RealTimeClock),
        cv.Required(CONF_RENDER_LAMBDA): cv.lambda_
    }
)
def mode_time_to_code(config, mode_id):
    args = [(DisplayBufferRef, 'it'), (cg.uint32, 'frame'), (bool, 'initial_run')]
    time_id_ = yield cg.get_variable(config[CONF_TIME_ID])
    render_lambda_ = yield cg.process_lambda(config[CONF_RENDER_LAMBDA], args, return_type=cg.bool_)
    update_interval_ = yield cg.uint32(config[CONF_UPDATE_INTERVAL])
    name = yield cg.std_string(config[CONF_NAME])
    var = cg.new_Pvariable(mode_id, name, render_lambda_, time_id_, update_interval_)
    yield var

@register_mode(
    'mode_lambda', ModeLambda, "lambda", {
        cv.GenerateID(): cv.declare_id(ModeLambda),
        cv.Required(CONF_RENDER_LAMBDA): cv.lambda_
    }
)
def mode_lambda_to_code(config, mode_id):
    args = [(DisplayBufferRef, 'it'), (cg.uint32, 'frame'), (bool, 'initial_run')]
    render_lambda_ = yield cg.process_lambda(config[CONF_RENDER_LAMBDA], args, return_type=cg.bool_)
    update_interval_ = yield cg.uint32(config[CONF_UPDATE_INTERVAL])
    name = yield cg.std_string(config[CONF_NAME])
    var = cg.new_Pvariable(mode_id, name, render_lambda_, update_interval_)
    yield var

@register_mode(
    'mode_effects', ModeEffects, "effects", {
        cv.GenerateID(): cv.declare_id(ModeEffects),
        cv.Optional(CONF_DEFAULT_EFFECT, default=""): cv.string_strict,
        cv.Optional(CONF_EFFECTS): validate_effects(ADDRESSABLE_DISPLAY_EFFECTS)
    }
)
def mode_effects_to_code(config, mode_id):
    name = yield cg.std_string(config[CONF_NAME])
    default_effect = yield cg.std_string(config[CONF_DEFAULT_EFFECT])
    var = cg.new_Pvariable(mode_id, name)
    effects = yield cg.build_registry_list(ADDRESSABLE_DISPLAY_EFFECTS_REGISTRY, config.get(CONF_EFFECTS, []))
    cg.add(var.add_effects(effects))
    cg.add(var.set_default_effect(default_effect))
    yield var


@register_mode(
    'mode_meteo', ModeMeteo, "meteo", {
        cv.GenerateID(): cv.declare_id(ModeMeteo),
        cv.Required(CONF_RENDER_LAMBDA): cv.lambda_
    }
)
def mode_meteo_to_code(config, mode_id):
    args = [(DisplayBufferRef, 'it'), (cg.uint32, 'frame'), (bool, 'initial_run')]
    render_lambda_ = yield cg.process_lambda(config[CONF_RENDER_LAMBDA], args, return_type=cg.bool_)
    update_interval_ = yield cg.uint32(config[CONF_UPDATE_INTERVAL])
    name = yield cg.std_string(config[CONF_NAME])
    var = cg.new_Pvariable(mode_id, name, render_lambda_, update_interval_)
    yield var


@register_mode(
    'mode_paint', ModePaint, "paint", {
        cv.GenerateID(): cv.declare_id(ModePaint),
        cv.GenerateID(CONF_WEB_SERVER_BASE_ID): cv.use_id(
            web_server_base.WebServerBase
        ),
    }
)
def mode_paint_to_code(config, mode_id):
    paren = yield cg.get_variable(config[CONF_WEB_SERVER_BASE_ID])
    name = yield cg.std_string(config[CONF_NAME])
    var = cg.new_Pvariable(mode_id, name, paren)
    print('this is it')
    #yield cg.register_component(var, config)
    yield var

def validate_modes(allowed_modes):
    def validator(value):
        value = cv.validate_registry('mode', ZILLO_MODES_REGISTRY)(value)
        errors = []
        names = set()
        for i, x in enumerate(value):
            key = next(it for it in x.keys())
            if key not in allowed_modes:
                errors.append(
                    cv.Invalid("The mode '{}' is not allowed for this "
                               "zilloscope type".format(key), [i])
                )
                continue
            #new
            name = x[key][CONF_NAME]
            #OLD: name = key
            if name in names:
                errors.append(
                    cv.Invalid("Found the mode key '{}' twice. All modes must have "
                               "unique names".format(name), [i])
                )
                continue
            names.add(name)
        if errors:
            raise cv.MultipleInvalid(errors)
        return value

    return validator
