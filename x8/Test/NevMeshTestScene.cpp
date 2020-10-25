#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <windows.h>

#include <stdio.h>
#include <stdarg.h>

#include "../NevMesh.h"

IDXGIFactory*	DXGIFactory = NULL;
IDXGIAdapter*	DXGIAdapter = NULL;
IDXGIOutput*	DXGIOutput = NULL;
IDXGISwapChain* DXGISwapChain = NULL;

ID3D11Device*			D3D11Device = NULL;
ID3D11DeviceContext*	D3D11DeviceContext = NULL;

ID3D11RenderTargetView* RenderTargetView = NULL;
ID3D11DepthStencilView* DepthStencialView = NULL;
ID3D11Texture2D* RenderTargetTexture = NULL;
ID3D11Texture2D* DepthStencialTexture = NULL;
D3D11_VIEWPORT Viewport;
ID3D11RasterizerState* RasterState;

LONG WindowWidth = 1920;
LONG WindowHeight = 1080;

HWND g_hWind;

#define X_LOG(Format,...) XLOG(Format, __VA_ARGS__)

inline void XLOG(const char* format, ...)
{
	char buffer[16 * 1024] = { 0 };
	va_list v_list;
	va_start(v_list, format);
	vsprintf_s(buffer, format, v_list);
	va_end(v_list);
	OutputDebugStringA(buffer);;
}

bool D3D11Setup()
{
	//create DXGIFactory
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&DXGIFactory);
	if (FAILED(hr))
	{
		X_LOG("CreateDXGIFactory failed!");
		return FALSE;
	}
	//EnumAapter
	for (UINT AdpaterIndex = 0; SUCCEEDED(DXGIFactory->EnumAdapters(AdpaterIndex, &DXGIAdapter)); ++AdpaterIndex)
	{
		if (DXGIAdapter)
		{
			DXGI_ADAPTER_DESC DESC;
			if (SUCCEEDED(DXGIAdapter->GetDesc(&DESC)))
			{
				X_LOG("Adapter %d: Description:%ls  VendorId:%x, DeviceId:0x%x, SubSysId:0x%x Revision:%u DedicatedVideoMemory:%uM DedicatedSystemMemory:%uM SharedSystemMemory:%uM \n",
					AdpaterIndex,
					DESC.Description,
					DESC.VendorId,
					DESC.DeviceId,
					DESC.SubSysId,
					DESC.Revision,
					DESC.DedicatedVideoMemory / (1024 * 1024),
					DESC.DedicatedSystemMemory / (1024 * 1024),
					DESC.SharedSystemMemory / (1024 * 1024)
				/*DESC.AdapterLuid*/);
			}

			for (UINT OutputIndex = 0; SUCCEEDED(DXGIAdapter->EnumOutputs(OutputIndex, &DXGIOutput)); ++OutputIndex)
			{
				if (DXGIOutput)
				{
					DXGI_OUTPUT_DESC OutDesc;
					if (SUCCEEDED(DXGIOutput->GetDesc(&OutDesc)))
					{
						X_LOG("\tOutput:%u, DeviceName:%ls, Width:%u Height:%u \n", OutputIndex, OutDesc.DeviceName, OutDesc.DesktopCoordinates.right - OutDesc.DesktopCoordinates.left, OutDesc.DesktopCoordinates.bottom - OutDesc.DesktopCoordinates.top);
					}
				}
			}
			break;
		}
	}

	//Create D3D11Device
	if (DXGIAdapter)
	{
		D3D_FEATURE_LEVEL FeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		D3D_FEATURE_LEVEL OutFeatureLevel;
		hr = D3D11CreateDevice(
			DXGIAdapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			FeatureLevels,
			6,
			D3D11_SDK_VERSION,
			&D3D11Device,
			&OutFeatureLevel,
			&D3D11DeviceContext
		);

		if (FAILED(hr))
		{
			X_LOG("D3D11CreateDevice failed!");
			return false;
		}

		UINT NumQualityLevels = 0;
		D3D11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &NumQualityLevels);

		extern HWND g_hWind;

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		ZeroMemory(&SwapChainDesc, sizeof SwapChainDesc);
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.BufferDesc.Width = WindowWidth;
		SwapChainDesc.BufferDesc.Height = WindowHeight;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.OutputWindow = g_hWind;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;// NumQualityLevels - 1;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		hr = DXGIFactory->CreateSwapChain(D3D11Device, &SwapChainDesc, &DXGISwapChain);
		if (FAILED(hr))
		{
			X_LOG("CreateSwapChain failed!");
			return false;
		}
	}

	hr = DXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture);
	if (FAILED(hr))
	{
		X_LOG("GetBuffer failed!");
		return false;
	}
	hr = D3D11Device->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
	if (FAILED(hr))
	{
		X_LOG("CreateRenderTargetView failed!");
		return false;
	}




	Viewport.Width = (FLOAT)WindowWidth;
	Viewport.Height = (FLOAT)WindowHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;

	D3D11DeviceContext->RSSetViewports(1, &Viewport);

	return true;
}

