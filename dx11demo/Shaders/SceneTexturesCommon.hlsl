
float4 CalcSceneDepth(float2 ScreenUV)
{
    float DeviceZ = SceneTexturesStruct.SceneDepthTexture.SampleLevel( SceneTexturesStruct.SceneDepthTextureSampler,ScreenUV,0).r;
    return ConvertFromDeviceZ(DeviceZ);
}
