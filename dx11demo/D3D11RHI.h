#include <dxgi.h>
#include <d3d11.h>
#include <wrl/client.h>

extern IDXGIFactory*	DXGIFactory;
extern IDXGIAdapter*	DXGIAdapter;
extern IDXGIOutput*	DXGIOutput;
extern IDXGISwapChain* DXGISwapChain;

extern ID3D11Device*			D3D11Device;
extern ID3D11DeviceContext*	D3D11DeviceContext;

extern ID3D11RenderTargetView* RenderTargetView;
extern ID3D11DepthStencilView* DepthStencialView;
extern D3D11_VIEWPORT Viewport;
extern ID3D11RasterizerState* RasterState;
extern ID3D11Texture2D* DepthStencialTexture;

extern LONG WindowWidth;
extern LONG WindowHeight;

bool D3D11Setup();
void D3D11ClearViewTarget();
void D3D11Present();

ID3D11Buffer* CreateVertexBuffer(void* Data,unsigned int Size);
ID3D11Buffer* CreateIndexBuffer(void* Data, unsigned int Size);
ID3D11Buffer* CreateConstantBuffer(void* Data, unsigned int Size);
ID3DBlob* CompileVertexShader(const wchar_t* File, const char* EntryPoint);
ID3DBlob* CompilePixelShader(const wchar_t* File, const char* EntryPoint);
ID3D11VertexShader* CreateVertexShader(ID3DBlob* VSBytecode);
ID3D11PixelShader* CreatePixelShader(ID3DBlob* PSBytecode);
ID3D11InputLayout* CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* InputDesc, unsigned int Count, ID3DBlob* VSBytecode);
