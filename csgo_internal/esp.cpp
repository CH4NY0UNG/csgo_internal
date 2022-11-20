#include "inc.h"

void ESP()
{
	for (size_t i = 0; i < GetMaxPlayer(); i++)
	{
		Ent* me = *(Ent**)(clientdll + offsets::dwLocalPlayer);
		if (!me->IsValidPlayer()) continue;

		EntList* entList = (EntList*)(clientdll + offsets::dwEntityList);
		if (!entList) continue;

		Ent* player = entList->entBaseList[i].ent;
		if (!player->IsValidPlayer()) continue;

		//나 자신은 건너뛰고, 팀은 무시한다.
		if (me == player || me->Team == player->Team) continue;

		//ESP
		//거리 1500 = 1500cm = (150 * 10)m = 150m
		//팀원이 스팟을 찍었을 경우 스팟 찍힌 적은 거리 상관 없이 ESP 그리기
		if (Distance(me->Origin, player->Origin) > 1500 && player->SpottedByMask == 0) continue;

		//적이 보이는지 확인
		bool checkVisible = false; //상대를 공격할 수 있는지 확인
		for (size_t idx = 0; idx < boneList.size(); idx++)
		{
			if (IsVisible(me, player, boneList[idx]))
			{
				checkVisible = true;
				break;
			}
		}

		//벽뒤구분에 따라 ESP 색상 변경
		D3DCOLOR color = D3DCOLOR_ARGB(255, 120, 120, 120);
		if (checkVisible) color = D3DCOLOR_ARGB(255, 153, 51, 255);

		vec2 vScreen, vBone; //W2S으로 상대 발, 머리 위치를 2D좌표로 가져옴
		if (W2S(player->Origin, vScreen))
		{
			vec3 bonePos = player->GetBone();
			bonePos.z += 7; //얼굴 정중앙에 있는 위치를 임의로 머리 끝으로 수정
			if (W2S(bonePos, vBone))
			{
				//박스를 그리기 위해 높이, 넓이, 넓이 절반 값을 가져옴
				float head = vBone.y - vScreen.y;
				float width = head / 2;
				float center = width / -2; //Origin은 발 사이 정중앙 위치라 center 값을 가져온다

				//Box ESP
				int thick = 1; //박스 ESP가 더 잘보이게 하기 위해 테두리 두께를 저장
				D3DCOLOR backColor = D3DCOLOR_ARGB(255, 0, 0, 0); //테두리 색상 저장

				//바깥쪽 테두리
				Draw2DBox(vScreen.x + center - thick, vScreen.y - thick, width + thick * 2, head + thick * 2, 1, backColor);
				//박스 ESP - center를 x에 빼준다. 그럼 캐릭터 x: 왼쪽 어깨 y: 머리 끝 정도 위치로 값이 나온다
				//아래에서 +인 이유는 CSGO는 x, y가 반대로 움직임
				Draw2DBox(vScreen.x + center, vScreen.y, width, head, thick, color);
				//안쪽 테두리
				Draw2DBox(vScreen.x + center + thick, vScreen.y + thick, width - thick * 2, head - thick * 2, 1, backColor);

				//HealthBar
				int health = player->Health; //체력에 따라 체력바를 감소하기 위해 저장
				float hThick = 1; //테두리 두께 저장

				//ESP 박스 길이(가로 또는 세로) / 100 * health 가 기본 공식
				//아래는 테두리 두께까지 계산한거
				float hWidth = -((width - thick * 2) / 100 * health); //헬스바 넓이
				float hHeight = 5; //헬스바 높이
				float hx = vScreen.x - center; //헬스바 x 위치
				float hy = vScreen.y + head - hHeight - 1; //헬스바 y 위치

				//체력에 따라 색상 조절
				D3DCOLOR healthColor = D3DCOLOR_ARGB(255, 0, 255, 0);
				if (health <= 60 && health > 30) healthColor = D3DCOLOR_ARGB(255, 255, 128, 0);
				if (health <= 30) healthColor = D3DCOLOR_ARGB(255, 255, 0, 0);

				//체력바 그리기
				DrawFilled(hx + hThick, hy + hThick, hWidth - hThick * 2, hHeight - hThick * 2, healthColor);
				//체력바 테두리 그리기
				Draw2DBox(hx, hy - hThick, -width + thick * 2, hHeight, hThick, D3DCOLOR_ARGB(255, 32, 32, 32));
			}
		}
	}
}
