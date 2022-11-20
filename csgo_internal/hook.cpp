#include "inc.h"

BOOL CALLBACK enumWind(HWND handle, LPARAM lp) {
	DWORD procId;
	GetWindowThreadProcessId(handle, &procId);
	if (GetCurrentProcessId() != procId)
		return TRUE;

	window = handle;
	return FALSE;
}

HWND GetProcessWindow()
{
	window = NULL;
	EnumWindows(enumWind, NULL);

	return window;
}

bool GetD3D9Device(void** pTable, size_t Size)
{
	if (!pTable)
		return false;

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return false;

	IDirect3DDevice9* pDummyDevice = NULL;

	// options to create dummy device
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();

	HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDeviceCreated != S_OK)
	{
		// may fail in windowed fullscreen mode, trying again with windowed mode
		d3dpp.Windowed = !d3dpp.Windowed;

		dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

		if (dummyDeviceCreated != S_OK)
		{
			pD3D->Release();
			return false;
		}
	}

	LI_FN(memcpy).cached()(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

	pDummyDevice->Release();
	pD3D->Release();
	return true;
}

bool Hook(char* src, char* dst, int len)
{
	if (len < 5) return false;

	DWORD curProtection;

	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(src, 0x90, len);

	uintptr_t relativeAddress = (uintptr_t)(dst - src - 5);

	*src = (char)0xE9;
	*(uintptr_t*)(src + 1) = (uintptr_t)relativeAddress;

	DWORD temp;
	VirtualProtect(src, len, curProtection, &temp);

	return true;
}

char* TrampHook(char* src, char* dst, unsigned int len)
{
	if (len < 5) return 0;

	// Create the gateway (len + 5 for the overwritten bytes + the jmp)
	char* gateway = (char*)VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Put the bytes that will be overwritten in the gateway
	memcpy(gateway, src, len);

	// Get the gateway to destination addy
	uintptr_t gateJmpAddy = (uintptr_t)(src - gateway - 5);

	// Add the jmp opcode to the end of the gateway
	*(gateway + len) = (char)0xE9;

	// Add the address to the jmp
	*(uintptr_t*)(gateway + len + 1) = gateJmpAddy;

	// Place the hook at the destination
	if (Hook(src, dst, len))
	{
		return gateway;
	}
	else return nullptr;
}

void Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oProc;
	LI_FN(VirtualProtect).cached()(dst, size, PAGE_EXECUTE_READWRITE, &oProc);
	LI_FN(memcpy).cached()(dst, src, size);
	LI_FN(VirtualProtect).cached()(dst, size, oProc, &oProc);
}