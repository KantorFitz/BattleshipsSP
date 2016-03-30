/*
 * interfejs.h
 *
 *  Created on: 14 mar 2016, 18:35:59
 *      Author: Kantor
 */

#pragma once

#include "okrety.h"

namespace spInfo
{
	void printHelp(size_t);
	void printRemainedShips(size_t, std::vector<std::vector<spData::EShip>> const &);
	void cleanDisplay(size_t);
	void gotoXY(const XY xy = XY(0, 0));
}
