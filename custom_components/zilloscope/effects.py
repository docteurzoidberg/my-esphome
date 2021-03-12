import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_NAME, CONF_LAMBDA, CONF_UPDATE_INTERVAL
from esphome.util import Registry
from .types import AddressableLightDisplayRef, AddressableLambdaLightDisplayEffect, ESPColor

CONF_ADDRESSABLE_LAMBDA = 'addressable_lambda'
ADDRESSABLE_EFFECTS = []
EFFECTS_REGISTRY = Registry()


def register_effect(name, effect_type, default_name, schema, *extra_validators):
    schema = cv.Schema(schema).extend({
        cv.Optional(CONF_NAME, default=default_name): cv.string_strict,
    })
    validator = cv.All(schema, *extra_validators)
    return EFFECTS_REGISTRY.register(name, effect_type, validator)

def register_addressable_effect(name, effect_type, default_name, schema, *extra_validators):
    # addressable effect can be used only in addressable
    ADDRESSABLE_EFFECTS.append(name)
    return register_effect(name, effect_type, default_name, schema, *extra_validators)


@register_addressable_effect(
    'addressable_lambda', AddressableLambdaLightDisplayEffect, "Addressable Lambda", {
        cv.Required(CONF_LAMBDA): cv.lambda_,
        cv.Optional(CONF_UPDATE_INTERVAL, default='0ms'): cv.positive_time_period_milliseconds,
    }
)
def addressable_lambda_effect_to_code(config, effect_id):
    args = [(AddressableLightDisplayRef, 'it'), (ESPColor, 'current_color'), (bool, 'initial_run')]
    lambda_ = yield cg.process_lambda(config[CONF_LAMBDA], args, return_type=cg.void)
    var = cg.new_Pvariable(effect_id, config[CONF_NAME], lambda_,
                           config[CONF_UPDATE_INTERVAL])
    yield var


def validate_effects(allowed_effects):
    def validator(value):
        value = cv.validate_registry('effect', EFFECTS_REGISTRY)(value)
        errors = []
        names = set()
        for i, x in enumerate(value):
            key = next(it for it in x.keys())
            if key not in allowed_effects:
                errors.append(
                    cv.Invalid("The effect '{}' is not allowed for this "
                               "light type".format(key), [i])
                )
                continue
            name = x[key][CONF_NAME]
            if name in names:
                errors.append(
                    cv.Invalid("Found the effect name '{}' twice. All effects must have "
                               "unique names".format(name), [i])
                )
                continue
            names.add(name)
        if errors:
            raise cv.MultipleInvalid(errors)
        return value

    return validator
