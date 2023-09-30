#include<windows.h>
#include<d3d11.h>
#include<d3dcompiler.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

static WCHAR WNDCLASS_NAME[] = L"wndclass";

struct vertex { float X, Y, Z; };
static vertex TRIANGLE_VERTS[3] =
{
  { 0.0f, 0.5f, 0.0f, },
  { 0.5f, -0.5f, 0.0f, },
  { -0.5f, -0.5f, 0.0f, },
};

int WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE Unused, WCHAR* CmdLine, int CmdShow)
{
	WNDCLASSEXW WindowClass = {0};
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.lpfnWndProc = DefWindowProcW;
	WindowClass.lpszClassName = WNDCLASS_NAME;
	RegisterClassExW(&WindowClass);
	HWND Window = CreateWindowExW(0, WNDCLASS_NAME, 0, WS_MAXIMIZE | WS_VISIBLE | WS_POPUP, 0, 0, 0, 0, 0, 0, 0, 0);

	RECT WindowRect;
	GetClientRect(Window, &WindowRect);
	LONG WindowWidth = WindowRect.right - WindowRect.left;
	LONG WindowHeight = WindowRect.bottom - WindowRect.top;

	ID3D11Device* Device = 0;
	ID3D11DeviceContext* DeviceContext = 0;
	IDXGISwapChain* SwapChain = 0;
	DXGI_SWAP_CHAIN_DESC SwapChainInfo = {0};
	SwapChainInfo.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainInfo.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainInfo.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	SwapChainInfo.SampleDesc.Count = 1;
	SwapChainInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainInfo.BufferCount = 2;
	SwapChainInfo.OutputWindow = Window;
	SwapChainInfo.Windowed = TRUE;
	SwapChainInfo.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_SINGLETHREADED, 0, 0, D3D11_SDK_VERSION, &SwapChainInfo, &SwapChain, &Device, 0, &DeviceContext);

	ID3D11Texture2D* Framebuffer = 0;
	ID3D11RenderTargetView* FramebufferView = 0;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&Framebuffer);
	Device->CreateRenderTargetView(Framebuffer, 0, &FramebufferView);
	DeviceContext->OMSetRenderTargets(1, &FramebufferView, 0);

	D3D11_VIEWPORT Viewport = {0};
	Viewport.Width = (FLOAT)WindowWidth;
	Viewport.Height = (FLOAT)WindowHeight;
	Viewport.MaxDepth = 1;
	DeviceContext->RSSetViewports(1, &Viewport);

	ID3DBlob* ShaderBlob = 0;
	ID3D11VertexShader* VertexShader = 0;
	D3DCompileFromFile(L"shaders.hlsl", 0, 0, "vs_main", "vs_5_0", 0, 0, &ShaderBlob, 0);
	Device->CreateVertexShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), 0, &VertexShader);
	DeviceContext->VSSetShader(VertexShader, 0, 0);

	ID3D11InputLayout* InputLayout = 0;
	D3D11_INPUT_ELEMENT_DESC InputElementDesc = {0};
	InputElementDesc.SemanticName = "POS";
	InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	Device->CreateInputLayout(&InputElementDesc, 1, ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), &InputLayout);
	DeviceContext->IASetInputLayout(InputLayout);

	ID3D11PixelShader* PixelShader = 0;
	D3DCompileFromFile(L"shaders.hlsl", 0, 0, "ps_main", "ps_5_0", 0, 0, &ShaderBlob, 0);
	Device->CreatePixelShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), 0, &PixelShader);
	DeviceContext->PSSetShader(PixelShader, 0, 0);

	ID3D11Buffer* VertexBuffer = 0;
	D3D11_SUBRESOURCE_DATA VertexResource = {0};
	D3D11_BUFFER_DESC VertexBufferDesc = {0};
	VertexBufferDesc.ByteWidth = sizeof(TRIANGLE_VERTS);
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexResource.pSysMem = TRIANGLE_VERTS;
	Device->CreateBuffer(&VertexBufferDesc, &VertexResource, &VertexBuffer);

	UINT VERTEX_OFFSET = 0;
	UINT VERTEX_STRIDE = sizeof(vertex);
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VERTEX_STRIDE, &VERTEX_OFFSET);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	FLOAT BackgroundColor[4] = { 0.15f, 0.15f, 0.15f, 1.0f };

	for(;;)
	{
		MSG Message;
		while(PeekMessageW(&Message, 0, 0, 0, PM_REMOVE))
		{
			if(Message.message == WM_KEYDOWN)
			{
				goto label_program_exit;
			}
			DispatchMessageW(&Message);
		}
		DeviceContext->ClearRenderTargetView(FramebufferView, BackgroundColor);
		DeviceContext->Draw(3, 0);
		SwapChain->Present(1, 0);
	}
label_program_exit:
	return 0;
}
