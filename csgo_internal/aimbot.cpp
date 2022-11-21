#include "inc.h"

void AimAt(Ent* target, vec3 diffs, float dist, float size)
{
	Ent* me = *(Ent**)(clientdll + offsets::dwLocalPlayer);
	if (!me || !me->IsValidPlayer() || !target || dist == FLT_MAX) return;

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
		if (recoil.x < 3) recoil.x += abs((newAngle - *Angle).x);
		else recoil.x += (newAngle - *Angle).x;
		recoil.y += (newAngle - *Angle).y;

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

	//적을 공격 할 위치 + 반동제어 앵글 값이 360도 회전하지 않게 조정
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
		Angle->x += (diffs.x / smoothY / distSmooth / GetSmoothY(dist, size) / GetRandomNum(2, 4)); //pitch
		Angle->y += (diffs.y / smoothX / distSmooth / GetSmoothX(dist, size) / GetRandomNum(2, 4)); //yaw
	}
}

void Aimbot()
{
	Ent* target = NULL;
	vec3 diffs;
	float fov_old = FLT_MAX;
	float size = 0;

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

		RECT rect = GetClientSize(XOR("Valve001"), 0);
		int midX = rect.right / 2; //클라이언트 정중앙 X
		int midY = rect.bottom / 2; //클라이언트 정중앙 Y
		float multX = midX / me->FOV; //클라이언트 (테두리 없는) 정중앙 X 위치 / 캐릭터 시야
		size = (FOV * multX); //내가 설정한 FOV 값 * multX

		int boneID = 999;
		for (size_t idx = 0; idx < boneList.size(); idx++)
		{
			if (IsVisible(me, player, boneList[idx]))
			{
				boneID = boneList[idx];
				break;
			}
		}

		if (boneID == 999) continue;

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
				veloc.x /= 13; //CSGO는 탄도학 개념이 없기 때문에
				veloc.y /= 13; //적당히 캐릭터 피격 범위 내에서만
				veloc.z /= 13; //예측하게 임의로 값을 낮춰줌 또 아예 없으면 에임이 뒤쳐짐
				vec3 predict = bonePos + veloc; //뼈위치 + 이동속도
				vec3 delta = CalcReverseAngle(myPos, predict); //내 에임(앵글)이 이동해야할 위치 계산
				diffs = delta - *GetMyAngle(); //현재 내 앵글과 에임(앵글)이 이동해야할 위치를 빼준다. (에임을 인간 처럼 보이기 위한 빌드업)
				target = player; //타겟을 저장
				fov_old = fovAngle; //FOV내에 적이 여러명이면 가장 가까운 적을 선별하기 위해 min 값을 계산
			}
		}
	}

	AimAt(target, diffs, fov_old, size);
}
