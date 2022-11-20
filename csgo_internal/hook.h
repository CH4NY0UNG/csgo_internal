#pragma once

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
static HWND window;
HWND GetProcessWindow();
bool GetD3D9Device(void** pTable, size_t Size);
bool Hook(char* src, char* dst, int len);
char* TrampHook(char* src, char* dst, unsigned int len);
void Patch(BYTE* dst, BYTE* src, unsigned int size);