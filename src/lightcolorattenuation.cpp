#include <ai.h>
#include <string>
#include <cstring>
#include <vector>

// TODO
// controlable ramps for spheres
// find light name, plug into warning
// ramps maya viewport representation

/* to compile and kick on my machine
g++ -std=c++11 -O3 -I"C:/ilionData/Users/zeno.pelgrims/Desktop/Arnold-4.2.12.0-windows/include" -L"C:/ilionData/Users/zeno.pelgrims/Desktop/Arnold-4.2.12.0-windows/bin" -lai --shared C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/src/lightColorAttenuation.cpp -o C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/bin/lightColorAttenuation.dll
C:/ilionData/Users/zeno.pelgrims/Desktop/Arnold-4.2.13.4-windows/bin/kick -l "C:\ilionData\Users\zeno.pelgrims\Documents\lightfilter\bin" -set options.skip_license_check on -i "C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/lightfilter_own_02.ass" -g 2.2 -v 2 -l "C:/Program Files/Ilion/IlionMayaFramework/2015/modules/mtoa/1.2.7.2.2-4.2.13.6/shaders" -o "C:/ilionData/Users/zeno.pelgrims/Documents/lightfilter/lightColorAttenuation.exr" -dp
*/


AI_SHADER_NODE_EXPORT_METHODS(lightColorAttenuationMethods);


enum lightColorAttenuationParams{
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
    p_doubleRamp_x,
    p_rampY,
    p_flipRampY,
    p_doubleRamp_y,
    p_rampZ,
    p_flipRampZ,
    p_doubleRamp_z
};


struct lightBlockers{
    std::vector<std::string> lightFilterNames;
    std::vector<float> locatorLocation;
    std::vector<float> colorDensity;
    std::vector<float> scale;
    std::vector<float> rampX;
    std::vector<float> rampY;
    std::vector<float> rampZ;
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
    AiParameterBOOL("doubleRamp_x", false);
    AiParameterFLT("rampY", 0.0);
    AiParameterBOOL("flipRampY", false);
    AiParameterBOOL("doubleRamp_y", false);
    AiParameterFLT("rampZ", 0.0);
    AiParameterBOOL("flipRampZ", false);
    AiParameterBOOL("doubleRamp_z", false);
}

node_initialize {
    // chuck all light filter nodes into vector for use in multiple negative blockers
    bool negative = AiNodeGetBool(node, "math");

    if(negative){
        std::string nodeName = std::string(AiNodeGetName(node));
        AiMsgInfo("nodename = %s", nodeName.c_str());
        lightblockers.lightFilterNames.push_back(nodeName);

        AtPoint locatorLocation = AiNodeGetPnt(node, "locatorLocation");
        lightblockers.locatorLocation.push_back(locatorLocation.x);
        lightblockers.locatorLocation.push_back(locatorLocation.y);
        lightblockers.locatorLocation.push_back(locatorLocation.z);

        AtVector scale = AiNodeGetVec(node, "scale"); // might have linking issues (not evalParam..)
        lightblockers.scale.push_back(scale.x);
        lightblockers.scale.push_back(scale.y);
        lightblockers.scale.push_back(scale.z);

        float colorDensity = AiNodeGetFlt(node, "colorDensity"); // might have linking issues (not evalParam..)
        lightblockers.colorDensity.push_back(colorDensity);

        float rampX = AiNodeGetFlt(node, "rampX"); // might have linking issues (not evalParam..)
        float rampY = AiNodeGetFlt(node, "rampY"); // might have linking issues (not evalParam..)
        float rampZ = AiNodeGetFlt(node, "rampZ"); // might have linking issues (not evalParam..)
        lightblockers.rampX.push_back(rampX);
        lightblockers.rampY.push_back(rampY);
        lightblockers.rampZ.push_back(rampZ);
    }
}


