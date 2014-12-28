
#include "yunjr_class_extern.h"
#include "yunjr_class_pc_party.h"

#include "util/sm_util_sena.h"

////////////////////////////////////////////////////////////////////////////////
// serializing method

bool yunjr::PcParty::_load(const smutil::ReadStream& stream)
{
	return (stream.read((void*)save, sizeof(save)) == sizeof(save));
}

bool yunjr::PcParty::_save(const smutil::WriteStream& stream) const
{
	return (stream.write((void*)save, sizeof(save)) == sizeof(save));
}

////////////////////////////////////////////////////////////////////////////////
// public method

yunjr::PcParty::PcParty(void)
{
	sena::memclear(save, sizeof(save));

	encounter = 3;
	max_enemy = 5;
	food      = 10;
	gold      = 100;
}

void yunjr::PcParty::face(int x1, int y1)
{
	if (x1 == 0)
		this->faced = (y1 < 0) ? 1 : 0;
	else
		this->faced = (x1 < 0) ? 3 : 2;

	this->faced_x1 = x1;
	this->faced_y1 = y1;
}

void yunjr::PcParty::move(int x1, int y1)
{
	if ((x1 != 0) || (y1 != 0))
	{
		x_prev = x;
		y_prev = y;

		// 실제 이동이 있을 때만 방향이 바뀐다
		this->face(x1, y1);
	}

	x += x1;
	y += y1;
}

void yunjr::PcParty::warp(POS pos, int x_in, int y_in)
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

void yunjr::PcParty::igniteTorch(void)
{
	++ability.magic_torch;

	if (ability.magic_torch > 255)
		ability.magic_torch = 255;

	game::console::showMessage(15, L"일행은 마법의 횃불을 밝혔습니다.");

	game::window::displayMap();
}

void yunjr::PcParty::levitate(void)
{
	ability.levitation = 255;
	game::console::showMessage(15, L"일행은 공중 부상 중 입니다.");
}

void yunjr::PcParty::walkOnWater(void)
{
	ability.walk_on_water = 255;
	game::console::showMessage(15, L"일행은 물 위를 걸을수 있습니다.");
}

void yunjr::PcParty::walkOnSwamp(void)
{
	ability.walk_on_swamp = 255;
	game::console::showMessage(15, L"일행은 늪 위를 걸을수 있습니다.");
}
