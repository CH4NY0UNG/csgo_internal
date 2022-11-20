#pragma once

inline LPDIRECT3DDEVICE9 pDevice;
inline ID3DXLine* pLine;
inline ID3DXFont* pMenuFont;

void DrawString(char* string, int x, int y, D3DCOLOR color);
void DrawFilled(float x, float y, float w, float h, D3DCOLOR color);
void DrawLine(float x1, float y1, float x2, float y2, float thickness, D3DCOLOR color);
void DrawLine(vec2 p, vec2 p2, float thickness, D3DCOLOR color);
//void Draw2DBox(vec2 top, vec2 bot, int thickness, D3DCOLOR color);
void Draw2DBox(int x, int y, int w, int h, int thickness, D3DCOLOR color);