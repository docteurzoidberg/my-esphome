import esphome.codegen as cg

# Base
light_ns = cg.esphome_ns.namespace('light')
display_ns = cg.esphome_ns.namespace('display')
zilloscope_ns = cg.esphome_ns.namespace('zilloscope')

Display = display_ns.class_('Display', cg.Component)

DisplayLambdaEffect = zilloscope_ns.class_('DisplayLambdaEffect')
DisplayFireEffect =  zilloscope_ns.class_('DisplayFireEffect')
DisplayBubblesEffect =  zilloscope_ns.class_('DisplayBubblesEffect')
DisplaySnowEffect =  zilloscope_ns.class_('DisplaySnowEffect')
DisplayMatrixEffect =  zilloscope_ns.class_('DisplayMatrixEffect')
DisplayTiledPuzzleEffect =  zilloscope_ns.class_('DisplayTiledPuzzleEffect')

Mode = zilloscope_ns.class_('Mode')
ModeTime = zilloscope_ns.class_('ModeTime')
ModeLambda = zilloscope_ns.class_('ModeLambda')
ModeEffects = zilloscope_ns.class_('ModeEffects')
ModePaint = zilloscope_ns.class_('ModePaint')
ModeMeteo = zilloscope_ns.class_('ModeMeteo')

ESPColor = light_ns.class_('ESPColor')


