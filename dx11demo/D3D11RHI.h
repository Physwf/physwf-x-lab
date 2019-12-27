#include <dxgi.h>
#include <d3d11.h>

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
