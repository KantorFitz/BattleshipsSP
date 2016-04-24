#include "okrety.h"

void XY::setXY(const XY& copy)
{
	X = copy.getX();
	Y = copy.getY();
}

void XY::setXY(const Coord& copy)
{
	X = copy.getX();
	Y = copy.getY();
}

void XY::setXY(const int x, const int y)
{
	X = x;
	Y = y;
}

bool XY::isValid(size_t Boundary)
{
	return X < Boundary && Y < Boundary && X > -1 && Y > -1;
}

void Coord::moveDown()
{
	if (Y >= max - 1)
		Y = 0;
	else
		++Y;
}

void Coord::moveUp()
{
	if (Y == 0)
		Y = max - 1;
	else
		--Y;
}

void Coord::moveLeft()
{
	if (X == 0)
		X = max - 1;
	else
		--X;
}

void Coord::moveRight()
{
	if (X >= max - 1)
		X = 0;
	else
		++X;
}

bool Coord::onTopBorder()
{
	return Y == 0;
}

bool Coord::onBottomBorder()
{
	return Y == max - 1;
}

bool Coord::onLeftBorder()
{
	return X == 0;
}

bool Coord::onRightBorder()
{
	return X == max - 1;
}

bool Coord::onBorder()
{
	return onLeftBorder() || onRightBorder() || onTopBorder() || onBottomBorder();
}

VecPairXY_Dir * getSth(const EPart::EPart* const * Tablica, const size_t Rozmiar, EShip::EShip Ship)
{
	bool isThereAnyPlaceAvailable = false;
	for (size_t y = 0; y < Rozmiar; y++)// Szukamy jakiegokolwiek wolnego miejsca tylko dla pola 1x1
	{
		for(size_t x = 0; x < Rozmiar; x++)
		{
			if((Tablica[y][x] == EPart::Empty) || (Tablica[y][x] == EPart::PlaceHolder))
			{
				isThereAnyPlaceAvailable = true;
				break;
			}
		}
		if(isThereAnyPlaceAvailable)
			break;
	}

	if(!isThereAnyPlaceAvailable)
		return nullptr;

	/*Wszystkie mo¿liwe pozycje dla danego statku w obu kierunkach*/
	VecPairXY_Dir * Ret = new VecPairXY_Dir;
	Ret->reserve(140);
	std::vector<size_t> tempPlaces;

	short kierunek = 2;
	while(kierunek--)
	{
		for (size_t i = 0; i < Rozmiar; i++)
		{ /*Zebranie wszystkich pozycji zawieraj¹cych puste pole w obu kierunkach*/
			for (size_t x = 0; x < Rozmiar; x++)
			{
				if((Tablica[kierunek ? i : x][kierunek ? x : i] == EPart::Empty)
						|| (Tablica[kierunek ? i : x][kierunek ? x : i] == EPart::PlaceHolder))
				{ /*czy statek zmieœci siê zaczynaj¹c od tego miejsca (x)*/
					if(x < Rozmiar - Ship + 1)
						tempPlaces.push_back(x);
				}
			}

			for (size_t j = 0; j < tempPlaces.size(); j++)
			{
				for (size_t k = tempPlaces[j]; k < tempPlaces[j] + Ship; k++)
				{
					if((Tablica[kierunek ? i : k][kierunek ? k : i] == EPart::Empty)
							|| (Tablica[kierunek ? i : k][kierunek ? k : i] == EPart::PlaceHolder))
					{
						if(k == tempPlaces[j] + Ship - 1)
						{
							size_t foo = kierunek ? tempPlaces[j] : i;
							size_t bar = kierunek ? i : tempPlaces[j];
							Ret->push_back(std::make_pair(XY(foo, bar), static_cast<EDirection::EDirection>(kierunek)));
						}
					}
					else
						break;
				}
			}
			tempPlaces.clear();
		}
	}

	if(Ret->size())
		return Ret;
	else
		return nullptr;
}