node_update {
    bool negative = AiNodeGetBool(node, "math");

    if(negative){
        // check if node is already in vector (multiple negative blockers)
        bool nodeNameInVector = false;
        std::string nodeName = std::string(AiNodeGetName(node));
        for (int i = 0; i < lightblockers.lightFilterNames.size(); i++){
            if (nodeName == lightblockers.lightFilterNames[i]){
                nodeNameInVector = true;
            }
        }

        if (nodeNameInVector == false){
            lightblockers.lightFilterNames.push_back(nodeName);

            AtPoint locatorLocation = AiNodeGetPnt(node, "locatorLocation"); // might have linking issues (not evalParam..)
            lightblockers.locatorLocation.push_back(locatorLocation.x);
            lightblockers.locatorLocation.push_back(locatorLocation.y);
            lightblockers.locatorLocation.push_back(locatorLocation.z);

            AtVector scale = AiNodeGetVec(node, "scale"); // might have linking issues (not evalParam..)
            lightblockers.scale.push_back(scale.x);
            lightblockers.scale.push_back(scale.y);
            lightblockers.scale.push_back(scale.z);

            float colorDensity = AiNodeGetFlt(node, "colorDensity"); // might have linking issues (not evalParam..)
            lightblockers.colorDensity.push_back(colorDensity);

            float rampX = AiNodeGetFlt(node, "rampX"); // might have linking issues (not evalParam..)
            float rampY = AiNodeGetFlt(node, "rampY"); // might have linking issues (not evalParam..)
            float rampZ = AiNodeGetFlt(node, "rampZ"); // might have linking issues (not evalParam..)
            lightblockers.rampX.push_back(rampX);
            lightblockers.rampY.push_back(rampY);
            lightblockers.rampZ.push_back(rampZ);
        }

        if (lightblockers.lightFilterNames.size() > 1){
            AiMsgWarning("More than one negative blocker for [TODO: INSERT LIGHT NAME]");
            AiMsgWarning("Summing color attenuations of blockers:");
            for (int i = 0; i < lightblockers.lightFilterNames.size(); i++){
                AiMsgWarning("\t %s", lightblockers.lightFilterNames[i].c_str());
            }
        }
    }
}

node_finish {
    if(lightblockers.lightFilterNames.size() > 0){
        AiMsgInfo("Negative blockers: [%d]", lightblockers.lightFilterNames.size());

        for (int i = 0; i < lightblockers.lightFilterNames.size(); i++){
            AiMsgInfo("Name: %s", lightblockers.lightFilterNames[i].c_str());

            AiMsgInfo("Coordinates: [%f, %f, %f]", lightblockers.locatorLocation[i*3],
                                                   lightblockers.locatorLocation[i*3+1],
                                                   lightblockers.locatorLocation[i*3+2]);

            AiMsgInfo("Scale: [%f, %f, %f]", lightblockers.scale[i*3],
                                             lightblockers.scale[i*3+1],
                                             lightblockers.scale[i*3+2]);
        }
    }

    AiMsgInfo("Clearing ilionLightBlocker data");
    lightblockers.lightFilterNames.clear();
    lightblockers.locatorLocation.clear();
    lightblockers.scale.clear();
    lightblockers.colorDensity.clear();
    lightblockers.rampX.clear();
    lightblockers.rampY.clear();
    lightblockers.rampZ.clear();
}

