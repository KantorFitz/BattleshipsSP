#include "okrety.h"
#include "interfejs.h"

using namespace std;

int main()
{
#if 0

	return 0;
#endif

	srand(time(NULL));

	const static std::vector<std::vector<spData::EShip>> shipsSet = { { spData::ES4Battleship },
			{ spData::ES3Cruiser, spData::ES3Cruiser },
			{ spData::ES2Destroyer, spData::ES2Destroyer, spData::ES2Destroyer },
			{ spData::ES1Cutter, spData::ES1Cutter, spData::ES1Cutter, spData::ES1Cutter } };

	struct Helper
	{
		XY StartPosition;
		bool isActive;
		bool isDirSet;
		spData::EShip shipType;
		spData::EDirection Direction;
		vector<vector<spData::EShip>> remainedShips = shipsSet;

		void checkShipAvailability(spData::EPart ** Tablica)
		{
			HANDLE hCONSOLE;
			hCONSOLE = GetStdHandle(STD_OUTPUT_HANDLE);

			if(remainedShips[4 - shipType].empty())
			{
				SetConsoleTextAttribute(hCONSOLE, 12);//red
				for (size_t i = 0; i < shipType; i++)
				{
					size_t tempx = (
							Direction ? StartPosition.getX() + i + 2 : StartPosition.getX() + 2);
					size_t tempy = (
							Direction ? StartPosition.getY() + 2 : StartPosition.getY() + i + 2);

					spInfo::gotoXY(XY(tempx, tempy));
					cout << static_cast<char>(spData::EPPlaceHolder);
				}
				spInfo::gotoXY();
				SetConsoleTextAttribute(hCONSOLE, 7);//default white
			}
		}

		void reset(void)
		{
			StartPosition.setXY(-1, -1);
			isDirSet = isActive = false;
			shipType = spData::ESRandom;
			Direction = spData::EDRandom;
		}
		void reinforcements(void)
		{
			remainedShips = shipsSet;
		}
		bool destroyCurrentShip()
		{
			if(shipType != spData::ESRandom)
			{
				int tier = 4 - shipType;
				if(!remainedShips[tier].empty())
				{
					remainedShips[tier].pop_back();
					return true;
				}
			}

			return false;
		}
		void setDirection(spData::EDirection Kierunek)
		{
			Direction = Kierunek;
			isDirSet = true;
		}

		Helper & operator++()
		{
			if(this->shipType != spData::ES4Battleship)
				this->shipType = static_cast<spData::EShip>(this->shipType + 1);
			return *this;
		}

		Helper & operator--()
		{
			if(this->shipType != spData::ES1Cutter)
				this->shipType = static_cast<spData::EShip>(this->shipType - 1);
			return *this;
		}

		Helper() : isActive(false), isDirSet(false), shipType(spData::ESRandom),
					 Direction(spData::EDRandom) {} //c-tor
	} helper;

	const size_t size = 10;
	Coord pos(size, 0, 0);

	size_t ** Values;				// index 0 - dla iksów
	char Coordinates[size][2];		// index 1 - dla igreków
	spData::EPart ** Plansza;

	/*Dwie Plansze bêd¹ce: (1)Wzorem do wype³nienia i (2)Plansz¹ operacyjn¹*/
	spData::EPart ** StartingPattern, BoardToFill;

	Plansza = new spData::EPart*[size];
	Values = new size_t*[2];
	for (size_t i = 0; i < size; i++)
	{
		Plansza[i] = new spData::EPart[size];
		if(i < 2)
			Values[i] = new size_t[size];
	}

	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = 0; j < size; j++)
		{
			Plansza[i][j] = spData::EPEmpty;
			if(i < 2)
				Values[i][j] = 0;
		}
	}

	unsigned char znak = 0;                      // up    72
	do                                           // down  80
	{                                            // left  75
		if(znak == 224)                          // right 77
			znak = getch();                      //113 119 101
                                                 //   q w e
		switch (znak)                            //   a s d
		{                                        // 97 115 100
		case 72: // ^
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = spData::EDVertical;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != spData::EDVertical)
						break;
				}

				if(pos.isCursorOnTopBorder())
					break;

				spData::EPart upperTile = Plansza[pos.getY() - 1][pos.getX()];

				if(upperTile != spData::EPPlaceHolder)
					break;

				if(pos.getY() >= 0)
				{
					if(helper.shipType != spData::ES1Cutter)
					{
						Plansza[pos.getY()][pos.getX()] = spData::EPEmpty;
						pos.moveUp();
						--helper; //statek siê zmniejsza
						break;
					}
					else
						break;
				}
			}
			pos.moveUp();
			break;
		case 80: // v
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = spData::EDVertical;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != spData::EDVertical)
					{
						if(helper.shipType == spData::ES1Cutter)
							helper.Direction = spData::EDVertical;
						else
							break;
					}
				}

				if(pos.isCursorOnBottomBorder())
					break;

				spData::EPart lowerTile = Plansza[pos.getY() + 1][pos.getX()];

				if(lowerTile != spData::EPEmpty)
					break;

				if(pos.getY() < size)
				{
					if(helper.shipType != spData::ES4Battleship)
					{

						pos.moveDown();
						Plansza[pos.getY()][pos.getX()] = spData::EPPlaceHolder;
						++helper; //statek siê zwiêksza
						break;
					}
					else
						break;
				}
			}
			pos.moveDown();
			break;
		case 75:// <--
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = spData::EDHorizontal;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != spData::EDHorizontal)
						break;
				}

				if(pos.isCursorOnLeftBorder())
					break;

				spData::EPart prevTile = Plansza[pos.getY()][pos.getX() - 1];

				if(prevTile != spData::EPPlaceHolder)
					break;

				if(pos.getX() >= 0)
				{
					if(helper.shipType != spData::ES1Cutter)
					{
						Plansza[pos.getY()][pos.getX()] = spData::EPEmpty;
						pos.moveLeft();
						--helper; //statek siê zmniejsza
						break;
					}
					else
						break;
				}
			}
			pos.moveLeft();
			break;
		case 77: //-->
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = spData::EDHorizontal;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != spData::EDHorizontal)
					{
						if(helper.shipType == spData::ES1Cutter)
							helper.Direction = spData::EDHorizontal;
						else
							break;
					}
				}

				if(pos.iscursorOnRightBorder())
					break;

				spData::EPart nextTile = Plansza[pos.getY()][pos.getX() + 1];

				if(nextTile != spData::EPEmpty)
					break;

				if(pos.getX() < size)
				{
					if(helper.shipType != spData::ES4Battleship)
					{
						pos.moveRight();
						Plansza[pos.getY()][pos.getX()] = spData::EPPlaceHolder;
						++helper; //statek siê zwiêksza
						break;
					}
					else
						break;
				}
			}
			pos.moveRight();
			break;
		case 113:
			Plansza[pos.getY()][pos.getX()] = spData::EPMiddle;
			break;
		case 101:
			Plansza[pos.getY()][pos.getX()] = spData::EPStriked;
			break;
		case 119:
			Plansza[pos.getY()][pos.getX()] = spData::EPTopEnd;
			break;
		case 115:
			Plansza[pos.getY()][pos.getX()] = spData::EPBotomEnd;
			break;
		case 97:
			Plansza[pos.getY()][pos.getX()] = spData::EPLeftEnd;
			break;
		case 100:
			Plansza[pos.getY()][pos.getX()] = spData::EPRightEnd;
			break;
		case 32:
			drawShipForced(Plansza, size);
			break;
		case 13:
			if(!helper.isActive)
			{
				for (size_t i = 0; i < size; i++)
				{
					for (size_t j = 0; j < size; j++)
					{
						Plansza[i][j] = spData::EPEmpty;
						if(i < 2)
							Values[i][j] = 0;
					}
				}
				helper.reinforcements();
			}
			break;
		case 8:	// backspace
			for (size_t i = 0; i < helper.shipType; i++)
			{
				size_t tempx = (
						helper.Direction ? helper.StartPosition.getX() + i : helper.StartPosition.getX());
				size_t tempy = (
						helper.Direction ? helper.StartPosition.getY() : helper.StartPosition.getY() + i);

				Plansza[tempy][tempx] = spData::EPEmpty;
			}
			helper.reset();
			break;
		case 114: //r
			if(!helper.isActive)
			{
				if(Plansza[pos.getY()][pos.getX()] != spData::EPEmpty)
					break;
				helper.StartPosition.setXY(pos.getX(), pos.getY());
				helper.shipType = spData::ES1Cutter;
				Plansza[pos.getY()][pos.getX()] = spData::EPPlaceHolder;
				helper.isActive = true;
			}
			else
			{
				if(helper.Direction == spData::EDRandom)
					helper.Direction = spData::EDHorizontal;
				if(!helper.remainedShips[4 - helper.shipType].empty())
				{
					drawShip(Plansza, size, helper.shipType, helper.Direction, helper.StartPosition);
					helper.destroyCurrentShip();
					helper.reset();
				}
			}
			break;
		case 63: // ?
			spInfo::printHelp(size);
			getch();
			spInfo::cleanDisplay(size);
			break;
		}

		spInfo::printRemainedShips(size, helper.remainedShips);

		countShipsParts(Plansza, size, Values);

		fill(Coordinates[0], Coordinates[0] + size * 2, spData::EPEmpty);
		Coordinates[pos.getX()][0] = spData::EPBotomEnd;
		Coordinates[pos.getY()][1] = spData::EPRightEnd;

		spInfo::gotoXY();

		for (size_t posX = 0; posX < size; posX++)
		{
			if(posX == 0)
			{
				cout << "  ";
				for (size_t currX = 0; currX < size; currX++)
					cout << Values[0][currX];
				cout << endl << "  ";
				for (size_t currX = 0; currX < size; currX++)
					cout << static_cast<char>(Coordinates[currX][0]);
				cout << endl;
			}

			cout << Values[1][posX] << static_cast<char>(Coordinates[posX][1]);
			for (size_t currY = 0; currY < size; currY++)
				cout << static_cast<char>(Plansza[posX][currY]);
			cout << endl;
		}

		helper.checkShipAvailability(Plansza);
#	ifdef printAdditionalInfo
		printf("Pozycja X: %d, pozycja Y: %d, znak(%c %d).", pos.getX(), pos.getY(), znak, znak);
#	endif

	} while((znak = getch()) != 27); //czyli ESC

	for (size_t i = 0; i < size; i++)
	{
		delete[] Plansza[i];
		if(i < 2)
			delete[] Values[i];
	}
	delete[] Plansza;
	delete[] Values;

#	ifdef printAdditionalInfo
#		undef printAdditionalInfo
#	endif

	return 0;
}
