#include "okrety.h"
#include "interfejs.h"

using namespace std;

int main()
{
	const size_t BOARD_SIZE = 10;
	Coord pos(BOARD_SIZE, 0, 0);

	size_t ** Values;
	char Coordinates[BOARD_SIZE][2]; /*[...][0 = x; 1 = y]*/
	EPart::EPart ** Solution;

	/*Dwie Plansze: (1)Wzor do wypelnienia i (2)Plansza operacyjna*/
	EPart::EPart ** StartingPattern, **BoardToFill;

	Solution = new EPart::EPart*[BOARD_SIZE];
	StartingPattern = new EPart::EPart*[BOARD_SIZE];
	BoardToFill = new EPart::EPart*[BOARD_SIZE];
	Values = new size_t*[2];

	/*Preparation of arrays*/
	for (size_t i = 0; i < BOARD_SIZE; i++)
	{
		Solution[i] = new EPart::EPart[BOARD_SIZE];
		StartingPattern[i] = new EPart::EPart[BOARD_SIZE];
		BoardToFill[i] = new EPart::EPart[BOARD_SIZE];
		if(i < 2)
			Values[i] = new size_t[BOARD_SIZE];
	}

	/*Initialization of their default values*/
	for (size_t i = 0; i < BOARD_SIZE; i++)
	{
		for (size_t j = 0; j < BOARD_SIZE; j++)
		{
			BoardToFill[i][j] = StartingPattern[i][j] = Solution[i][j] = EPart::Empty;
			if(i < 2)
				Values[i][j] = 0;
		}
	}

	srand(time(NULL));

	static const std::vector<std::vector<EShip::EShip>> shipsSet = { { EShip::Battleship_4 },
			{ EShip::Cruiser_3, EShip::Cruiser_3 },
			{ EShip::Destroyer_2, EShip::Destroyer_2, EShip::Destroyer_2 },
			{ EShip::Cutter_1, EShip::Cutter_1, EShip::Cutter_1, EShip::Cutter_1 } };

	struct Helper
	{
		XY StartPosition;
		bool fixedShip;
		bool isActive;
		bool isDirSet;
		EShip::EShip shipType;
		EDirection::EDirection Direction;
		vector<vector<EShip::EShip>> remainedShips = shipsSet;

		void checkShipAvailability(EPart::EPart ** Tablica)
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
					cout << static_cast<char>(EPart::PlaceHolder);
				}
				spInfo::gotoXY();
				SetConsoleTextAttribute(hCONSOLE, 7);//default white
			}
		}

		void reset(void)
		{
			StartPosition.setXY(-1, -1);
			fixedShip = isDirSet = isActive = false;
			shipType = EShip::Random;
			Direction = EDirection::Random;
		}
		void reinforcements(void)
		{
			remainedShips = shipsSet;
		}
		bool destroyCurrentShip(void)
		{
			if(shipType != EShip::Random)
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
		void setDirection(EDirection::EDirection Kierunek)
		{
			Direction = Kierunek;
			isDirSet = true;
		}

		Helper & operator++()
		{
			if(this->shipType != EShip::Battleship_4)
				this->shipType = static_cast<EShip::EShip>(this->shipType + 1);
			return *this;
		}

		Helper & operator--()
		{
			if(this->shipType != EShip::Cutter_1)
				this->shipType = static_cast<EShip::EShip>(this->shipType - 1);
			return *this;
		}

		Helper() : fixedShip(false), isActive(false), isDirSet(false), shipType(EShip::Random),
					 Direction(EDirection::Random) {} //c-tor
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
					helper.Direction = EDirection::Vertical;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != EDirection::Vertical)
					{
						if(helper.shipType == EShip::Cutter_1 && !helper.fixedShip)
							helper.Direction = EDirection::Vertical;
						else
							break;
					}
				}

				if(pos.onTopBorder())
					break;

				EPart::EPart upperTile = BoardToFill[pos.getY() - 1][pos.getX()];

				if(upperTile != EPart::PlaceHolder && upperTile != EPart::Empty)
				{
					if(!(helper.fixedShip && upperTile == EPart::Middle))
						break;
				}

				if(pos.getY() > 0)
				{
					if(pos.getY() <= helper.StartPosition.getY())
					{
						if(helper.shipType != EShip::Battleship_4)
						{
							if(helper.fixedShip && pos.getY() == helper.StartPosition.getY()
									&& BoardToFill[pos.getY()][pos.getX()] == EPart::TopEnd)
							{
								break;
							}
							else
							{
								pos.moveUp();
								helper.StartPosition.setXY(pos);
								BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
								++helper; //statek sie zwieksza
							}
						}
						break;
					}
					if(pos.getY() > helper.StartPosition.getY())
					{
						BoardToFill[pos.getY()][pos.getX()] = EPart::Empty;
						pos.moveUp();
						--helper; //statek siê zmniejsza
						break;
					}
				}
			}
			pos.moveUp();
			break;
		case 80: // v
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = EDirection::Vertical;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != EDirection::Vertical)
					{
						if(helper.shipType == EShip::Cutter_1 && !helper.fixedShip)
							helper.Direction = EDirection::Vertical;
						else
							break;
					}
				}

				if(pos.onBottomBorder())
					break;

				EPart::EPart lowerTile = BoardToFill[pos.getY() + 1][pos.getX()];

				if(lowerTile != EPart::PlaceHolder && lowerTile != EPart::Empty)
				{
					if(!(helper.fixedShip && lowerTile == EPart::Middle))
						break;
				}

				if(pos.getY() < BOARD_SIZE)
				{
					if(pos.getY() > helper.StartPosition.getY())
					{
						if(helper.shipType != EShip::Battleship_4)
						{
							pos.moveDown();
							BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
							++helper;
						}
						break;
					}
					if(pos.getY() <= helper.StartPosition.getY())
					{
						if(helper.shipType == EShip::Cutter_1)
						{
							if(helper.fixedShip && helper.StartPosition.getY() == pos.getY()
									&& BoardToFill[pos.getY()][pos.getX()] == EPart::BotomEnd)
							{
								break;
							}
							else
							{
								pos.moveDown();
								BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
								++helper;
								break;
							}
						}
						else
						{
							BoardToFill[pos.getY()][pos.getX()] = EPart::Empty;
							pos.moveDown();
							helper.StartPosition.setXY(pos);
							--helper;
							break;
						}
					}
				}
			}
			pos.moveDown();
			break;
		case 75:// <--
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = EDirection::Horizontal;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != EDirection::Horizontal)
					{
						if(helper.shipType == EShip::Cutter_1 && !helper.fixedShip)
							helper.Direction = EDirection::Horizontal;
						else
							break;
					}
				}

				if(pos.onLeftBorder())
					break;

				EPart::EPart prevTile = BoardToFill[pos.getY()][pos.getX() - 1];

				if(prevTile != EPart::PlaceHolder && prevTile != EPart::Empty)
				{
					if(!(helper.fixedShip && prevTile == EPart::Middle))
						break;
				}

				if(pos.getX() >= 0)
				{
					if(pos.getX() <= helper.StartPosition.getX())
					{
						if(helper.shipType != EShip::Battleship_4)
						{
							if(helper.fixedShip && helper.StartPosition.getX() == pos.getX()
									&& BoardToFill[pos.getY()][pos.getX()] == EPart::LeftEnd)
							{
								break;
							}
							pos.moveLeft();
							BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
							helper.StartPosition.setXY(pos);
							++helper;
						}
						break;
					}
					if(pos.getX() > helper.StartPosition.getX())
					{
						if(helper.shipType == EShip::Cutter_1)
						{
							pos.moveLeft();
							helper.StartPosition.setXY(pos);
							BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
							break;
						}
						else
						{
							BoardToFill[pos.getY()][pos.getX()] = EPart::Empty;
							pos.moveLeft();
							--helper;
							break;
						}
					}
				}
			}
			pos.moveLeft();
			break;
		case 77: //-->
			if(helper.isActive)
			{
				if(helper.isDirSet == false)
				{
					helper.Direction = EDirection::Horizontal;
					helper.isDirSet = true;
				}
				else
				{
					if(helper.Direction != EDirection::Horizontal)
					{
						if(helper.shipType == EShip::Cutter_1 && !helper.fixedShip)
							helper.Direction = EDirection::Horizontal;
						else
							break;
					}
				}

				if(pos.onRightBorder())
					break;

				EPart::EPart nextTile = BoardToFill[pos.getY()][pos.getX() + 1];

				if(nextTile != EPart::Empty && nextTile != EPart::PlaceHolder)
				{
					if(!(helper.fixedShip && nextTile == EPart::Middle))
						break;
				}

				if(pos.getX() < BOARD_SIZE)
				{
					if(pos.getX() <= helper.StartPosition.getX())
					{
						if(helper.shipType == EShip::Cutter_1)
						{
							if(helper.fixedShip && helper.StartPosition.getX() == pos.getX()
									&& BoardToFill[pos.getY()][pos.getX()] == EPart::RightEnd)
							{
								break;
							}
							pos.moveRight();
							BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
							++helper;
							break;
						}
						else
						{
							BoardToFill[pos.getY()][pos.getX()] = EPart::Empty;
							pos.moveRight();
							helper.StartPosition.setXY(pos);
							--helper;
							break;
						}
					}
					if(pos.getX() > helper.StartPosition.getX())
					{
						if(helper.shipType != EShip::Battleship_4)
						{
							pos.moveRight();
							BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
							++helper;
						}
						break;
					}
				}
			}
			pos.moveRight();
			break;
		case 113:
			if(BoardToFill[pos.getY()][pos.getX()] == EPart::Middle)
				BoardToFill[pos.getY()][pos.getX()] = EPart::Single;
			if(BoardToFill[pos.getY()][pos.getX()] == EPart::Empty)
				BoardToFill[pos.getY()][pos.getX()] = EPart::Middle;
			break;
		case 101:
			BoardToFill[pos.getY()][pos.getX()] = EPart::Striked;
			break;
		case 119:
			BoardToFill[pos.getY()][pos.getX()] = EPart::TopEnd;
			break;
		case 115:
			BoardToFill[pos.getY()][pos.getX()] = EPart::BotomEnd;
			break;
		case 97:
			BoardToFill[pos.getY()][pos.getX()] = EPart::LeftEnd;
			break;
		case 100:
			BoardToFill[pos.getY()][pos.getX()] = EPart::RightEnd;
			break;
		case 32:
			drawShipForced(Solution, BOARD_SIZE);
			prepareStartingPattern(Solution, BOARD_SIZE, StartingPattern);
			for(size_t y = 0; y < BOARD_SIZE; y++)
			{//todo oplaca sie przerobic na i/10 i i%10 ?
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
				{//todo oplaca sie przerobic na i/10 i i%10 ?
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

				BoardToFill[tempy][tempx] = StartingPattern[tempy][tempx];
			}
			helper.reset();
			break;
		case 114: //r
			if(!helper.isActive)
			{
				EPart::EPart tile = BoardToFill[pos.getY()][pos.getX()];
				if(tile != EPart::WaterField && tile != EPart::Middle && tile != EPart::Single)
				{
					helper.StartPosition.setXY(pos);
					helper.shipType = EShip::Cutter_1;
					helper.isActive = true;

					switch (tile)
					{
					case EPart::LeftEnd:
					case EPart::RightEnd:
					{
						helper.fixedShip = helper.isDirSet = true;
						helper.setDirection(EDirection::Horizontal);
					}
					break;
					case EPart::TopEnd:
					case EPart::BotomEnd:
					{
						helper.fixedShip = helper.isDirSet = true;
						helper.setDirection(EDirection::Vertical);
					}
					break;
					case EPart::Empty:
					{
						BoardToFill[pos.getY()][pos.getX()] = EPart::PlaceHolder;
					}
					break;
					default:
					{
					}
					}
				}
			}
			else
			{
				if(helper.Direction == EDirection::Random)
					helper.Direction = EDirection::Horizontal;
				if(!helper.remainedShips[4 - helper.shipType].empty())
				{
					drawShip(BoardToFill, BOARD_SIZE, helper.shipType, helper.Direction, helper.StartPosition, false);
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

		fill(Coordinates[0], Coordinates[0] + BOARD_SIZE * 2, EPart::Empty);
		Coordinates[pos.getX()][0] = EPart::BotomEnd;
		Coordinates[pos.getY()][1] = EPart::RightEnd;

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
