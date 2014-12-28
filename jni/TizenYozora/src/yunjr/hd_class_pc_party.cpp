
#include "hd_class_extern.h"
#include "hd_class_pc_party.h"

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool hadar::PcParty::_load(const smutil::ReadStream& stream)
{
	return (stream.read((void*)save, sizeof(save)) == sizeof(save));
}

bool hadar::PcParty::_save(const smutil::WriteStream& stream) const
{
	return (stream.write((void*)save, sizeof(save)) == sizeof(save));
}

////////////////////////////////////////////////////////////////////////////////
// public method

hadar::PcParty::PcParty(void)
{
	sena::memclear(save, sizeof(save));

	encounter = 3;
	max_enemy = 5;
	food      = 10;
	gold      = 100;
}

void hadar::PcParty::face(int x1, int y1)
{
	if (x1 == 0)
		this->faced = (y1 < 0) ? 1 : 0;
	else
		this->faced = (x1 < 0) ? 3 : 2;
}

void hadar::PcParty::move(int x1, int y1)
{
	if ((x1 != 0) || (y1 != 0))
	{
		x_prev = x;
		y_prev = y;

		// ���� �̵��� ���� ���� ������ �ٲ��
		this->face(x1, y1);
	}

	x += x1;
	y += y1;
}

void hadar::PcParty::warp(POS pos, int x_in, int y_in)
{
	switch (pos)
	{
	case POS_PREV:
		this->x = this->x_prev;
		this->y = this->y_prev;
		break;
	case POS_ABS:
		this->x_prev = this->x;
		this->y_prev = this->y;
		this->x = x_in;
		this->y = y_in;
		break;
	case POS_REL:
		this->x_prev = this->x;
		this->y_prev = this->y;
		this->x += x_in;
		this->y += y_in;
		break;
	default:
		assert(false);
	}
}

void hadar::PcParty::igniteTorch(void)
{
	++ability.magic_torch;

	if (ability.magic_torch > 255)
		ability.magic_torch = 255;

	game::console::showMessage(15, "������ ������ ȶ���� �������ϴ�.");

	game::window::displayMap();
}

void hadar::PcParty::levitate(void)
{
	ability.levitation = 255;
	game::console::showMessage(15, "������ ���� �λ� �� �Դϴ�.");
}

void hadar::PcParty::walkOnWater(void)
{
	ability.walk_on_water = 255;
	game::console::showMessage(15, "������ �� ���� ������ �ֽ��ϴ�.");
}

void hadar::PcParty::walkOnSwamp(void)
{
	ability.walk_on_swamp = 255;
	game::console::showMessage(15, "������ �� ���� ������ �ֽ��ϴ�.");
}
