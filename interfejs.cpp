/*
 * interfejs.cpp
 *
 *  Created on: 14 mar 2016
 *      Author: Kantor
 */

#include "interfejs.h"

namespace conInfo
{
	int wherex()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
			return -1;
		return csbi.dwCursorPosition.X;
	}

	int wherey()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if(!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
			return -1;
		return csbi.dwCursorPosition.Y;
	}
}

void spInfo::printHelp(size_t gap)
{
	gotoXY();

    gotoXY(XY(gap + 5, conInfo::wherey())); printf("Gra w okrety - wersja ALFA\n"              );
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("wskazniki przesuwa sie strzalkami,\n"      );
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("elementy statku to:\n"                     );
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("gorna czesc okretu (%c) - klawisz w,\n",     EPart::TopEnd);
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("dolna czesc okretu (%c) - klawisz s,\n",     EPart::BotomEnd);
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("lewa czesc okretu (%c) - klawisz a,\n",      EPart::LeftEnd);
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("prawa czesc okretu (%c) - klawisz d,\n",     EPart::RightEnd);
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("srodkowa czesc okretu (%c) - klawisz q,\n",  EPart::Middle);
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("przekreslenie (%c) - klawisz e\n",           EPart::Striked);
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("Dynamiczny wybor statku - klawisz r\n"     );
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("Anulowanie powyzszego trybu - klawisz BACKSPACE\n");

    gotoXY(XY(gap + 5, conInfo::wherey())); printf("Od poczatku - klawisz Enter\n"             );
    gotoXY(XY(gap + 5, conInfo::wherey())); printf("opuszczenie programu - klawisz ESC\n"      );
}

void spInfo::printRemainedShips(size_t gap, std::vector< std::vector<EShip::EShip> > const & remainedShips)
{
	std::map<EShip::EShip, std::string> bar = { { EShip::Battleship_4, "\x11\xfe\xfe\x10" }, //<==>
			{ EShip::Cruiser_3, "\x11\xfe\x10" }, //<=>
			{ EShip::Destroyer_2, "\x11\x10" }, //<>
			{ EShip::Cutter_1, "\x04" }, }; //=

	gotoXY();
	for(size_t i = 0; i < remainedShips.size(); i++)
	{
		printf("%*c", gap + 5, ' ');

		for(const EShip::EShip a : remainedShips[i])
			printf("%s ", bar[a].c_str());

		printf("%*c\n", 10, ' ');
	}
}

void spInfo::cleanDisplay(size_t size)
{
	gotoXY();
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	size_t tempSize = size;
	size += 5;
	while(size--)
	{
		unsigned line = csbi.dwMaximumWindowSize.X - (tempSize + 5); //d³ugoœc linii minus szerokosc planszy
		gotoXY(XY(tempSize + 5, conInfo::wherey()));
		while(line--)
			std::cout << " ";
	}

	gotoXY();
	return;
}

void spInfo::gotoXY(const XY xy)
{
	COORD foo;
	foo.X = xy.getX();
	foo.Y = xy.getY();
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), foo);
	return;
}
