//View
#ifndef __UniformBuffer_View_Definition__
#define __UniformBuffer_View_Definition__

cbuffer View : register(b0)
{
    float4x4 View_TranslatedWorldToClip;
    float4x4 View_WorldToClip;
    float4x4 View_TranslatedWorldToView;
    float4x4 View_ViewToTranslatedWorld;
    float4x4 View_TranslatedWorldToCameraView;
    float4x4 View_CameraViewToTranslatedWorld;
    float4x4 View_ViewToClip;
    float4x4 View_ViewToClipNoAA;
    float4x4 View_ClipToView;
    float4x4 View_ClipToTranslatedWorld;
    float4x4 View_SVPositionToTranslatedWorld;
    float4x4 View_ScreenToWorld;
    float4x4 View_ScreenToTranslatedWorld;
    // half3 View_ViewForward;
    // half3 View_ViewUp;
    // half3 View_ViewRight;
    // half3 View_HMDViewNoRollUp;
    // half3 View_HMDViewNoRollRight;
    float4 View_InvDeviceZToWorldZTransform;
    // half4 View_ScreenPositionScaleBias;
    // float4 View_WorldCameraOrigin;
    // float4 View_TranslatedWorldCameraOrigin;
    // float4 View_WorldViewOrigin;


    float3 View_PreViewTranslation;
    //float View_Padding;
    float4 View_ViewRectMin;
    float4 View_ViewSizeAndInvSize;
};
static const struct
{
    float4x4 TranslatedWorldToClip;
    float4x4 WorldToClip;
    float4x4 TranslatedWorldToView;
    float4x4 ViewToTranslatedWorld;
    float4x4 TranslatedWorldToCameraView;
    float4x4 CameraViewToTranslatedWorld;
    float4x4 ViewToClip;
    float4x4 ViewToClipNoAA;
    float4x4 ClipToView;
    float4x4 ClipToTranslatedWorld;
    float4x4 SVPositionToTranslatedWorld;
    float4x4 ScreenToWorld;
    float4x4 ScreenToTranslatedWorld;
    // half3 ViewForward;
    // half3 ViewUp;
    // half3 ViewRight;
    // half3 HMDViewNoRollUp;
    // half3 HMDViewNoRollRight;
    float4 InvDeviceZToWorldZTransform;
    // half4 ScreenPositionScaleBias;
    // float4 WorldCameraOrigin;
    // float4 TranslatedWorldCameraOrigin;
    // float4 WorldViewOrigin;

    float3 PreViewTranslation;

    float4 ViewRectMin;
    float4 ViewSizeAndInvSize;
} View = 
{
    View_TranslatedWorldToClip,
    View_WorldToClip,
    View_TranslatedWorldToView,
    View_ViewToTranslatedWorld,
    View_TranslatedWorldToCameraView,
    View_CameraViewToTranslatedWorld,
    View_ViewToClip,
    View_ViewToClipNoAA,
    View_ClipToView,
    View_ClipToTranslatedWorld,
    View_SVPositionToTranslatedWorld,
    View_ScreenToWorld,
    View_ScreenToTranslatedWorld,
    // View_ViewForward,
    // View_ViewUp,
    // View_ViewRight,
    // View_HMDViewNoRollUp,
    // View_HMDViewNoRollRight,
    View_InvDeviceZToWorldZTransform,
    // View_ScreenPositionScaleBias,
    // View_WorldCameraOrigin,
    // View_TranslatedWorldCameraOrigin,
    // View_WorldViewOrigin,

    View_PreViewTranslation,

    View_ViewRectMin,
    View_ViewSizeAndInvSize
};
#endif
