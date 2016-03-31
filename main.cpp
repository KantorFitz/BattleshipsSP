#include "okrety.h"
#include "interfejs.h"

using namespace std;

int main()
{
	const size_t BOARD_SIZE = 10;
	Coord pos(BOARD_SIZE, 0, 0);

	size_t ** Values;
	char Coordinates[BOARD_SIZE][2]; /*[...][0 = x; 1 = y]*/
	spData::EPart ** Solution;

	/*Dwie Plansze: (1)Wzor do wypelnienia i (2)Plansza operacyjna*/
	spData::EPart ** StartingPattern, **BoardToFill;

	Solution = new spData::EPart*[BOARD_SIZE];
	StartingPattern = new spData::EPart*[BOARD_SIZE];
	BoardToFill = new spData::EPart*[BOARD_SIZE];
	Values = new size_t*[2];

	/*Preparation of arrays*/
	for (size_t i = 0; i < BOARD_SIZE; i++)
	{
		Solution[i] = new spData::EPart[BOARD_SIZE];
		StartingPattern[i] = new spData::EPart[BOARD_SIZE];
		BoardToFill[i] = new spData::EPart[BOARD_SIZE];
		if(i < 2)
			Values[i] = new size_t[BOARD_SIZE];
	}

	/*Initialization of their default values*/
	for (size_t i = 0; i < BOARD_SIZE; i++)
	{
		for (size_t j = 0; j < BOARD_SIZE; j++)
		{
			BoardToFill[i][j] = StartingPattern[i][j] = Solution[i][j] = spData::EPEmpty;
			if(i < 2)
				Values[i][j] = 0;
		}
	}

	srand(time(NULL));

	static const std::vector<std::vector<spData::EShip>> shipsSet = { { spData::ES4Battleship },
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

	/*
	 *	up 72
	 *	down 80
	 *	left 75
	 *	right 77
	 *
	 *	113 119 101
	 *	  q  w  e
	 *	  a  s  d
	 *	 97 115 100
	 */
	unsigned char znak = 0;
	do
	{
		if(znak == 224)
			znak = getch();

		switch (znak)
		{
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

				spData::EPart upperTile = Solution[pos.getY() - 1][pos.getX()];

				if(upperTile != spData::EPPlaceHolder)
					break;

				if(pos.getY() >= 0)
				{
					if(helper.shipType != spData::ES1Cutter)
					{
						Solution[pos.getY()][pos.getX()] = spData::EPEmpty;
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

				spData::EPart lowerTile = Solution[pos.getY() + 1][pos.getX()];

				if(lowerTile != spData::EPEmpty)
					break;

				if(pos.getY() < BOARD_SIZE)
				{
					if(helper.shipType != spData::ES4Battleship)
					{

						pos.moveDown();
						Solution[pos.getY()][pos.getX()] = spData::EPPlaceHolder;
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

				spData::EPart prevTile = Solution[pos.getY()][pos.getX() - 1];

				if(prevTile != spData::EPPlaceHolder)
					break;

				if(pos.getX() >= 0)
				{
					if(helper.shipType != spData::ES1Cutter)
					{
						Solution[pos.getY()][pos.getX()] = spData::EPEmpty;
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

				spData::EPart nextTile = Solution[pos.getY()][pos.getX() + 1];

				if(nextTile != spData::EPEmpty)
					break;

				if(pos.getX() < BOARD_SIZE)
				{
					if(helper.shipType != spData::ES4Battleship)
					{
						pos.moveRight();
						Solution[pos.getY()][pos.getX()] = spData::EPPlaceHolder;
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
			Solution[pos.getY()][pos.getX()] = spData::EPMiddle;
			break;
		case 101:
			Solution[pos.getY()][pos.getX()] = spData::EPStriked;
			break;
		case 119:
			Solution[pos.getY()][pos.getX()] = spData::EPTopEnd;
			break;
		case 115:
			Solution[pos.getY()][pos.getX()] = spData::EPBotomEnd;
			break;
		case 97:
			Solution[pos.getY()][pos.getX()] = spData::EPLeftEnd;
			break;
		case 100:
			Solution[pos.getY()][pos.getX()] = spData::EPRightEnd;
			break;
		case 32:
			drawShipForced(Solution, BOARD_SIZE);
			prepareStartingPattern(Solution, BOARD_SIZE, StartingPattern);
			for(size_t y = 0; y < BOARD_SIZE; y++)
			{
				for(size_t x = 0; x < BOARD_SIZE; x++)
				{
					BoardToFill[y][x] = StartingPattern[y][x];
				}
			}
			break;
		case 13:
			if(!helper.isActive)
			{
				for (size_t i = 0; i < BOARD_SIZE; i++)
				{
					for (size_t j = 0; j < BOARD_SIZE; j++)
					{
						BoardToFill[i][j] = StartingPattern[i][j];
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

				Solution[tempy][tempx] = spData::EPEmpty;
			}
			helper.reset();
			break;
		case 114: //r
			if(!helper.isActive)
			{
				if(BoardToFill[pos.getY()][pos.getX()] != spData::EPEmpty)
					break;
				helper.StartPosition.setXY(pos.getX(), pos.getY());
				helper.shipType = spData::ES1Cutter;
				BoardToFill[pos.getY()][pos.getX()] = spData::EPPlaceHolder;
				helper.isActive = true;
			}
			else
			{
				if(helper.Direction == spData::EDRandom)
					helper.Direction = spData::EDHorizontal;
				if(!helper.remainedShips[4 - helper.shipType].empty())
				{
					drawShip(BoardToFill, BOARD_SIZE, helper.shipType, helper.Direction, helper.StartPosition);
					helper.destroyCurrentShip();
					helper.reset();
				}
			}
			break;
		case 63: // ?
			spInfo::printHelp(BOARD_SIZE);
			getch();
			spInfo::cleanDisplay(BOARD_SIZE);
			break;
		}

		spInfo::printRemainedShips(BOARD_SIZE, helper.remainedShips);

		countShipsParts(Solution, BOARD_SIZE, Values);

		fill(Coordinates[0], Coordinates[0] + BOARD_SIZE * 2, spData::EPEmpty);
		Coordinates[pos.getX()][0] = spData::EPBotomEnd;
		Coordinates[pos.getY()][1] = spData::EPRightEnd;

		spInfo::gotoXY();

		for (size_t posX = 0; posX < BOARD_SIZE; posX++)
		{
			if(posX == 0)
			{
				cout << "  ";
				for (size_t currX = 0; currX < BOARD_SIZE; currX++)
					cout << Values[0][currX];
				cout << endl << "  ";
				for (size_t currX = 0; currX < BOARD_SIZE; currX++)
					cout << static_cast<char>(Coordinates[currX][0]);
				cout << endl;
			}

			cout << Values[1][posX] << static_cast<char>(Coordinates[posX][1]);
			for (size_t currY = 0; currY < BOARD_SIZE; currY++)
				cout << static_cast<char>(BoardToFill[posX][currY]);
			cout << endl;
		}

		helper.checkShipAvailability(Solution);
#	ifdef printAdditionalInfo
		printf("Pozycja X: %d, pozycja Y: %d, znak(%c %d).", pos.getX(), pos.getY(), znak, znak);
#	endif

	} while((znak = getch()) != 27); //czyli ESC

	for (size_t i = 0; i < BOARD_SIZE; i++)
	{
		delete[] Solution[i];
		delete[] BoardToFill[i];
		delete[] StartingPattern[i];
		if(i < 2)
			delete[] Values[i];
	}
	delete[] Solution;
	delete[] BoardToFill;
	delete[] StartingPattern;
	delete[] Values;

#	ifdef printAdditionalInfo
#		undef printAdditionalInfo
#	endif

	return 0;
}
