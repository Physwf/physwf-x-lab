#ifndef __SceneTexturesCommon_H__
#define __SceneTexturesCommon_H__

float CalcSceneDepth(float2 ScreenUV)
{
    float DeviceZ = SceneTexturesStruct.SceneDepthTexture.SampleLevel( SceneTexturesStruct.SceneDepthTextureSampler,ScreenUV,0).r;
    return ConvertFromDeviceZ(DeviceZ);
}


#endif