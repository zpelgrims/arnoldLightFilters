// EXTENDED ARNOLD LIGHT FILTER

// FEATURES:
// COLOR ATTENUATION
// MULTIPLE NEGATIVE BLOCKERS
// MULTIPLE RAMPS

#include <ai.h>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>


AI_SHADER_NODE_EXPORT_METHODS(lightColorAttenuationMethods);


enum lightColorAttenuationParams {
    p_mode,
    p_colorStart,
    p_colorEnd,
    p_interpolationMethod,
    p_attenuationStart,
    p_attenuationEnd,
    p_geo,
    p_math,
    p_locatorLocation,
    p_scale,
    p_sphereRadius,
    p_colorAttenuation,
    p_colorDensity,
    p_rampX,
    p_flipRampX,
    p_rampY,
    p_flipRampY,
    p_rampZ,
    p_flipRampZ
};


struct lightBlockers{
    std::vector<std::string> lightFilterNames;
    std::vector<float> locatorLocation;
    std::vector<float> sphereRadius;
} lightblockers;



node_parameters {
    AiParameterBOOL("mode", true);
    AiParameterRGB("colorStart", 1.0, 1.0, 1.0);
    AiParameterRGB("colorEnd", 1.0, 1.0, 1.0);
    AiParameterBOOL("interpolationMethod", false);
    AiParameterFLT("attenuationStart", 0.001);
    AiParameterFLT("attenuationEnd", 10.0);
    AiParameterBOOL("geo", false);
    AiParameterBOOL("math", false);
    AiParameterPNT("locatorLocation", 0.0, 0.0, 0.0);
    AiParameterVEC("scale", 1.0, 1.0, 1.0);
    AiParameterFLT("sphereRadius", 1.0);
    AiParameterRGB("colorAttenuation", 1.0, 1.0, 1.0);
    AiParameterFLT("colorDensity", 1.0);
    AiParameterFLT("rampX", 0.0);
    AiParameterBOOL("flipRampX", false);
    AiParameterFLT("rampY", 0.0);
    AiParameterBOOL("flipRampY", false);
    AiParameterFLT("rampZ", 0.0);
    AiParameterBOOL("flipRampZ", false);
}

node_initialize {}


node_update {
    lightblockers.lightFilterNames.clear();
    lightblockers.locatorLocation.clear();
}

node_finish {
    AiMsgInfo("vector size [%lu]", lightblockers.lightFilterNames.size());
    for (int i = 0; i < lightblockers.lightFilterNames.size(); i++){
    AiMsgInfo("vector element [%d] = %s", i, lightblockers.lightFilterNames[i].c_str());
    AiMsgInfo("vector element [%d] = [%f, %f, %f]", i, lightblockers.locatorLocation[i*3], lightblockers.locatorLocation[i*3+1], lightblockers.locatorLocation[i*3+2]);
    }

    lightblockers.lightFilterNames.clear();
    lightblockers.locatorLocation.clear();
}

