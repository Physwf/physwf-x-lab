#pragma once

#include "d3d12demo.h"

class TriangleDemo : public D3D12Demo
{
public:
	TriangleDemo(HWND hWnd) :D3D12Demo(hWnd) {}

protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
};

class TriangleDemoMSAA : public TriangleDemo
{
public:
	TriangleDemoMSAA(HWND hWnd) :TriangleDemo(hWnd) {}

protected:
	virtual void InitPipelineStates() override;
	virtual void Draw() override;
private:
	ComPtr<ID3D12Resource> m_SceneColorMSAA;
};