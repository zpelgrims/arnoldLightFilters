#include <ai.h>
#include <strings.h>

/*

#define _colorStart                 (params[0].RGB)
#define _colorEnd                   (params[1].RGB)
#define _interpolationMethod        (params[2].BOOL)
#define _attenuationStart           (params[3].FLT)
#define _attenuationEnd             (params[4].FLT)

*/

enum lightColorAttenuationParams {
   p_colorStart,
   p_colorEnd,
   p_interpolationMethod,
   p_attenuationStart,
   p_attenuationEnd
};

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
    AtColor colorStart = AiShaderEvalParamRGB(p_colorStart);
    AtColor colorEnd = AiShaderEvalParamRGB(p_colorEnd);
    bool interpolationMethod = AiShaderEvalParamBool(p_interpolationMethod);
    float attenuationStart = AiShaderEvalParamFlt(p_attenuationStart);
    float attenuationEnd = AiShaderEvalParamFlt(p_attenuationEnd);


    if (sg->Ldist < attenuationStart){
        sg->Liu *= colorStart;
    } else if (sg->Ldist > attenuationEnd){
        sg->Liu *= colorEnd;
    } else {
       // calculate how far the shading point is in respect to the distance between attenuation start and attenuation end
       double percentage = ((sg->Ldist - attenuationStart) / (attenuationEnd - attenuationStart + 0.001)); //avoiding division by 0
       if (interpolationMethod == 0){
            sg->Liu *= AiColorLerp(percentage, colorStart, colorEnd);
       } else if (interpolationMethod == 1){
            sg->Liu *= AiColorHerp(percentage, colorStart, colorEnd);
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
