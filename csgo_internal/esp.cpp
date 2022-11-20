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

		//�� �ڽ��� �ǳʶٰ�, ���� �����Ѵ�.
		if (me == player || me->Team == player->Team) continue;

		//ESP
		//�Ÿ� 1500 = 1500cm = (150 * 10)m = 150m
		//������ ������ ����� ��� ���� ���� ���� �Ÿ� ��� ���� ESP �׸���
		if (Distance(me->Origin, player->Origin) > 1500 && player->SpottedByMask == 0) continue;

		//���� ���̴��� Ȯ��
		bool checkVisible = false; //��븦 ������ �� �ִ��� Ȯ��
		for (size_t idx = 0; idx < boneList.size(); idx++)
		{
			if (IsVisible(me, player, boneList[idx]))
			{
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
				//�ڽ� ESP - center�� x�� ���ش�. �׷� ĳ���� x: ���� ��� y: �Ӹ� �� ���� ��ġ�� ���� ���´�
				//�Ʒ����� +�� ������ CSGO�� x, y�� �ݴ�� ������
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
				if (health <= 60 && health > 30) healthColor = D3DCOLOR_ARGB(255, 255, 128, 0);
				if (health <= 30) healthColor = D3DCOLOR_ARGB(255, 255, 0, 0);

				//ü�¹� �׸���
				DrawFilled(hx + hThick, hy + hThick, hWidth - hThick * 2, hHeight - hThick * 2, healthColor);
				//ü�¹� �׵θ� �׸���
				Draw2DBox(hx, hy - hThick, -width + thick * 2, hHeight, hThick, D3DCOLOR_ARGB(255, 32, 32, 32));
			}
		}
	}
}
