#pragma once

extern void VerifyD3D11Result(HRESULT Result, const ANSICHAR* Code, const ANSICHAR* FileName, uint32 Line, ID3D11Device* Device);

extern void VerifyD3D11ShaderResult();

#define VERIFYD3D11RESULT_EX(x,Device) {HRESULT hr = x; if(FAILED(hr)) {VerifyD3D11Result(hr,#x,__FILE__,__LINE__,Device);}}
#define VERIFYD3D11RESULT(x) {HRESULT hr = x; if(FAILED(hr)) {VerifyD3D11Result(hr,#x,__FILE__,__LINE__,0);}}