shader_evaluate{
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
    bool doubleRamp_x = AiShaderEvalParamBool(p_doubleRamp_x);

    float rampY = AiShaderEvalParamFlt(p_rampY);
    bool flipRampY = AiShaderEvalParamBool(p_flipRampY);
    bool doubleRamp_y = AiShaderEvalParamBool(p_doubleRamp_y);

    float rampZ = AiShaderEvalParamFlt(p_rampZ);
    bool flipRampZ = AiShaderEvalParamBool(p_flipRampZ);
    bool doubleRamp_z = AiShaderEvalParamBool(p_doubleRamp_z);

    bool sphereRamp = true;

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

                // equation seems to be correct math wise but isn't working. Possibly because ellipsiod equation?
                float sphereRampPercentage =
                      ((ABS(sg->P.x - locatorLocation.x) / (ABS(scale.x))) *
                       (ABS(sg->P.x - locatorLocation.x) / (ABS(scale.x))) +
                       (ABS(sg->P.y - locatorLocation.y) / (ABS(scale.y))) *
                       (ABS(sg->P.y - locatorLocation.y) / (ABS(scale.y))) +
                       (ABS(sg->P.z - locatorLocation.z) / (ABS(scale.z))) *
                       (ABS(sg->P.z - locatorLocation.z) / (ABS(scale.z))));

                //if (sphereRampPercentage < 0.0){
                //    sphereRampPercentage = 0.0;
                //}

                //if (sphereRampPercentage > 1.0){
                //    sphereRampPercentage = 1.0;
                //}

                if (sphereRampPercentage < 1.0){
                    //if (sphereRamp){
                    //    sg->Liu = AiColorLerp((1.0 - sphereRampPercentage) * colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    //} else {
                        sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    //}
                }

            } else {

                if (ABS((sg->P.x - locatorLocation.x)) < ABS(scale.x) &&
                    ABS((sg->P.y - locatorLocation.y)) < ABS(scale.y) &&
                    ABS((sg->P.z - locatorLocation.z)) < ABS(scale.z)){

                    double rampxPercentage = 1.0;
                    double rampyPercentage = 1.0;
                    double rampzPercentage = 1.0;

                    if (rampX > 0.0){
                        if (doubleRamp_x){
                            rampxPercentage = ((sg->P.x - (locatorLocation.x - scale.x)) * ABS((sg->P.x - (locatorLocation.x + scale.x)))) / (4.0 * scale.x * scale.x * rampX * rampX);
                        } else if (!flipRampX){
                            rampxPercentage = (sg->P.x - (locatorLocation.x - scale.x)) / (2.0 * scale.x * rampX);
                        } else {
                            rampxPercentage = ABS((sg->P.x - (locatorLocation.x + scale.x)) / (2.0 * scale.x * rampX));
                        }

                        if (rampxPercentage > 1.0){rampxPercentage = 1.0;}
                    }

                    if (rampY > 0.0){
                        if (doubleRamp_y){
                           rampyPercentage = ((sg->P.y - (locatorLocation.y - scale.y)) * ABS((sg->P.y - (locatorLocation.y + scale.y)))) / (4.0 * scale.y * scale.y * rampY * rampY);
                        } else if (!flipRampY){
                            rampyPercentage = (sg->P.y - (locatorLocation.y - scale.y)) / (2.0 * scale.y * rampY);
                        } else {
                            rampyPercentage = ABS((sg->P.y - (locatorLocation.y + scale.y)) / (2.0 * scale.y * rampY));
                        }

                        if (rampyPercentage > 1.0){rampyPercentage = 1.0;}
                    }

                    if (rampZ > 0.0){
                        if (doubleRamp_z){
                            rampzPercentage = ((sg->P.z - (locatorLocation.z - scale.z)) * ABS((sg->P.z - (locatorLocation.z + scale.z)))) / (4.0 * scale.z * scale.z * rampZ * rampZ);
                        } else if (!flipRampZ){
                            rampzPercentage = (sg->P.z - (locatorLocation.z - scale.z)) / (2.0 * scale.z * rampZ);
                        } else {
                            rampzPercentage = ABS((sg->P.z - (locatorLocation.z + scale.z)) / (2.0 * scale.z * rampZ));
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

            if (colorDensity > 1.0){
                sg->Liu *= colorDensity;
            }

        }

        // SUBTRACTIVE MATH
        else if (math){
            bool insideGeometry = false;

            if(!geo){
                float sphereRampPercentage;

                for(int i = 0; i < lightblockers.lightFilterNames.size(); i++){

                    AtVector locatorLocation2 = {lightblockers.locatorLocation[i*3],
                                                 lightblockers.locatorLocation[i*3+1],
                                                 lightblockers.locatorLocation[i*3+2]};

                    AtVector scale2 = {lightblockers.scale[i*3],
                                       lightblockers.scale[i*3+1],
                                       lightblockers.scale[i*3+2]};

                    sphereRampPercentage =
                       ((ABS((sg->P.x - locatorLocation2.x)) / ABS(scale2.x)) * (ABS((sg->P.x - locatorLocation2.x)) / ABS(scale2.x)) +
                        (ABS((sg->P.y - locatorLocation2.y)) / ABS(scale2.y)) * (ABS((sg->P.y - locatorLocation2.y)) / ABS(scale2.y)) +
                        (ABS((sg->P.z - locatorLocation2.z)) / ABS(scale2.z)) * (ABS((sg->P.z - locatorLocation2.z)) / ABS(scale2.z)));

                    if (sphereRampPercentage < 1.0){
                        insideGeometry = true;
                    }
                }

                if(!insideGeometry){
                    //if (sphereRamp){
                    //    sg->Liu = AiColorLerp((1.0 - sphereRampPercentage) * colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    //} else {
                        sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                    //}
                }
            }
            else {

                double rampxPercentage = 1.0;
                double rampyPercentage = 1.0;
                double rampzPercentage = 1.0;

                for(int i = 0; i < lightblockers.lightFilterNames.size(); i++){

                    AtVector locatorLocation2 = {lightblockers.locatorLocation[i*3],
                                                 lightblockers.locatorLocation[i*3+1],
                                                 lightblockers.locatorLocation[i*3+2]};

                    AtVector scale2 = {lightblockers.scale[i*3],
                                       lightblockers.scale[i*3+1],
                                       lightblockers.scale[i*3+2]};

                    if (ABS((sg->P.x - locatorLocation2.x)) < ABS(scale2.x) &&
                        ABS((sg->P.y - locatorLocation2.y)) < ABS(scale2.y) &&
                        ABS((sg->P.z - locatorLocation2.z)) < ABS(scale2.z)){


                        if (rampX > 0.0){
                            if (doubleRamp_x){
                                rampxPercentage = ((sg->P.x - (locatorLocation2.x - scale2.x)) * ABS((sg->P.x - (locatorLocation2.x + scale2.x)))) / (4.0 * scale2.x * scale2.x * lightblockers.rampX[i] * lightblockers.rampX[i]);
                            } else if (flipRampX){
                                rampxPercentage = (sg->P.x - (locatorLocation2.x - scale2.x)) / (2.0 * scale2.x * lightblockers.rampX[i]);
                            } else {
                                rampxPercentage = ABS((sg->P.x - (locatorLocation2.x + scale2.x)) / (2.0 * scale2.x * lightblockers.rampX[i]));
                            }

                            if (rampxPercentage > 1.0){rampxPercentage = 1.0;}
                        }

                        if (rampY > 0.0){
                            if (doubleRamp_y){
                                rampyPercentage = ((sg->P.y - (locatorLocation2.y - scale2.y)) * ABS((sg->P.y - (locatorLocation2.y + scale2.y)))) / (4.0 * scale2.y * scale2.y * lightblockers.rampY[i] * lightblockers.rampY[i]);
                            } else if (flipRampY){
                                rampyPercentage = (sg->P.y - (locatorLocation2.y - scale2.y)) / (2.0 * scale2.y * lightblockers.rampY[i]);
                            } else {
                                rampyPercentage = ABS((sg->P.y - (locatorLocation2.y + scale2.y)) / (2.0 * scale2.y * lightblockers.rampY[i]));
                            }

                            if (rampyPercentage > 1.0){rampyPercentage = 1.0;}
                        }

                        if (rampZ > 0.0){
                            if (doubleRamp_z){
                                rampzPercentage = ((sg->P.z - (locatorLocation2.z - scale2.z)) * ABS((sg->P.z - (locatorLocation2.z + scale2.z)))) / (4.0 * scale2.z * scale2.z * lightblockers.rampZ[i] * lightblockers.rampZ[i]);
                            } else if (flipRampZ){
                                rampzPercentage = (sg->P.z - (locatorLocation2.z - scale2.z)) / (2.0 * scale2.z * lightblockers.rampZ[i]);
                            } else {
                                rampzPercentage = ABS((sg->P.z - (locatorLocation2.z + scale2.z)) / (2.0 * scale2.z * lightblockers.rampZ[i]));
                            }

                            if (rampzPercentage > 1.0){rampzPercentage = 1.0;}
                        }

                        insideGeometry = true;
                    }
                }


                if (rampX > 0.0 || rampY > 0.0 || rampZ > 0.0){
                    sg->Liu = AiColorLerp((1.0 - rampxPercentage * rampyPercentage * rampzPercentage) * colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                } else {
                    sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
                }


                if (!insideGeometry){
                    sg->Liu = AiColorLerp(colorDensity, sg->Liu, sg->Liu * colorAttenuation);
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
