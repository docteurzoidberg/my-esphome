import esphome.codegen as cg

# Base
light_ns = cg.esphome_ns.namespace('light')
display_ns = cg.esphome_ns.namespace('display')
zilloscope_ns = cg.esphome_ns.namespace('zilloscope')

DisplayBuffer = display_ns.class_('DisplayBuffer', cg.Component)
DisplayBufferRef = DisplayBuffer.operator('ref')

DisplayLambdaEffect = zilloscope_ns.class_('DisplayLambdaEffect')
DisplayFireEffect =  zilloscope_ns.class_('DisplayFireEffect')
DisplayBubblesEffect =  zilloscope_ns.class_('DisplayBubblesEffect')
DisplayMatrixEffect =  zilloscope_ns.class_('DisplayMatrixEffect')
DisplayTiledPuzzleEffect =  zilloscope_ns.class_('DisplayTiledPuzzleEffect')

Mode = zilloscope_ns.class_('Mode')
ModeTime = zilloscope_ns.class_('ModeTime')
ModeLambda = zilloscope_ns.class_('ModeLambda')
ModeEffects = zilloscope_ns.class_('ModeEffects')
ModePaint = zilloscope_ns.class_('ModePaint')

ESPColor = light_ns.class_('ESPColor')


