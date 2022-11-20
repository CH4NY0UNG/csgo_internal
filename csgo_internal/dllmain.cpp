#include "inc.h"

void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;

typedef void* (__cdecl* tCreateInterface)(const char* name, int* pReturnCode);
void* GetInterface(tCreateInterface fn, const char* name)
{
	return (fn)(name, 0);
}

tTraceRay TraceRay;
tCreateInterface CreateInterface;

void ShowConsole()
{
	if (AllocConsole())
	{
		FILE* input;
		freopen_s(&input, "CONIN$", "rb", stdin);
		freopen_s(&input, "CONOUT$", "wb", stdout);
		freopen_s(&input, "CONOUT$", "wb", stderr);
		ios::sync_with_stdio();
	}
}

HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 dummyDevice)
{
	if (!pDevice) pDevice = dummyDevice;
	if (!pLine) D3DXCreateLine(pDevice, &pLine);
	if (!pMenuFont) D3DXCreateFontA(pDevice, 20, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Calibri", &pMenuFont);

	if (ShowMenu)
	{
		DrawFilled(5, 5, 120, 200, D3DCOLOR_ARGB(255, 31, 31, 31));
		DrawString((char*)"AIM [PGUP] ON", 12, 30, D3DCOLOR_ARGB(255, 0, 255, 0));
		DrawString((char*)"ESP [PGDN] ON", 12, 10, D3DCOLOR_ARGB(255, 0, 255, 0));
	}

	ESP();
	if (LI_FN(GetAsyncKeyState).cached()(VK_XBUTTON2) & 0x8000) Aimbot();

	return oEndScene(pDevice);
}

DWORD WINAPI MainForm(HMODULE hModule)
{
	while (!(clientdll && enginedll))
	{
		clientdll = (uintptr_t)GetModuleHandle(XOR("client.dll"));
		enginedll = (uintptr_t)GetModuleHandle(XOR("engine.dll"));
		Sleep(1);
	}

	CreateInterface = (tCreateInterface)GetProcAddress((HMODULE)enginedll, XOR("CreateInterface"));
	EngineTrace = (IEngineTrace*)GetInterface(CreateInterface, XOR("EngineTraceClient004"));

	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
	{
		LI_FN(memcpy).cached()(EndSceneBytes, (char*)d3d9Device[42], 7);
		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}

	while (!LI_FN(GetAsyncKeyState).cached()(VK_END))
	{
		if (GetAsyncKeyState(VK_INSERT) & 1) ShowMenu = !ShowMenu;
		Sleep(1);
	}

	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7); //Unhook;
	FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainForm, hModule, 0, 0));
			break;
		}
	}

	return TRUE;
}