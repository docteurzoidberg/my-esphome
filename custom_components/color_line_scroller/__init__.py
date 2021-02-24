import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ['display']
MULTI_CONF = True

colorlinescroller_ns = cg.esphome_ns.namespace('colorlinescroller')
ColorLineScroller = colorlinescroller_ns.class_('ColorLineScroller', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ColorLineScroller),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
