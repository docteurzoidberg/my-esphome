import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_NAME
from esphome.util import Registry
from .types import DisplayBufferRef, ModeTime, ModeEffects, ModePaint
from esphome.components import web_server_base
from esphome.components.web_server_base import CONF_WEB_SERVER_BASE_ID

CONF_RENDER_LAMBDA = 'render_lambda'
CONF_UPDATE_INTERVAL = 'update_interval'

ZILLO_MODES = []
ZILLO_MODES_REGISTRY = Registry()

def register_mode(name, mode_type, default_name, schema, *extra_validators):
    ZILLO_MODES.append(name)
    #schema = cv.Schema(schema)
    schema = cv.Schema(schema).extend({
        #cv.Optional(CONF_NAME, default=default_name): cv.string_strict,
        cv.Optional(CONF_UPDATE_INTERVAL, default='0ms'): cv.positive_time_period_milliseconds,
    })
    validator = cv.All(schema, *extra_validators)
    return ZILLO_MODES_REGISTRY.register(name,mode_type, validator)



@register_mode(
    'mode_time', ModeTime, "time", {
        cv.GenerateID(): cv.declare_id(ModeTime),
        cv.Required(CONF_RENDER_LAMBDA): cv.lambda_
    }
)
def mode_time_to_code(config, mode_id):
    args = [(DisplayBufferRef, 'it'), (cg.uint32, 'frame'), (bool, 'initial_run')]
    render_lambda_ = yield cg.process_lambda(config[CONF_RENDER_LAMBDA], args, return_type=cg.bool_)
    update_interval_ = yield cg.uint32(config[CONF_UPDATE_INTERVAL])
    var = cg.new_Pvariable(mode_id, 'time', render_lambda_, update_interval_)
    yield var

@register_mode(
    'mode_effects', ModeEffects, "effects", {
        cv.GenerateID(): cv.declare_id(ModeEffects),
    }
)
def mode_effects_to_code(config, mode_id):
    var = cg.new_Pvariable(mode_id, 'effects')
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
    var = cg.new_Pvariable(mode_id, 'paint', paren)
    yield cg.register_component(var, config)
    #yield var

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
            #name = x[key][CONF_NAME]
            name = key
            if name in names:
                errors.append(
                    cv.Invalid("Found the mode key '{}' twice. All effects must have "
                               "unique names".format(name), [i])
                )
                continue
            names.add(name)
        if errors:
            raise cv.MultipleInvalid(errors)
        return value

    return validator
