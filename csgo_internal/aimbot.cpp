#include "inc.h"

void AimAt(Ent* target, vec3 diffs, float dist, float size)
{
	Ent* me = *(Ent**)(clientdll + offsets::dwLocalPlayer);
	if (!me || !me->IsValidPlayer() || !target || dist == FLT_MAX) return;

	vec3* Angle = GetMyAngle();
	float distSmooth = Distance(me->Origin, target->Origin);

	//�������� ���� �ѱ���� �� ������ ������� Ȯ���Ѵ�.
	//������ ����� ���� ���� ������ �ſ� ��������Ƿ� �� ���� �������ϰ� ������ش�.
	if (InRange(me->FOV, 90, 91) && distSmooth > 800) distSmooth = 5;
	if (InRange(me->FOV, 90, 91) && distSmooth <= 800) distSmooth = 2;

	if (InRange(me->FOV, 40, 90) && distSmooth <= 400) distSmooth = 5;
	if (InRange(me->FOV, 40, 90) && distSmooth > 400) distSmooth = 2;

	if (InRange(me->FOV, 0, 20) && distSmooth <= 200) distSmooth = 5;
	if (InRange(me->FOV, 0, 40) && distSmooth > 200) distSmooth = 2;

	//�ݵ����� (Anti Recoil)
	vec3 punchAngle = me->PunchAngle * 2; //�ݵ��� ������ �ޱ� �� * 2�� ����
	int fire = me->IsShotFire; //�� ĳ���Ͱ� ���� ��� �ִ��� Ȯ��
	bool IsRecoil = false; //�ݵ��� �����ؾ��ϴ��� Ȯ��
	if (fire >= 1) //1�� �̻� ���ٸ�
	{
		//�� �ޱ� + ������ ������ �ݵ� - (���� ������ �ݵ� * 2)
		//���� �����ϸ� *Angle + oPunch�� ���� ���� �� ������ �ö� ������ ���̴�.
		//�ݵ��� ���� �ö� ������ ��ġ����
		//���� �ݵ��� 2�踦 �����ָ鼭 ���� �θ��� ��ź ������ �ϰ� �ȴ�.
		vec3 newAngle = *Angle + oPunch - punchAngle;
		newAngle.Normalize(); //�����¿� 360�� ȸ������ �ʰ� �ޱ� ���� �������ش�.

		//���� ��ġ�� ���� newAngle - *Angle ���� ���, ���� �����ϱ� ������
		//ó�� ����� ������ ������ �Ʒ��� �����ش�.
		//���Ŀ��� ������ ��� ������ ���������� �츮�� ��ǥ���� ���� �������� �ݵ���� �ȵȴ�.
		if (recoil.x < 3) recoil.x += abs((newAngle - *Angle).x);
		else recoil.x += (newAngle - *Angle).x;
		recoil.y += (newAngle - *Angle).y;

		//ù�߶��� �ݵ� ũ�⸸ ����ϰ� ���������� 2��° �߻���� �ݵ��� �������ش�.
		if (fire > 2)
		{
			diffs.x += recoil.x;
			diffs.y += recoil.y;
			IsRecoil = true;
		}
	}
	else recoil = { 0,0,0 }; //�߻縦 ���߸� ����� �ݵ��� 0���� �ʱ�ȭ
	oPunch = punchAngle; //������ �߻�� �ݵ� ũ��

	//���� ���� �� ��ġ + �ݵ����� �ޱ� ���� 360�� ȸ������ �ʰ� ����
	if (diffs.y < -180) { diffs.y += 360; }
	if (diffs.y > 180) { diffs.y -= 360; }

	//�ݵ���� �ؾߵȴٸ� �������� ����� �� �� ������ �� ������ �̵������ش�.
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

		//�� �ڽ��� �ǳʶٰ�, ���� �����Ѵ�.
		if (me == player || me->Team == player->Team) continue;

		RECT rect = GetClientSize(XOR("Valve001"), 0);
		int midX = rect.right / 2; //Ŭ���̾�Ʈ ���߾� X
		int midY = rect.bottom / 2; //Ŭ���̾�Ʈ ���߾� Y
		float multX = midX / me->FOV; //Ŭ���̾�Ʈ (�׵θ� ����) ���߾� X ��ġ / ĳ���� �þ�
		size = (FOV * multX); //���� ������ FOV �� * multX

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

		//Ÿ�� ����
		vec3 bonePos = player->GetBone(boneID); //������ ������ ���̴� �� ��ġ ����
		vec2 vAngle;
		if (W2S(bonePos, vAngle))
		{
			//Ŭ���̾�Ʈ ���߾� x, y��ǥ�� ��� ��ġ������ �Ÿ� ��� (2D ��ǥ)
			float fovAngle = sqrt((midX - vAngle.x) * (midX - vAngle.x) + (midY - vAngle.y) * (midY - vAngle.y));

			//���� ������ FOV ���� ���� �ִ��� Ȯ��
			//FOV���� �������� ���� ��� 2D��ǥ�� ���� ����� ���� Ÿ������ ����
			if (fovAngle < size && fovAngle < fov_old)
			{
				vec3 myPos = me->Origin + me->ViewOffset; //�ڽ��� ĳ���� �� ��ġ
				vec3 veloc = player->Velocity; //���� �̵��ӵ��� ������
				veloc.x /= 13; //CSGO�� ź���� ������ ���� ������
				veloc.y /= 13; //������ ĳ���� �ǰ� ���� ��������
				veloc.z /= 13; //�����ϰ� ���Ƿ� ���� ������ �� �ƿ� ������ ������ ������
				vec3 predict = bonePos + veloc; //����ġ + �̵��ӵ�
				vec3 delta = CalcReverseAngle(myPos, predict); //�� ����(�ޱ�)�� �̵��ؾ��� ��ġ ���
				diffs = delta - *GetMyAngle(); //���� �� �ޱ۰� ����(�ޱ�)�� �̵��ؾ��� ��ġ�� ���ش�. (������ �ΰ� ó�� ���̱� ���� �����)
				target = player; //Ÿ���� ����
				fov_old = fovAngle; //FOV���� ���� �������̸� ���� ����� ���� �����ϱ� ���� min ���� ���
			}
		}
	}

	AimAt(target, diffs, fov_old, size);
}
