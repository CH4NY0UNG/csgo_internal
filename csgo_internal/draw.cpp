#include "inc.h"

void DrawString(char* string, int x, int y, D3DCOLOR color)
{
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	pMenuFont->DrawTextA(0, string, strlen(string), &FontPos, DT_NOCLIP, color);
}

void DrawFilled(float x, float y, float w, float h, D3DCOLOR color)
{
	D3DRECT rect = { x, y, x + w, y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawLine(float x1, float y1, float x2, float y2, float thickness, D3DCOLOR color)
{
	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	pLine->SetWidth(thickness);
	pLine->Draw(Line, 2, color);
}

void DrawLine(vec2 src, vec2 dst, float thickness, D3DCOLOR color)
{
	DrawLine(src.x, src.y, dst.x, dst.y, thickness, color);
}

void Draw2DBox(int x, int y, int w, int h, int thickness, D3DCOLOR color)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + w, y);
	points[2] = D3DXVECTOR2(x + w, y + h);
	points[3] = D3DXVECTOR2(x, y + h);
	points[4] = D3DXVECTOR2(x, y);
	pLine->SetWidth(thickness);
	pLine->Draw(points, 5, color);
}