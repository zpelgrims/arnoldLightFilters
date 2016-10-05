#include <ai.h>
#include <iostream>
#include <string>
#include <strings.h>
#include <algorithm>
#include <vector>
#include <iterator>
#include <stdlib.h>


enum lightColorAttenuationParams {
   p_colorStart,
   p_colorEnd,
   p_interpolationMethod,
   p_attenuationStart,
   p_attenuationEnd,
   p_locatorLocation,
   p_negativeColor
};


struct lightBlockers{
    std::vector<std::string> lightFilterNames;
    std::vector<float> locatorLocation;
    std::vector<AtColor> color;
    std::vector<float> sphereRadius;
} lb;



AI_SHADER_NODE_EXPORT_METHODS(lightColorAttenuationMethods);


node_parameters {
    AiParameterRGB("colorStart", 0.3, 1.0, 1.0);
    AiParameterRGB("colorEnd", 1.0, 0.3, 0.6);
    AiParameterBOOL("interpolationMethod", false);
    AiParameterFLT("attenuationStart", 55.0);
    AiParameterFLT("attenuationEnd", 105.0);
    AiParameterPNT("locatorLocation", 0.0, 0.0, 0.0);
    AiParameterRGB("negativeColor", 0.0, 1.0, 1.0);
}

node_initialize {
    lb.lightFilterNames.clear();
    lb.locatorLocation.clear();
}


node_update {
    lb.lightFilterNames.clear();
    lb.locatorLocation.clear();
}

node_finish {
    AiMsgInfo("vector size [%d]", lb.lightFilterNames.size());
    for (int i = 0; i < lb.lightFilterNames.size(); i++){
       AiMsgInfo("vector element [%d] = %s", i, lb.lightFilterNames[i].c_str());
       AiMsgInfo("vector element [%d] = [%f, %f, %f]", i, lb.locatorLocation[i*3], lb.locatorLocation[i*3+1], lb.locatorLocation[i*3+2]);
    }


    lb.lightFilterNames.clear();
    lb.locatorLocation.clear();
}

