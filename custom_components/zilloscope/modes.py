import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_NAME
from esphome.util import Registry
from .types import DisplayBufferRef, ZilloModeTime, ZilloModeEffects

CONF_RENDER_LAMBDA = 'render_lambda'
CONF_UPDATE_INTERVAL = 'update_interval'

MODES_REGISTRY = Registry()

def register_mode(name, default_name, schema, *extra_validators):
    schema = cv.Schema(schema).extend({
        cv.Optional(CONF_NAME, default=default_name): cv.string_strict,
        cv.Optional(CONF_UPDATE_INTERVAL, default='0ms'): cv.positive_time_period_milliseconds,
    })
    validator = cv.All(schema, *extra_validators)
    return MODES_REGISTRY.register(name, validator)


@register_mode(
    'mode_time', ZilloModeTime, "time", {
        cv.GenerateID(): cv.declare_id(ZilloModeTime),
        cv.Required(CONF_RENDER_LAMBDA): cv.lambda_
    },
)
def mode_time_to_code(config, mode_id):
    args = [(DisplayBufferRef, 'it'), (bool, 'initial_run')]
    render_lambda_ = yield cg.process_lambda(config[CONF_RENDER_LAMBDA], args, return_type=cg.void)
    var = cg.new_Pvariable(mode_id, config[CONF_NAME], render_lambda_)
    yield var

@register_mode(
    'mode_effects', ZilloModeEffects, "effects", {
        cv.GenerateID(): cv.declare_id(ZilloModeEffects),
    },
)
def mode_effects_to_code(config, mode_id):
    var = cg.new_Pvariable(mode_id, config[CONF_NAME])
    yield var

