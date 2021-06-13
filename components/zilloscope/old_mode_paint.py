from typing import Optional
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from .types import Mode

DEPENDENCIES = ["network"]
AUTO_LOAD = ["web_server_base"]

zilloscope_ns = cg.esphome_ns.namespace('zilloscope')
ModePaint= zilloscope_ns.class_('ModePaint', Mode)

CONFIG_SCHEMA = cv.Schema({
    #id
    cv.GenerateID(): cv.declare_id(ModePaint),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], 'paint', render_lambda_, update_interval_)
    yield var