shader_evaluate{

    // get values
    AtColor colorStart = AiShaderEvalParamRGB(p_colorStart);
    AtColor colorEnd = AiShaderEvalParamRGB(p_colorEnd);
    bool interpolationMethod = AiShaderEvalParamBool(p_interpolationMethod);
    float attenuationStart = AiShaderEvalParamFlt(p_attenuationStart);
    float attenuationEnd = AiShaderEvalParamFlt(p_attenuationEnd);
    AtPoint locatorLocation = AiShaderEvalParamPnt(p_locatorLocation);
    AtColor negativeColor = AiShaderEvalParamRGB(p_negativeColor);

    std::string nodeName = std::string(AiNodeGetName(node));

    bool decayBlockerSwitch = 1;
    bool additiveSubtractiveSwitch = 0;
    bool geometrySwitch = 1;

    float colorDensity = 0.0;
    float inverseColorDensity = (1.0 - colorDensity);

    double sphereRadius = 15.0;
    AtPoint cubeScale = {15.0, 15.0, 15.0};

    float rampX = 0.2;
    float rampY = 0.0;
    float rampZ = 0.2;

    bool flipRampX = true;
    bool flipRampY = false;
    bool flipRampZ = true;


    if (decayBlockerSwitch == 0){
        if (sg->Ldist < attenuationStart){
            sg->Liu *= colorStart;
        } else if (sg->Ldist > attenuationEnd){
            sg->Liu *= colorEnd;
        } else {
            // calculate how far the shading point is in respect to the distance between attenuation start and attenuation end
            double percentage = ((sg->Ldist - attenuationStart) / (attenuationEnd - attenuationStart));
            if (interpolationMethod == 0){
                           sg->Liu *= AiColorLerp(percentage, colorStart, colorEnd);
            } else if (interpolationMethod == 1){
                           sg->Liu *= AiColorHerp(percentage, colorStart, colorEnd);
            }
        }
    }

    else if (decayBlockerSwitch == 1){
        // ADDITIVE MODE
        if (additiveSubtractiveSwitch == 0){
            if (geometrySwitch == 0){
               if (AiV3Dist(sg->P, locatorLocation) < sphereRadius){
                              sg->Liu = AiColorLerp(inverseColorDensity, sg->Liu, sg->Liu * colorEnd);
               }
            } else {
                if (ABS((sg->P.x - locatorLocation.x)) > ABS(cubeScale.x) ||
                    ABS((sg->P.y - locatorLocation.y)) > ABS(cubeScale.y) ||
                    ABS((sg->P.z - locatorLocation.z)) > ABS(cubeScale.z)){
                                  // do nothing
                } else {
                  double rampxPercentage = 1.0;
                  double rampyPercentage = 1.0;
                  double rampzPercentage = 1.0;

                if (rampX > 0.0){
                    if (!flipRampX){
                        rampxPercentage = ((sg->P.x - (locatorLocation.x - cubeScale.x)) / (((locatorLocation.x + cubeScale.x) - (locatorLocation.x - cubeScale.x)) * rampX));
                    } else {
                        rampxPercentage = ((sg->P.x - (locatorLocation.x + cubeScale.x)) / (((locatorLocation.x + cubeScale.x) - (locatorLocation.x - cubeScale.x)) * rampX));
                        rampxPercentage = ABS(rampxPercentage);
                    }

                    if (rampxPercentage > 1.0){
                        rampxPercentage = 1.0;
                    }
                }

                if (rampY > 0.0){
                    if (!flipRampY){
                        rampyPercentage = ((sg->P.y - (locatorLocation.y - cubeScale.y)) / (((locatorLocation.y + cubeScale.y) - (locatorLocation.y - cubeScale.y)) * rampY));
                    } else {
                        rampyPercentage = ((sg->P.y - (locatorLocation.y + cubeScale.y)) / (((locatorLocation.y + cubeScale.y) - (locatorLocation.y - cubeScale.y)) * rampY));
                        rampyPercentage = ABS(rampyPercentage);
                    }

                    if (rampyPercentage > 1.0){
                         rampyPercentage = 1.0;
                    }
                }

                if (rampZ > 0.0){
                    if (!flipRampZ){
                         rampzPercentage = ((sg->P.z - (locatorLocation.z - cubeScale.z)) / (((locatorLocation.z + cubeScale.z) - (locatorLocation.z - cubeScale.z)) * rampZ));
                    } else {
                      rampzPercentage = ((sg->P.z - (locatorLocation.z + cubeScale.z)) / (((locatorLocation.z + cubeScale.z) - (locatorLocation.z - cubeScale.z)) * rampZ));
                      rampzPercentage = ABS(rampzPercentage);
                    }

                    if (rampzPercentage > 1.0){
                        rampzPercentage = 1.0;
                    }
                }

                if (rampX > 0.0 || rampY > 0.0 || rampZ > 0.0){
                    sg->Liu = AiColorLerp(rampxPercentage * rampyPercentage * rampzPercentage * inverseColorDensity, sg->Liu, sg->Liu * colorEnd);
                }

                }

            }
        }

        // SUBTRACTIVE MODE
        // sometimes more than 2 filters are picked up, not sure why... then it all goes phithongelong
        else if (additiveSubtractiveSwitch == 1){
            bool inRadius = false;
            bool foundName = false;
            if (lb.lightFilterNames.size() == 0){
                lb.lightFilterNames.push_back(nodeName);
                lb.locatorLocation.push_back(locatorLocation.x);
                lb.locatorLocation.push_back(locatorLocation.y);
                lb.locatorLocation.push_back(locatorLocation.z);
            } else {
                for (int i = 0; i < lb.lightFilterNames.size(); i++){
                    if (nodeName == lb.lightFilterNames[i]){
                        foundName = true;
                    }
                }

                if (foundName == false){
                    lb.lightFilterNames.push_back(nodeName);
                    lb.locatorLocation.push_back(locatorLocation.x);
                    lb.locatorLocation.push_back(locatorLocation.y);
                    lb.locatorLocation.push_back(locatorLocation.z);
                } else {
                    for(int i = 0; i < lb.lightFilterNames.size(); i++){
                        AtVector locatorLocation2 = {lb.locatorLocation[i*3], lb.locatorLocation[i*3+1], lb.locatorLocation[i*3+2]};
                        if (AiV3Dist(sg->P, locatorLocation2) < sphereRadius){
                            inRadius = true;
                        }
                    }

                    if (inRadius != true){
                        sg->Liu = AiColorLerp(inverseColorDensity, sg->Liu, sg->Liu * negativeColor);
                    }
                }
            }
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
