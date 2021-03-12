import esphome.codegen as cg
from esphome import automation

# Base
light_ns = cg.esphome_ns.namespace('light')
addressable_light_display_ns = cg.esphome_ns.namespace('addressable_light_display')
zilloscope_ns = cg.esphome_ns.namespace('zilloscope')


AddressableLightDisplay = addressable_light_display_ns.class_('AddressableLightDisplay', cg.Component)
AddressableLightDisplayRef = AddressableLightDisplay.operator('ref')

ESPColor = light_ns.class_('ESPColor')
LightColorValues = light_ns.class_('LightColorValues')

# Actions
ToggleAction = light_ns.class_('ToggleAction', automation.Action)
LightControlAction = light_ns.class_('LightControlAction', automation.Action)
DimRelativeAction = light_ns.class_('DimRelativeAction', automation.Action)
AddressableSet = light_ns.class_('AddressableSet', automation.Action)
LightIsOnCondition = light_ns.class_('LightIsOnCondition', automation.Condition)
LightIsOffCondition = light_ns.class_('LightIsOffCondition', automation.Condition)

# Triggers
LightTurnOnTrigger = light_ns.class_('LightTurnOnTrigger', automation.Trigger.template())
LightTurnOffTrigger = light_ns.class_('LightTurnOffTrigger', automation.Trigger.template())

# Effects
LightEffect = light_ns.class_('LightEffect')
RandomLightEffect = light_ns.class_('RandomLightEffect', LightEffect)
LambdaLightEffect = light_ns.class_('LambdaLightEffect', LightEffect)
AutomationLightEffect = light_ns.class_('AutomationLightEffect', LightEffect)
StrobeLightEffect = light_ns.class_('StrobeLightEffect', LightEffect)
StrobeLightEffectColor = light_ns.class_('StrobeLightEffectColor', LightEffect)
FlickerLightEffect = light_ns.class_('FlickerLightEffect', LightEffect)
AddressableLightEffect = light_ns.class_('AddressableLightEffect', LightEffect)
AddressableLambdaLightEffect = light_ns.class_('AddressableLambdaLightEffect',
                                               AddressableLightEffect)
AddressableRainbowLightEffect = light_ns.class_('AddressableRainbowLightEffect',
                                                AddressableLightEffect)
AddressableColorWipeEffect = light_ns.class_('AddressableColorWipeEffect', AddressableLightEffect)
AddressableColorWipeEffectColor = light_ns.struct('AddressableColorWipeEffectColor')
AddressableScanEffect = light_ns.class_('AddressableScanEffect', AddressableLightEffect)
AddressableTwinkleEffect = light_ns.class_('AddressableTwinkleEffect', AddressableLightEffect)
AddressableRandomTwinkleEffect = light_ns.class_('AddressableRandomTwinkleEffect',
                                                 AddressableLightEffect)
AddressableFireworksEffect = light_ns.class_('AddressableFireworksEffect', AddressableLightEffect)
AddressableFlickerEffect = light_ns.class_('AddressableFlickerEffect', AddressableLightEffect)


AddressableLambdaLightDisplayEffect = zilloscope_ns.class_('AddressableLambdaLightDisplayEffect')