#ifndef __SceneTexturesCommon_H__
#define __SceneTexturesCommon_H__


float3 CalcSceneColor(float2 ScreenUV)
{
// #if SCENE_TEXTURES_DISABLED
// 	return float3(0.0f,0.0f,0.0f);
// #else
	return Texture2DSampleLevel(SceneTexturesStruct.SceneColorTexture, SceneTexturesStruct.SceneColorTextureSampler, ScreenUV, 0).rgb;
//#endif
}

float CalcSceneDepth(float2 ScreenUV)
{
    float DeviceZ = SceneTexturesStruct.SceneDepthTexture.SampleLevel( SceneTexturesStruct.SceneDepthTextureSampler,ScreenUV,0).r;
    return ConvertFromDeviceZ(DeviceZ);
}


#endif