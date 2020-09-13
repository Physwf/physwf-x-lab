#pragma once

// core/api.h*
#include "pbrt.h"

// API Function Declarations
void pbrtInit(const Options &opt);
void pbrtCleanup();
void pbrtIdentity();
void pbrtTranslate(Float dx, Float dy, Float dz);
void pbrtRotate(Float angle, Float ax, Float ay, Float az);
void pbrtScale(Float sx, Float sy, Float sz);
void pbrtLookAt(Float ex, Float ey, Float ez, Float lx, Float ly, Float lz,
	Float ux, Float uy, Float uz);
void pbrtConcatTransform(Float transform[16]);
void pbrtTransform(Float transform[16]);
void pbrtCoordinateSystem(const std::string &);
void pbrtCoordSysTransform(const std::string &);
void pbrtActiveTransformAll();
void pbrtActiveTransformEndTime();
void pbrtActiveTransformStartTime();
void pbrtTransformTimes(Float start, Float end);
void pbrtPixelFilter(const std::string &name, const ParamSet &params);
void pbrtFilm(const std::string &type, const ParamSet &params);
void pbrtSampler(const std::string &name, const ParamSet &params);
void pbrtAccelerator(const std::string &name, const ParamSet &params);
void pbrtIntegrator(const std::string &name, const ParamSet &params);
void pbrtCamera(const std::string &, const ParamSet &cameraParams);
void pbrtMakeNamedMedium(const std::string &name, const ParamSet &params);
void pbrtMediumInterface(const std::string &insideName,
	const std::string &outsideName);
void pbrtWorldBegin();
void pbrtAttributeBegin();
void pbrtAttributeEnd();
void pbrtTransformBegin();
void pbrtTransformEnd();
void pbrtTexture(const std::string &name, const std::string &type,
	const std::string &texname, const ParamSet &params);
void pbrtMaterial(const std::string &name, const ParamSet &params);
void pbrtMakeNamedMaterial(const std::string &name, const ParamSet &params);
void pbrtNamedMaterial(const std::string &name);
void pbrtLightSource(const std::string &name, const ParamSet &params);
void pbrtAreaLightSource(const std::string &name, const ParamSet &params);
void pbrtShape(const std::string &name, const ParamSet &params);
void pbrtReverseOrientation();
void pbrtObjectBegin(const std::string &name);
void pbrtObjectEnd();
void pbrtObjectInstance(const std::string &name);
void pbrtWorldEnd();

void pbrtParseFile(std::string filename);
void pbrtParseString(std::string str);