void D3D11ClearViewTarget()
{
	float ClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	D3D11DeviceContext->ClearDepthStencilView(DepthStencialView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void D3D11Present()
{
	DXGISwapChain->Present(0, 0);
}
/************************************************************************/
/* CreateScene                                                                
/************************************************************************/

void CreateScene(std::vector<Poly2D>& OutPolys, std::vector<Vector2D>& ClipedPoly, std::vector<std::vector<Vector2D>>& Clipers)
{
	Poly2D SceneBound = CreateRect(Vector2D(10.f, 10.f), Vector2D(WindowWidth - 10.f, WindowHeight - 10.f), 0.f);
	std::vector<Poly2D> Obstacles;
	OutPolys.push_back(SceneBound);
	Poly2D TriangleTemplate = CreateTriangle(Vector2D(100.0f, 0.0f), Vector2D(-100.0f, 0.0f), Vector2D(0.0f, 200.0f));
	int i = 0;
	while (i < 1)
	{
		i++;
		Matrix3x3 Rotation;
		Matrix3x3 Translation;
// 		float fRand = (rand() % 100) / 100.f;
// 		float fTransX = (rand() % 100) / 100.f;
// 		float fTransY = (rand() % 100) / 100.f;
		float fRand = 0.0f;
		float fTransX = (i % 5)/5.0f;
		float fTransY = (i / 5)*0.5;
		Rotation.Rotate(2 * 3.14f * fRand);
		Translation.Translate(fTransX * 1900.f + 10.f, fTransY * 1060.f + 200.f);
		Matrix3x3 Transform = Rotation * Translation;
		Poly2D Obstacle = CreateTriangle(Transform.Transform(TriangleTemplate.Vertices[0]), Transform.Transform(TriangleTemplate.Vertices[1]), Transform.Transform(TriangleTemplate.Vertices[2]));
		OutPolys.push_back(Obstacle);
		Obstacles.push_back(Obstacle);
	}

	for (Vector2D& V : SceneBound.Vertices)
	{
		ClipedPoly.push_back(V);
	}

	for (Poly2D& P : Obstacles)
	{
		std::vector<Vector2D> Vertices;
		for (Vector2D& V : P.Vertices)
		{
			Vertices.push_back(V);
		}
		Clipers.push_back(Vertices);
	}
}

struct LineSection
{
	size_t Start;
	size_t Count;
};

void CreateSceneVertices(const std::vector<Poly2D>& InputPolys, std::vector<Vector2D>& OutVertices, std::vector<size_t>& OutIndices, std::vector<LineSection>& Sections)
{
	for (const Poly2D& Poly : InputPolys)
	{
		LineSection Section = {0};
		Section.Start = OutVertices.size();
		for (const Vector2D& V : Poly.Vertices)
		{
			OutIndices.push_back(OutVertices.size());
			OutVertices.push_back(V);
			Section.Count++;
		}
		OutVertices.push_back(Poly.Vertices[0]);
		Section.Count++;
		Sections.push_back(Section);
	}
}

ID3D11Buffer* CreateVertexBuffer(const std::vector<Vector2D>& InVertices)
{
	D3D11_BUFFER_DESC VertexDesc;
	ZeroMemory(&VertexDesc, sizeof(VertexDesc));
	VertexDesc.ByteWidth = InVertices.size() * sizeof(Vector2D);
	VertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA InitData;
	{
		VertexDesc.Usage = D3D11_USAGE_DEFAULT;
		VertexDesc.CPUAccessFlags = 0;
		VertexDesc.MiscFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = InVertices.data();
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
	}

	ID3D11Buffer* Result;
	if (S_OK == D3D11Device->CreateBuffer(&VertexDesc, &InitData, &Result))
	{
		return Result;
	}
	X_LOG("CreateSceneVertexBuffer failed!");
	return NULL;
}

ID3D11Buffer* CreateIndexBuffer(std::vector<size_t>& InIndices)
{
	D3D11_BUFFER_DESC IndexDesc;
	ZeroMemory(&IndexDesc, sizeof(IndexDesc));
	IndexDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexDesc.ByteWidth = InIndices.size() * sizeof(size_t);
	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexDesc.CPUAccessFlags = 0;
	IndexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexData;
	ZeroMemory(&IndexData, sizeof(IndexData));
	IndexData.pSysMem = InIndices.data();
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	ID3D11Buffer* Result;
	if (S_OK == D3D11Device->CreateBuffer(&IndexDesc, &IndexData, &Result))
	{
		return Result;
	}
	X_LOG("CreateSceneIndexBuffer failed!");
	return NULL;
}

ID3D11Buffer* CreateConstantBuffer(void* Data, size_t Size)
{
	D3D11_BUFFER_DESC VertexDesc;
	ZeroMemory(&VertexDesc, sizeof(VertexDesc));
	VertexDesc.ByteWidth = Size;
	VertexDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA InitData;
	{
		VertexDesc.Usage = D3D11_USAGE_DEFAULT;
		VertexDesc.CPUAccessFlags = 0;
		VertexDesc.MiscFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = Data;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
	}

	ID3D11Buffer* Result;
	if (S_OK == D3D11Device->CreateBuffer(&VertexDesc, &InitData, &Result))
	{
		return Result;
	}
	X_LOG("CreateConstantBuffer failed!");
	return NULL;
}

ID3DBlob* CompileVertexShader(const wchar_t* File, const char* EntryPoint)
{
	ID3DBlob* Bytecode;
	ID3DBlob* OutErrorMsg;
	LPCSTR VSTarget = D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "vs_5_0" : "vs_4_0";
	UINT VSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;//*| D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY*/;
	D3D_SHADER_MACRO Macros[] =
	{
		NULL,NULL
	};
	if (S_OK == D3DCompileFromFile(File, Macros, NULL, EntryPoint, VSTarget, VSFlags, 0, &Bytecode, &OutErrorMsg))
	{
		return Bytecode;
	}
	X_LOG("D3DCompileFromFile failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
	return NULL;
}

ID3DBlob* CompilePixelShader(const wchar_t* File, const char* EntryPoint)
{
	ID3DBlob* Bytecode;
	ID3DBlob* OutErrorMsg;
	LPCSTR VSTarget = D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0";
	UINT VSFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;/*| D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY*/;
	D3D_SHADER_MACRO Macros[] =
	{
		NULL,NULL
	};
	if (S_OK == D3DCompileFromFile(File, Macros, NULL, EntryPoint, VSTarget, VSFlags, 0, &Bytecode, &OutErrorMsg))
	{
		return Bytecode;
	}
	X_LOG("D3DCompileFromFile failed! %s", (const char*)OutErrorMsg->GetBufferPointer());
	return NULL;
}

ID3D11VertexShader* CreateVertexShader(ID3DBlob* VSBytecode)
{
	ID3D11VertexShader* Result;
	if (S_OK == D3D11Device->CreateVertexShader(VSBytecode->GetBufferPointer(), VSBytecode->GetBufferSize(), NULL, &Result))
	{
		return Result;
	}

	X_LOG("CreateVertexShader failed!");
	return NULL;
}

ID3D11PixelShader* CreatePixelShader(ID3DBlob* PSBytecode)
{
	ID3D11PixelShader* Result;
	if (S_OK == D3D11Device->CreatePixelShader(PSBytecode->GetBufferPointer(), PSBytecode->GetBufferSize(), NULL, &Result))
	{
		return Result;
	}

	X_LOG("CreateVertexShader failed!");
	return NULL;
}
/************************************************************************/
/* WinMain                                                              
/************************************************************************/
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof WNDCLASSEX;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	RECT wr = { 0,0,WindowWidth,WindowHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWind = CreateWindowEx
	(
		NULL,
		L"WindowClass1",
		L"dx11demo",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(g_hWind, nCmdShow);
	if (!D3D11Setup())
	{
		return 1;
	}

	std::vector<Poly2D> Scene;
	std::vector<Vector2D> Meshes;
	std::vector<size_t> MeshIndeices;
	std::vector<Vector2D> ClipedPoly;
	std::vector<std::vector<Vector2D>> Clipers;
	CreateScene(Scene, ClipedPoly, Clipers);
	std::vector<std::vector<Vector2D>> ResultPolys;
	ClipPolygon(ClipedPoly, Clipers, ResultPolys);
	ConstructTriangleMesh(ResultPolys, Meshes, MeshIndeices);
	std::vector<Vector2D> Vertices;
	std::vector<size_t> Indices;
	std::vector<LineSection> Sections;
	CreateSceneVertices(Scene, Vertices, Indices, Sections);
	ID3D11Buffer* SceneVertexBuffer = CreateVertexBuffer(Vertices);
	ID3D11Buffer* SceneIndexBuffer = CreateIndexBuffer(Indices);
	Matrix4x4 WorldToClip = Matrix4x4::DXFromOrthognalLH(0, 1920, 0,1080,0, 100);
	WorldToClip.Transpose();
	//WorldToClip.SetIndentity();
	ID3D11Buffer* ViewConstant = CreateConstantBuffer(&WorldToClip,sizeof(WorldToClip));

	D3D11_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory(&DSDesc,sizeof(DSDesc));
	DSDesc.DepthEnable = FALSE;
	DSDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState* DSState;
	D3D11Device->CreateDepthStencilState(&DSDesc, &DSState);

	ID3D11InputLayout* SceneInputLayout;
	ID3D11VertexShader* SceneVertexShader;
	ID3D11PixelShader* ScenePixelShader;
	{
		ID3DBlob* Bytecode = CompileVertexShader(TEXT("SceneShader.fx"),"VS_Main");
		SceneVertexShader = CreateVertexShader(Bytecode);
		
		D3D11_INPUT_ELEMENT_DESC InputDesc[] =
		{
			{"ATTRIBUTE",0,	DXGI_FORMAT_R32G32_FLOAT,	0, 0,  D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		D3D11Device->CreateInputLayout(InputDesc,sizeof(InputDesc)/sizeof(D3D11_INPUT_ELEMENT_DESC), Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(),&SceneInputLayout);
		Bytecode->Release();
	}

	{
		ID3DBlob* Bytecode = CompilePixelShader(TEXT("SceneShader.fx"), "PS_Main");
		ScenePixelShader = CreatePixelShader(Bytecode);
		Bytecode->Release();
	}

	ID3D11Buffer* MeshVertexBuffer = CreateVertexBuffer(Meshes);
	ID3D11Buffer* MeshIndexBuffer = CreateIndexBuffer(MeshIndeices);
	ID3D11VertexShader* MeshVertexShader;
	ID3D11PixelShader* MeshPixelShader;
	{
		ID3DBlob* Bytecode = CompileVertexShader(TEXT("MeshShader.fx"), "VS_Main");
		MeshVertexShader = CreateVertexShader(Bytecode);
	}
	{
		ID3DBlob* Bytecode = CompilePixelShader(TEXT("MeshShader.fx"), "PS_Main");
		MeshPixelShader = CreatePixelShader(Bytecode);
	}
	ID3D11RasterizerState* WireframeMode;
	{
		D3D11_RASTERIZER_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.CullMode = D3D11_CULL_NONE;
		Desc.FillMode = D3D11_FILL_WIREFRAME;
		D3D11Device->CreateRasterizerState(&Desc,&WireframeMode);
	}

	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}

			float ClearColor[] = { 0.1f,0.1f, 0.1f, 1.0f };
			D3D11DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
			D3D11DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
			D3D11DeviceContext->OMSetDepthStencilState(DSState, 0);
			D3D11DeviceContext->IASetIndexBuffer(SceneIndexBuffer, DXGI_FORMAT_R32_UINT,0);
			UINT Stride = sizeof(Vector2D);
			UINT Offset = 0;
			D3D11DeviceContext->IASetVertexBuffers(0, 1, &SceneVertexBuffer, &Stride, &Offset);
			D3D11DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
			D3D11DeviceContext->IASetInputLayout(SceneInputLayout);

			D3D11DeviceContext->VSSetConstantBuffers(0,1, &ViewConstant);
			D3D11DeviceContext->VSSetShader(SceneVertexShader, 0, 0);
			D3D11DeviceContext->PSSetShader(ScenePixelShader, 0, 0);

			for (const LineSection& Section : Sections)
			{
				//D3D11DeviceContext->DrawIndexed(Section.Count - 1, Section.Start, 0);
				D3D11DeviceContext->Draw(Section.Count, Section.Start);
			}

			D3D11DeviceContext->RSSetState(WireframeMode);

			D3D11DeviceContext->IASetIndexBuffer(MeshIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			D3D11DeviceContext->IASetVertexBuffers(0, 1, &MeshVertexBuffer, &Stride, &Offset);
			D3D11DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			D3D11DeviceContext->VSSetShader(MeshVertexShader, 0, 0);
			D3D11DeviceContext->PSSetShader(MeshPixelShader, 0, 0);

			D3D11DeviceContext->DrawIndexed(MeshIndeices.size(), 0, 0);

			D3D11Present();
		}
		Sleep(10);
	}


	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		X_LOG("WM_KEYDOWN\n");
		break;
	}
	case WM_KEYUP:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
		break;
	}
	case WM_LBUTTONUP:
	{
		break;
	}
	case WM_RBUTTONDOWN:
	{
		break;
	}
	case WM_RBUTTONUP:
	{
		break;
	}
	case WM_MOUSEMOVE:
	{
		break;
	}
	}

	return	DefWindowProc(hWnd, message, wParam, lParam);
}