void countShipsParts(const EPart::EPart* const* Tablica, const size_t Rozmiar, size_t ** Wartosci)
{
	for (size_t i = 0; i < 2; i++)	//zerowanie tablicy
	{
		for (size_t j = 0; j < Rozmiar; j++)
			Wartosci[i][j] = 0;
	}

	for (size_t i = 0; i < Rozmiar; i++)
	{
		for (size_t j = 0; j < Rozmiar; j++)
		{
			if(Tablica[i][j] != EPart::Empty && Tablica[i][j] != EPart::Striked
					&& Tablica[i][j] != EPart::WaterField)
			{
				Wartosci[0][j]++;
				Wartosci[1][i]++;
			}
		}
	}
}

void drawBoundary(EPart::EPart** Tablica, size_t const Rozmiar, EShip::EShip Statek,
		EDirection::EDirection Kierunek, size_t XPos, size_t YPos, bool oneTileOnly /* = true */)
{
	if(Statek == EShip::Random || Kierunek == EDirection::Random)
		return;

	for (short loop = 0; loop < Statek; loop++)
	{					/*Je¿eli YPos > 0, to zaczynamy wczeœniej*/
		for (size_t y = (YPos ? YPos - 1 : 0); y < YPos + 2; y++)
		{
			if(!(y < Rozmiar))
				continue;//Analogicznie jak wy¿ej
			for (size_t x = (XPos ? XPos - 1 : 0); x < XPos + 2; x++)
			{
				if(!(x < Rozmiar))
					continue;
				if(Tablica[y][x] == EPart::Empty || Tablica[y][x] == EPart::PlaceHolder)
					Tablica[y][x] = EPart::Striked;
			}
		}
		if(oneTileOnly)
			return;
		Kierunek ? ++XPos : ++YPos;
	}
}

bool drawShip(EPart::EPart ** Tablica, size_t const Rozmiar, EShip::EShip Statek,
		EDirection::EDirection Kierunek, XY posXY /* = ( -1, -1 ) */)//Edirection == EDRandom znaczy sprawdzenie w dwóch kierunkach
{
	if(Statek == EShip::Random)
		return false;

	VecPairXY_Dir * foobar;
	std::random_device rd;
	std::mt19937 g(rd());

	size_t HorStart, VerStart;
	XY tempXY;

	/*Pobieranie wszystkich miejsc dla danego statku*/
	foobar = getSth(Tablica, Rozmiar, Statek);
	if(foobar)	//je¿eli jakieœ s¹...
	{	/* je¿eli kierunek jest ustalony, wywalamy wszystkie opcje nie zgadzaj¹ce siê z nim*/
		if(Kierunek != EDirection::Random)
		{
			auto i = std::begin(*foobar);
			while(i != std::end(*foobar))
			{
				if((i->second != Kierunek) || ((posXY.isValid(Rozmiar)) && (i->first != posXY)))
					i = foobar->erase(i);
				else
					i++;
			}
#	ifdef printAdditionalInfo
			i = std::begin(*foobar);
			while(i != std::end(*foobar))
			{
				printf("XY(%i, %i) - %s\n", i->first.getX(), i->first.getY(), i->second ? "Hor" : "Ver");
				i++;
			}
#	endif
		}
		if(!foobar->empty())
		{
			std::shuffle(foobar->begin(), foobar->end(), g);
			size_t randIndex = std::rand() % foobar->size();
			Kierunek = foobar->at(randIndex).second;
			tempXY.setXY(foobar->at(randIndex).first);
		}
		else
			return false;
		delete foobar;
	}
	else
		return false;

#	ifdef printAdditionalInfo
	printf("Kierunek(%s), tempXY(%i)(%i).\n", Kierunek ? "EDHorizontal" : "EDVertical", tempXY.getX(),
			tempXY.getY());
#	endif

	HorStart = tempXY.getX();
	VerStart = tempXY.getY();
	for (size_t i = 0; i < Statek; i++)
	{
		size_t tempx = (Kierunek ? HorStart + i : HorStart);
		size_t tempy = (Kierunek ? VerStart : VerStart + i);
		if(Statek != EShip::Cutter_1)
		{
			if(i == 0)
			{
				Tablica[VerStart][HorStart] = Kierunek ? EPart::LeftEnd : EPart::TopEnd;

				drawBoundary(Tablica, Rozmiar, Statek, Kierunek, HorStart, VerStart);
				continue;
			}
			else if(i == (size_t) Statek - 1)
			{
				Tablica[tempy][tempx] = Kierunek ? EPart::RightEnd : EPart::BotomEnd;

				drawBoundary(Tablica, Rozmiar, Statek, Kierunek, tempx, tempy);
				continue;
			}
		}
		if(Statek == EShip::Cutter_1)
			Tablica[tempy][tempx] = EPart::Single;
		else
			Tablica[tempy][tempx] = EPart::Middle;

		drawBoundary(Tablica, Rozmiar, Statek, Kierunek, tempx, tempy);
	}

	return true;
}