shader_evaluate{
    // get values
    bool mode = AiShaderEvalParamBool(p_mode);

    AtColor colorStart = AiShaderEvalParamRGB(p_colorStart);
    AtColor colorEnd = AiShaderEvalParamRGB(p_colorEnd);
    bool interpolationMethod = AiShaderEvalParamBool(p_interpolationMethod);
    float attenuationStart = AiShaderEvalParamFlt(p_attenuationStart);
    float attenuationEnd = AiShaderEvalParamFlt(p_attenuationEnd);

    bool geo = AiShaderEvalParamBool(p_geo);
    bool math = AiShaderEvalParamBool(p_math);

    AtPoint locatorLocation = AiShaderEvalParamPnt(p_locatorLocation);
    AtVector scale = AiShaderEvalParamVec(p_scale);
    float sphereRadius = AiShaderEvalParamFlt(p_sphereRadius);

    AtColor colorAttenuation = AiShaderEvalParamRGB(p_colorAttenuation);
    float colorDensity = AiShaderEvalParamFlt(p_colorDensity);

    float rampX = AiShaderEvalParamFlt(p_rampX);
    bool flipRampX = AiShaderEvalParamBool(p_flipRampX);

    float rampY = AiShaderEvalParamFlt(p_rampY);
    bool flipRampY = AiShaderEvalParamBool(p_flipRampY);

    float rampZ = AiShaderEvalParamFlt(p_rampZ);
    bool flipRampZ = AiShaderEvalParamBool(p_flipRampZ);


    std::string nodeName = std::string(AiNodeGetName(node));

    //float sphereRamp = 0.3;


    // DECAY
    if (!mode){
        if (sg->Ldist < attenuationStart){
            sg->Liu *= colorStart;
        } else if (sg->Ldist > attenuationEnd){
            sg->Liu *= colorEnd;
        } else {
            double percentage = ((sg->Ldist - attenuationStart) / (attenuationEnd - attenuationStart));
            if (interpolationMethod == 0){
                sg->Liu *= AiColorLerp(percentage, colorStart, colorEnd);
            } else if (interpolationMethod == 1){
                sg->Liu *= AiColorHerp(percentage, colorStart, colorEnd);
            }
        }
    }

    // BLOCKER
    else if (mode){
        // ADDITIVE MATH
        if (!math){
            if (!geo){
                if (AiV3Dist(sg->P, locatorLocation) < sphereRadius){
                    //double sphereRampPercentage;
                    //sphereRampPercentage = AiV3Dist(sg->P, locatorLocation) / sphereRamp;
                    //if (sphereRampPercentage > 1.0){
                    //            sphereRampPercentage = 1.0;
                    //}
                    //sg->Liu = AiColorLerp(sphereRampPercentage * colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                }
                } else {
                    if (ABS((sg->P.x - locatorLocation.x)) > ABS(scale.x) ||
                    ABS((sg->P.y - locatorLocation.y)) > ABS(scale.y) ||
                    ABS((sg->P.z - locatorLocation.z)) > ABS(scale.z)){
                    // do nothing
                } else {

                    double rampxPercentage = 1.0;
                    double rampyPercentage = 1.0;
                    double rampzPercentage = 1.0;

                    if (rampX > 0.0){
                        if (!flipRampX){
                            rampxPercentage = ((sg->P.x - (locatorLocation.x - scale.x)) / (((locatorLocation.x + scale.x) - (locatorLocation.x - scale.x)) * rampX));
                        } else {
                            rampxPercentage = ABS(((sg->P.x - (locatorLocation.x + scale.x)) / (((locatorLocation.x + scale.x) - (locatorLocation.x - scale.x)) * rampX)));
                        }

                        if (rampxPercentage > 1.0){rampxPercentage = 1.0;}
                    }

                    if (rampY > 0.0){
                        if (!flipRampY){
                            rampyPercentage = ((sg->P.y - (locatorLocation.y - scale.y)) / (((locatorLocation.y + scale.y) - (locatorLocation.y - scale.y)) * rampY));
                        } else {
                            rampyPercentage = ABS(((sg->P.y - (locatorLocation.y + scale.y)) / (((locatorLocation.y + scale.y) - (locatorLocation.y - scale.y)) * rampY)));
                        }

                        if (rampyPercentage > 1.0){rampyPercentage = 1.0;}
                    }

                    if (rampZ > 0.0){
                        if (!flipRampZ){
                            rampzPercentage = ((sg->P.z - (locatorLocation.z - scale.z)) / (((locatorLocation.z + scale.z) - (locatorLocation.z - scale.z)) * rampZ));
                        } else {
                        rampzPercentage = ABS(((sg->P.z - (locatorLocation.z + scale.z)) / (((locatorLocation.z + scale.z) - (locatorLocation.z - scale.z)) * rampZ)));
                        }

                        if (rampzPercentage > 1.0){rampzPercentage = 1.0;}
                    }

                    if (rampX > 0.0 || rampY > 0.0 || rampZ > 0.0){
                        sg->Liu = AiColorLerp(rampxPercentage * rampyPercentage * rampzPercentage * colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    } else {
                        sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    }

                }

            }
        }

        // SUBTRACTIVE MATH
        // sometimes more than 2 filters are picked up, not sure why... then it all goes phithongelong
        else if (math){
            bool inRadius = false;
            bool foundName = false;
            if (lightblockers.lightFilterNames.size() == 0){
                lightblockers.lightFilterNames.push_back(nodeName);
                lightblockers.locatorLocation.push_back(locatorLocation.x);
                lightblockers.locatorLocation.push_back(locatorLocation.y);
                lightblockers.locatorLocation.push_back(locatorLocation.z);
            } else {
                for (int i = 0; i < lightblockers.lightFilterNames.size(); i++){
                    if (nodeName == lightblockers.lightFilterNames[i]){
                        foundName = true;
                    }
                }

                if (!foundName){
                    lightblockers.lightFilterNames.push_back(nodeName);
                    lightblockers.locatorLocation.push_back(locatorLocation.x);
                    lightblockers.locatorLocation.push_back(locatorLocation.y);
                    lightblockers.locatorLocation.push_back(locatorLocation.z);
                } else {
                    for(int i = 0; i < lightblockers.lightFilterNames.size(); i++){
                    AtVector locatorLocation2 = {lightblockers.locatorLocation[i*3], lightblockers.locatorLocation[i*3+1], lightblockers.locatorLocation[i*3+2]};
                    if (AiV3Dist(sg->P, locatorLocation2) < sphereRadius){
                        inRadius = true;
                    }
                }

                if (!inRadius){
                    sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                }
            }
        }
    }
}

}

node_loader {
    if (i > 0){return false;}
    node->methods     = lightColorAttenuationMethods;
    node->output_type = AI_TYPE_RGB;
    node->name        = "lightColorAttenuation";
    node->node_type   = AI_NODE_SHADER;
    strcpy(node->version, AI_VERSION);
    return true;
}
