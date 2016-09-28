import pymel.core as pm
from mtoa.ui.ae.shaderTemplate import ShaderAETemplate

class AEaiLightColorAttenuationTemplate(ShaderAETemplate):
    def setup(self):
        self.beginScrollLayout()
        self.beginLayout('Color Attenuation', collapse=False)

        self.addControl("aiColorStart", label="Start Color")
        self.addControl("aiColorEnd", label="End Color")
        self.addControl("aiInterpolationMethod", label="Interpolation Method [off = linear / on = hermite-cubic]")
        self.addControl("aiAttenuationStart", label="Attenuation Start")
        self.addControl("aiAttenuationEnd", label="Attenuation End")

        self.endLayout()
        pm.mel.AEdependNodeTemplate(self.nodeName)
        self.addExtraControls()
        self.endScrollLayout()
