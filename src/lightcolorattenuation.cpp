#include <ai.h>
#include <strings.h>

#define _colorStart                 (params[0].RGB)
#define _colorEnd                   (params[1].RGB)
#define _interpolationMethod        (params[2].BOOL)
#define _attenuationStart           (params[3].FLT)
#define _attenuationEnd             (params[4].FLT)

// IDEAS
// saturation
// color temperature
// change bool to enum

AI_SHADER_NODE_EXPORT_METHODS(lightColorAttenuationMethods)


node_parameters {
    AiParameterRGB("colorStart", 0.3, 1.0, 1.0);
    AiParameterRGB("colorEnd", 1.0, 0.3, 0.6);
    AiParameterBOOL("interpolationMethod", false);
    AiParameterFLT("attenuationStart", 55.0);
    AiParameterFLT("attenuationEnd", 105.0);
}


node_initialize {}
node_update {}
node_finish {}


shader_evaluate {
    // get values
    const AtParamValue* params = AiNodeGetParams(node);

    if (sg->Ldist < _attenuationStart){
        sg->Liu *= _colorStart;
    } else if (sg->Ldist > _attenuationEnd){
        sg->Liu *= _colorEnd;
    } else {
       // calculate how far the shading point is in respect to the distance between attenuation start and attenuation end
       double percentage = ((sg->Ldist - _attenuationStart) / (_attenuationEnd - _attenuationStart));
       if (_interpolationMethod == 0){
            sg->Liu *= AiColorLerp(percentage, _colorStart, _colorEnd);
       } else if (_interpolationMethod == 1){
            sg->Liu *= AiColorHerp(percentage, _colorStart, _colorEnd);
       }
    }
}


node_loader {
    if (i > 0)
        return false;
    node->methods     = lightColorAttenuationMethods;
    node->output_type = AI_TYPE_RGB;
    node->name        = "lightColorAttenuation";
    node->node_type   = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    return true;
}
