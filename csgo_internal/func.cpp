#include "inc.h"

void SetCheat()
{
	Ent* target = NULL;
	float fov_old = FLT_MAX;
	RECT rect = GetClientSize(XOR("Valve001"), 0);
	int midX = rect.right / 2; //Ŭ���̾�Ʈ ���߾� X
	int midY = rect.bottom / 2; //Ŭ���̾�Ʈ ���߾� Y
	float multX;
	float size;
	vec3* Angle;
	vec3 diffs;

	//Aimbot, ESP�� ���� ���ȭ���� ���� ������ for�� 32���� ������ ���� ��� ���� ó���ϱ� ����
	//EndScene�� ��ŷ�߱� ������ ������ �� �۾��� �ּ�ȭ �� ���� (������ ����� �ּ�ȭ�ϰ� ������ �ּ�ȭ)
	//�ڵ尡 �����ϱ� ������ ���ȭ�ص� ū ���̴� �������� �ҽ����� �������� ����غ����� ����
	for (size_t i = 0; i < 32; i++) //GetMaxPlayer()�� ����ϸ� �׻� 64 �����̱� ������ �׳� �ִ� �ο� 32�� ���� (���� �����ϰ� 2�� ���̴� ���� ����)
	{
		Ent* me = *(Ent**)(clientdll + offsets::dwLocalPlayer);
		if (!me->IsValidPlayer()) continue;

		EntList* entList = (EntList*)(clientdll + offsets::dwEntityList);
		if (!entList) continue;

		Ent* player = entList->entBaseList[i].ent;
		if (!player->IsValidPlayer()) continue;

		//�� �ڽ��� �ǳʶٰ�, ���� �����Ѵ�.
		if (me == player || me->Team == player->Team) continue;

		//ESP
		//�Ÿ� 1500 = 1500cm = (150 * 10)m = 150m
		//������ ������ ����� ��� ���� ���� ���� �Ÿ� ��� ���� ESP �׸���
		if (Distance(me->Origin, player->Origin) > 1500 && player->SpottedByMask == 0) continue;

		//���� ���̴��� Ȯ�� (ESP, ���Ӻ�)
		int boneID = 8; //� ���� �������� ����
		bool checkVisible = false; //��븦 ������ �� �ִ��� Ȯ��
		//�Ӹ����� �������� ��ĵ�� ���� ���� ã�� ���� ����
		for (size_t idx = 0; idx < boneList.size(); idx++)
		{
			if (IsVisible(me, player, boneList[idx]))
			{
				boneID = boneList[idx];
				checkVisible = true;
				break;
			}
		}

		//���ڱ��п� ���� ESP ���� ����
		D3DCOLOR color = D3DCOLOR_ARGB(255, 120, 120, 120);
		if (checkVisible) color = D3DCOLOR_ARGB(255, 153, 51, 255);

		vec2 vScreen, vBone; //W2S���� ��� ��, �Ӹ� ��ġ�� 2D��ǥ�� ������
		if (W2S(player->Origin, vScreen))
		{
			vec3 bonePos = player->GetBone();
			bonePos.z += 7; //�� ���߾ӿ� �ִ� ��ġ�� ���Ƿ� �Ӹ� ������ ����
			if (W2S(bonePos, vBone))
			{
				//�ڽ��� �׸��� ���� ����, ����, ���� ���� ���� ������
				float head = vBone.y - vScreen.y;
				float width = head / 2;
				float center = width / -2; //Origin�� �� ���� ���߾� ��ġ�� center ���� �����´�

				//Box ESP
				int thick = 1; //�ڽ� ESP�� �� �ߺ��̰� �ϱ� ���� �׵θ� �β��� ����
				D3DCOLOR backColor = D3DCOLOR_ARGB(255, 0, 0, 0); //�׵θ� ���� ����

				//�ٱ��� �׵θ�
				Draw2DBox(vScreen.x + center - thick, vScreen.y - thick, width + thick * 2, head + thick * 2, 1, backColor);
				//�ڽ� ESP - center�� x�� ���ش�. �׷� ĳ���� ���� ���� ��ġ�� x���� ���´�
				//�Ʒ����� +�� ������ vScreen�� �����̱� ������
				Draw2DBox(vScreen.x + center, vScreen.y, width, head, thick, color);
				//���� �׵θ�
				Draw2DBox(vScreen.x + center + thick, vScreen.y + thick, width - thick * 2, head - thick * 2, 1, backColor);

				//HealthBar
				int health = player->Health; //ü�¿� ���� ü�¹ٸ� �����ϱ� ���� ����
				float hThick = 1; //�׵θ� �β� ����

				//ESP �ڽ� ����(���� �Ǵ� ����) / 100 * health �� �⺻ ����
				//�Ʒ��� �׵θ� �β����� ����Ѱ�
				float hWidth = -((width - thick * 2) / 100 * health); //�ｺ�� ����
				float hHeight = 5; //�ｺ�� ����
				float hx = vScreen.x - center; //�ｺ�� x ��ġ
				float hy = vScreen.y + head - hHeight - 1; //�ｺ�� y ��ġ

				//ü�¿� ���� ���� ����
				D3DCOLOR healthColor = D3DCOLOR_ARGB(255, 0, 255, 0);
				if(health <= 60 && health > 30) healthColor = D3DCOLOR_ARGB(255, 255, 128, 0);
				if(health <= 30) healthColor = D3DCOLOR_ARGB(255, 255, 0, 0);

				//ü�¹� �׸���
				DrawFilled(hx + hThick, hy + hThick, hWidth - hThick * 2, hHeight - hThick * 2, healthColor);
				//ü�¹� �׵θ� �׸���
				Draw2DBox(hx, hy - hThick, -width + thick * 2, hHeight, hThick, D3DCOLOR_ARGB(255, 32, 32, 32));
			}
		}

		//Aimbot
		//������ ������ ���ڱ����� false��� continue
		if (!checkVisible) continue;
		multX = midX / me->FOV; //Ŭ���̾�Ʈ (�׵θ� ����) ���߾� X ��ġ / ĳ���� �þ�
		size = (FOV * multX); //���� ������ FOV �� * multX

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
				veloc.x /= 12; //CSGO�� ź���� ������ ���� ������
				veloc.y /= 12; //������ ĳ���� �ǰ� ���� ��������
				veloc.z /= 12; //�����ϰ� ���Ƿ� ���� ������ �� �ƿ� ������ ������ ������
				vec3 predict = bonePos + veloc; //����ġ + �̵��ӵ�
				vec3 delta = CalcReverseAngle(myPos, predict); //�� ����(�ޱ�)�� �̵��ؾ��� ��ġ ���
				diffs = delta - *GetMyAngle(); //���� �� �ޱ۰� ����(�ޱ�)�� �̵��ؾ��� ��ġ�� ���ش�. (������ �ΰ� ó�� ���̱� ���� �����)
				target = player; //Ÿ���� ����
				fov_old = fovAngle; //FOV���� ���� �������̸� ���� ����� ���� �����ϱ� ���� min ���� ���
			}
		}
	} //��ƼƼ for�� ����

	//��Ű�� �����ִٸ� Ÿ������ ���콺 �̵�
	if (LI_FN(GetAsyncKeyState).cached()(VK_XBUTTON2) & 0x8000)
	{
		Ent* me = *(Ent**)(clientdll + offsets::dwLocalPlayer);
		if (me->IsValidPlayer() && target && fov_old != FLT_MAX)
		{
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
				if (recoil.x < 5) recoil.x += abs((newAngle - *Angle).x);
				else recoil.x += (newAngle - *Angle).x;
				//CSGO�� y�� �¿��� (y ���� ������ �ʴ� ������ FOV ������ �����ϱ� ������ ������ ��� �ݵ��� �����Ѵ�)
				//������ �߱��� �����Ҽ��� ������ ������ ��밡 ���� ���ɼ��� ����.
				recoil.y = (newAngle - *Angle).y;

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

			//���� ���� �� ��ġ + �ݵ���� 360�� ȸ������ �ʰ� ����
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
				Angle->x += (diffs.x / smoothY / distSmooth / GetSmoothY(fov_old, size) / GetRandomNum(2, 4)); //pitch
				Angle->y += (diffs.y / smoothX / distSmooth / GetSmoothX(fov_old, size) / GetRandomNum(2, 4)); //yaw
			}
		}
	}
}
