import pymel.core as pm
from mtoa.ui.ae.shaderTemplate import ShaderAETemplate

class AEaiLightColorAttenuationTemplate(ShaderAETemplate):
    def setup(self):
        self.beginScrollLayout()
        self.beginLayout('Color Attenuation', collapse=False)

        self.addControl("aiMode", label="Decay / Blocker")
        self.addControl("aiColorStart", label="Start Color")
        self.addControl("aiColorEnd", label="End Color")
        self.addControl("aiInterpolationMethod", label="Interpolation Method [off = linear / on = hermite-cubic]")
        self.addControl("aiAttenuationStart", label="Attenuation Start")
        self.addControl("aiAttenuationEnd", label="Attenuation End")

        self.addSeparator()

        self.addControl("aiGeo", label="Sphere / Cube")
        self.addControl("aiMath", label="Additive / Subtractive")

        self.addSeparator()

        self.addControl("aiLocatorLocation", label="Locator Location")
        self.addControl("aiScale", label="Blocker Scale")
        self.addControl("aiSphereRadius", label="Sphere Radius")

        self.addSeparator()

        self.addControl("aiColorAttenuation", label="Blocker Color Attenuation")
        self.addControl("aiColorDensity", label="Blocker Color Density")

        self.addSeparator()

        self.addControl("aiRampX", label="Ramp X Axis")
        self.addControl("aiFlipRampX", label="Flip X Axis")

        self.addControl("aiRampY", label="Ramp Y Axis")
        self.addControl("aiFlipRampY", label="Flip Y Axis")

        self.addControl("aiRampZ", label="Ramp Z Axis")
        self.addControl("aiFlipRampZ", label="Flip Z Axis")


        self.endLayout()
        pm.mel.AEdependNodeTemplate(self.nodeName)
        self.addExtraControls()
        self.endScrollLayout()
