#include <ai.h>
#include <cstring>
 
enum lightColorAttenuationParams {
   p_colorStart,
   p_colorEnd,
   p_attenuationStart,
   p_attenuationEnd
};
 
 
AI_SHADER_NODE_EXPORT_METHODS(lightColorAttenuationMethods)
 
 
node_parameters {
    AiParameterRGB("colorStart", 0.3, 1.0, 1.0);
    AiParameterRGB("colorEnd", 1.0, 0.3, 0.6);
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
    float attenuationStart = AiShaderEvalParamFlt(p_attenuationStart);
    float attenuationEnd = AiShaderEvalParamFlt(p_attenuationEnd);
 
 
    if (sg->Ldist < attenuationStart){
        sg->Liu *= colorStart;
    } else if (sg->Ldist > attenuationEnd){
        sg->Liu *= colorEnd;
    } else {
       double percentage = ((sg->Ldist - attenuationStart) / (attenuationEnd - attenuationStart + 0.001));
       sg->Liu *= AiColorLerp(percentage, colorStart, colorEnd);
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