bool drawShipForced(EPart::EPart ** Tablica, const size_t Rozmiar)
{
	std::vector<EShip::EShip> shipsSet = { EShip::Battleship_4,
			EShip::Cruiser_3,
			EShip::Cruiser_3,
			EShip::Destroyer_2,
			EShip::Destroyer_2,
			EShip::Destroyer_2,
			EShip::Cutter_1,
			EShip::Cutter_1,
			EShip::Cutter_1,
			EShip::Cutter_1 };

	VecPairXY_Dir * foobar;

	std::random_device rd;
	std::mt19937 g(rd());

	while(!shipsSet.empty())
	{
		foobar = getSth(Tablica, Rozmiar, shipsSet[0]);
		if(foobar)
		{
			std::shuffle(foobar->begin(), foobar->end(), g);
			size_t randIndex = std::rand() % foobar->size();
			EDirection::EDirection Kierunek = foobar->at(randIndex).second;
			drawShip(Tablica, Rozmiar, shipsSet[0], Kierunek, foobar->at(randIndex).first);
			delete foobar;
		}
		shipsSet.erase(shipsSet.begin());
	}

	std::vector<XY> waterFields;
	for(size_t y = 0; y < Rozmiar; y++)
	{
		for(size_t x = 0; x < Rozmiar; x++)
		{
			EPart::EPart tile = Tablica[y][x];
			if(tile == EPart::Striked || tile == EPart::Empty)
				waterFields.push_back(XY(x, y));
		}
	}
	std::shuffle(waterFields.begin(), waterFields.end(), g);
	for(int i = 0; i < 4; i++)
	{
		if(rand() % 100 >= 75)	//we have 25% chance to generate any water field
			Tablica[waterFields[0].getY()][waterFields[0].getX()] = EPart::WaterField;
		waterFields.erase(waterFields.begin());
	}

	return true;
}

void prepareStartingPattern(const EPart::EPart* const * Tablica, const size_t Rozmiar, EPart::EPart** Pattern)
{
	std::random_device rd;
	std::mt19937 g(rd());

	std::vector<XY> startingTiles;
	EPart::EPart tile;
	for(size_t y = 0; y < Rozmiar; y++)
	{
		for(size_t x = 0; x < Rozmiar; x++)
		{
			tile = Tablica[y][x];
			if(tile != EPart::Empty && tile != EPart::Striked && tile != EPart::PlaceHolder)
			{
				startingTiles.push_back(XY(x, y));
				if(tile == EPart::WaterField)
					Pattern[y][x] = tile;
			}
		}
	}

	auto iter = startingTiles.begin();
	while(iter != startingTiles.end())
	{
		tile = Tablica[iter->getY()][iter->getX()];
		if(tile == EPart::RightEnd || tile == EPart::BotomEnd)
			iter = startingTiles.erase(iter);
		else
			iter++;
	}
	std::shuffle(startingTiles.begin(), startingTiles.end(), g);
	startingTiles.erase(startingTiles.begin() + 4, startingTiles.end());

	for(const auto & a : startingTiles)
	{
		Pattern[a.getY()][a.getX()] = Tablica[a.getY()][a.getX()];
	}
}

