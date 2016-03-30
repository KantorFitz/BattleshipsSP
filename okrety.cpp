#include "okrety.h"

void XY::setXY(const XY& copy)
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

bool Coord::isCursorOnTopBorder()
{
	return Y == 0;
}

bool Coord::isCursorOnBottomBorder()
{
	return Y == max - 1;
}

bool Coord::isCursorOnLeftBorder()
{
	return X == 0;
}

bool Coord::iscursorOnRightBorder()
{
	return X == max - 1;
}

bool Coord::isCursonOnBorder()
{
	return isCursorOnLeftBorder() || iscursorOnRightBorder() || isCursorOnTopBorder()
			|| isCursorOnBottomBorder();
}

VecPairXY_Dir * getSth(const spData::EPart* const * Tablica, const size_t Rozmiar, spData::EShip Ship)
{
	bool isThereAnyPlaceAvailable = false;
	for (size_t y = 0; y < Rozmiar; y++)// Szukamy jakiegokolwiek wolnego miejsca tylko dla pola 1x1
	{
		for(size_t x = 0; x < Rozmiar; x++)
		{
			if((Tablica[y][x] == spData::EPEmpty) || (Tablica[y][x] == spData::EPPlaceHolder))
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
				if((Tablica[kierunek ? i : x][kierunek ? x : i] == spData::EPEmpty)
						|| (Tablica[kierunek ? i : x][kierunek ? x : i] == spData::EPPlaceHolder))
				{ /*czy statek zmieœci siê zaczynaj¹c od tego miejsca (x)*/
					if(x < Rozmiar - Ship + 1)
						tempPlaces.push_back(x);
				}
			}

			for (size_t j = 0; j < tempPlaces.size(); j++)
			{
				for (size_t k = tempPlaces[j]; k < tempPlaces[j] + Ship; k++)
				{
					if((Tablica[kierunek ? i : k][kierunek ? k : i] == spData::EPEmpty)
							|| (Tablica[kierunek ? i : k][kierunek ? k : i] == spData::EPPlaceHolder))
					{
						if(k == tempPlaces[j] + Ship - 1)
						{
							size_t foo = kierunek ? tempPlaces[j] : i;
							size_t bar = kierunek ? i : tempPlaces[j];
							Ret->push_back(std::make_pair(XY(foo, bar), (spData::EDirection) kierunek));
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

void countShipsParts(const spData::EPart* const* Tablica, const size_t Rozmiar, size_t ** Wartosci)
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
			if(Tablica[i][j] != spData::EPEmpty && Tablica[i][j] != spData::EPStriked
					&& Tablica[i][j] != spData::EPWaterField)
			{
				Wartosci[0][j]++;
				Wartosci[1][i]++;
			}
		}
	}
}

void drawBoundary(spData::EPart** Tablica, size_t const Rozmiar, spData::EShip Statek,
		spData::EDirection Kierunek, size_t XPos, size_t YPos, bool oneTileOnly /* = true */)
{
	if(Statek == spData::ESRandom || Kierunek == spData::EDRandom)
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
				if(Tablica[y][x] == spData::EPEmpty || Tablica[y][x] == spData::EPPlaceHolder)
					Tablica[y][x] = spData::EPStriked;
			}
		}
		if(oneTileOnly)
			return;
		Kierunek ? ++XPos : ++YPos;
	}
}

bool drawShip(spData::EPart ** Tablica, size_t const Rozmiar, spData::EShip Statek,
		spData::EDirection Kierunek, XY posXY /* = ( -1, -1 ) */)//Edirection == EDRandom znaczy sprawdzenie w dwóch kierunkach
{
	if(Statek == spData::ESRandom)
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
		if(Kierunek != spData::EDRandom)
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
		if(Statek != spData::ES1Cutter)
		{
			if(i == 0)
			{
				Tablica[VerStart][HorStart] = Kierunek ? spData::EPLeftEnd : spData::EPTopEnd;

				drawBoundary(Tablica, Rozmiar, Statek, Kierunek, HorStart, VerStart);
				continue;
			}
			else if(i == (size_t) Statek - 1)
			{
				Tablica[tempy][tempx] = Kierunek ? spData::EPRightEnd : spData::EPBotomEnd;

				drawBoundary(Tablica, Rozmiar, Statek, Kierunek, tempx, tempy);
				continue;
			}
		}
		Tablica[tempy][tempx] = spData::EPMiddle;

		drawBoundary(Tablica, Rozmiar, Statek, Kierunek, tempx, tempy);
	}

	return true;
}

bool drawShipForced(spData::EPart ** Tablica, const size_t Rozmiar)
{
	std::vector<spData::EShip> shipsSet = { spData::ES4Battleship,
			spData::ES3Cruiser,
			spData::ES3Cruiser,
			spData::ES2Destroyer,
			spData::ES2Destroyer,
			spData::ES2Destroyer,
			spData::ES1Cutter,
			spData::ES1Cutter,
			spData::ES1Cutter,
			spData::ES1Cutter };

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
			spData::EDirection Kierunek = foobar->at(randIndex).second;
			drawShip(Tablica, Rozmiar, shipsSet[0], Kierunek, foobar->at(randIndex).first);
			delete foobar;
		}
		shipsSet.erase(shipsSet.begin());
	}

	return true;
}
