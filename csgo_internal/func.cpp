#include "inc.h"

void SetCheat()
{
	Ent* target = NULL;
	float fov_old = FLT_MAX;
	RECT rect = GetClientSize(XOR("Valve001"), 0);
	int midX = rect.right / 2; //클라이언트 정중앙 X
	int midY = rect.bottom / 2; //클라이언트 정중앙 Y
	float multX;
	float size;
	vec3* Angle;
	vec3 diffs;

	//Aimbot, ESP를 따로 모듈화하지 않은 이유는 for문 32번을 루핑할 동안 모든 것을 처리하기 위함
	//EndScene를 후킹했기 때문에 프레임 별 작업을 최소화 한 것임 (프레임 드랍을 최소화하고 지연을 최소화)
	//코드가 간단하기 때문에 모듈화해도 큰 차이는 없었으나 소스량이 많아지면 고려해봐야할 사항
	for (size_t i = 0; i < 32; i++) //GetMaxPlayer()를 사용하면 항상 64 고정이기 때문에 그냥 최대 인원 32로 고정 (위와 동일하게 2배 차이는 무시 못함)
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

		//적이 보이는지 확인 (ESP, 에임봇)
		int boneID = 8; //어떤 뼈가 보였는지 저장
		bool checkVisible = false; //상대를 공격할 수 있는지 확인
		//머리부터 발쪽으로 스캔후 가장 빨리 찾은 뼈을 저장
		for (size_t idx = 0; idx < boneList.size(); idx++)
		{
			if (IsVisible(me, player, boneList[idx]))
			{
				boneID = boneList[idx];
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
				//박스 ESP - center를 x에 빼준다. 그럼 캐릭터 왼쪽 발쯤 위치로 x값이 나온다
				//아래에서 +인 이유는 vScreen이 음수이기 때문에
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
				if(health <= 60 && health > 30) healthColor = D3DCOLOR_ARGB(255, 255, 128, 0);
				if(health <= 30) healthColor = D3DCOLOR_ARGB(255, 255, 0, 0);

				//체력바 그리기
				DrawFilled(hx + hThick, hy + hThick, hWidth - hThick * 2, hHeight - hThick * 2, healthColor);
				//체력바 테두리 그리기
				Draw2DBox(hx, hy - hThick, -width + thick * 2, hHeight, hThick, D3DCOLOR_ARGB(255, 32, 32, 32));
			}
		}

		//Aimbot
		//위에서 저장한 벽뒤구분이 false라면 continue
		if (!checkVisible) continue;
		multX = midX / me->FOV; //클라이언트 (테두리 없는) 정중앙 X 위치 / 캐릭터 시야
		size = (FOV * multX); //내가 설정한 FOV 값 * multX

		//타겟 설정
		vec3 bonePos = player->GetBone(boneID); //위에서 저장한 보이는 뼈 위치 저장
		vec2 vAngle;
		if (W2S(bonePos, vAngle))
		{
			//클라이언트 정중앙 x, y좌표와 상대 위치까지의 거리 계산 (2D 좌표)
			float fovAngle = sqrt((midX - vAngle.x) * (midX - vAngle.x) + (midY - vAngle.y) * (midY - vAngle.y));

			//내가 설정한 FOV 범위 내에 있는지 확인
			//FOV내에 여러명이 있을 경우 2D좌표상 가장 가까운 적을 타겟으로 지정
			if (fovAngle < size && fovAngle < fov_old)
			{
				vec3 myPos = me->Origin + me->ViewOffset; //자신의 캐릭터 눈 위치
				vec3 veloc = player->Velocity; //적의 이동속도를 가져옴
				veloc.x /= 12; //CSGO는 탄도학 개념이 없기 때문에
				veloc.y /= 12; //적당히 캐릭터 피격 범위 내에서만
				veloc.z /= 12; //예측하게 임의로 값을 낮춰줌 또 아예 없으면 에임이 뒤쳐짐
				vec3 predict = bonePos + veloc; //뼈위치 + 이동속도
				vec3 delta = CalcReverseAngle(myPos, predict); //내 에임(앵글)이 이동해야할 위치 계산
				diffs = delta - *GetMyAngle(); //현재 내 앵글과 에임(앵글)이 이동해야할 위치를 빼준다. (에임을 인간 처럼 보이기 위한 빌드업)
				target = player; //타겟을 저장
				fov_old = fovAngle; //FOV내에 적이 여러명이면 가장 가까운 적을 선별하기 위해 min 값을 계산
			}
		}
	} //엔티티 for문 종료

	//핫키가 눌려있다면 타겟으로 마우스 이동
	if (LI_FN(GetAsyncKeyState).cached()(VK_XBUTTON2) & 0x8000)
	{
		Ent* me = *(Ent**)(clientdll + offsets::dwLocalPlayer);
		if (me->IsValidPlayer() && target && fov_old != FLT_MAX)
		{
			vec3* Angle = GetMyAngle();
			float distSmooth = Distance(me->Origin, target->Origin);

			//스나이퍼 같은 총기류일 때 스코프 모드인지 확인한다.
			//스코프 모드일 때는 적과 에임이 매우 가까워지므로 더 많이 스무스하게 만들어준다.
			if (InRange(me->FOV, 90, 91) && distSmooth > 800) distSmooth = 5;
			if (InRange(me->FOV, 90, 91) && distSmooth <= 800) distSmooth = 2;

			if (InRange(me->FOV, 40, 90) && distSmooth <= 400) distSmooth = 5;
			if (InRange(me->FOV, 40, 90) && distSmooth > 400) distSmooth = 2;

			if (InRange(me->FOV, 0, 20) && distSmooth <= 200) distSmooth = 5;
			if (InRange(me->FOV, 0, 40) && distSmooth > 200) distSmooth = 2;

			//반동제어 (Anti Recoil)
			vec3 punchAngle = me->PunchAngle * 2; //반동이 움직인 앵글 값 * 2를 저장
			int fire = me->IsShotFire; //내 캐릭터가 총을 쏘고 있는지 확인
			bool IsRecoil = false; //반동을 제어해야하는지 확인
			if (fire >= 1) //1발 이상 쐈다면
			{
				//내 앵글 + 이전에 움직인 반동 - (지금 움직인 반동 * 2)
				//쉽게 생각하면 *Angle + oPunch는 총을 쐈을 때 에임이 올라간 상태의 값이다.
				//반동에 의해 올라간 에임의 위치에서
				//현재 반동에 2배를 내려주면서 흔히 부르는 상탄 조절을 하게 된다.
				vec3 newAngle = *Angle + oPunch - punchAngle;
				newAngle.Normalize(); //상하좌우 360도 회전되지 않게 앵글 값을 조정해준다.

				//에임 위치에 따라 newAngle - *Angle 값이 양수, 음수 랜덤하기 때문에
				//처음 몇발은 강제로 에임을 아래로 내려준다.
				//이후에도 강제로 계속 에임을 내려버리면 우리가 목표로한 뼈를 기준으로 반동제어가 안된다.
				if (recoil.x < 5) recoil.x += abs((newAngle - *Angle).x);
				else recoil.x += (newAngle - *Angle).x;
				//CSGO는 y가 좌우임 (y 값을 합하지 않는 이유는 FOV 내에서 쏴야하기 때문에 적당히 흔들어서 반동을 제어한다)
				//마지막 발까지 제어할수는 없지만 그전에 상대가 죽을 가능성이 높다.
				recoil.y = (newAngle - *Angle).y;

				//첫발때는 반동 크기만 계산하고 실질적으로 2번째 발사부터 반동을 제어해준다.
				if (fire > 2)
				{
					diffs.x += recoil.x;
					diffs.y += recoil.y;
					IsRecoil = true;
				}
			}
			else recoil = { 0,0,0 }; //발사를 멈추면 계산한 반동을 0으로 초기화
			oPunch = punchAngle; //이전에 발사된 반동 크기

			//적을 공격 할 위치 + 반동제어가 360도 회전하지 않게 조정
			if (diffs.y < -180) { diffs.y += 360; }
			if (diffs.y > 180) { diffs.y -= 360; }

			//반동제어를 해야된다면 스무스를 지우고 좀 더 빠르게 내 에임을 이동시켜준다.
			if (IsRecoil)
			{
				Angle->x += (diffs.x / smoothY / distSmooth / (GetRandomNum(2, 4) + me->IsShotFire / 10)); //pitch
				Angle->y += (diffs.y / smoothX / distSmooth / GetRandomNum(10, 12)); //yaw
			}
			else
			{
				Angle->x += (diffs.x / smoothY / distSmooth / GetSmoothY(fov_old, size) / GetRandomNum(2, 4)); //pitch
				Angle->y += (diffs.y / smoothX / distSmooth / GetSmoothX(fov_old, size) / GetRandomNum(2, 4)); //yaw
			}
		}
	}
}
