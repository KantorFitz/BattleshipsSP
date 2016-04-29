#pragma once

#include "StdInc.h"

struct XY;
struct Coord;

namespace EPart
{
	enum EPart
	{
		Single = 4,
		Middle = 254,
		LeftEnd = 17,
		RightEnd = 16,
		TopEnd = 30,
		BotomEnd = 31,
		WaterField = 176,
		Striked = 45,
		Empty = 32,
		PlaceHolder = 120
	};
}

namespace EDirection
{
	enum EDirection
	{
		Random = -1,
		Vertical,
		Horizontal
	};
}

namespace EShip
{
	enum EShip
	{
		Random = 0,
		Cutter_1,
		Destroyer_2,
		Cruiser_3,
		Battleship_4
	};
}

/* Vector of Pairs containing both XY struct and EDirection Enum*/
typedef std::vector<std::pair<XY, EDirection::EDirection>> VecPairXY_Dir;

struct XY
{
	XY() : X(-1), Y(-1) {} //default c-tor
	XY(int p_x, int p_y) : X(p_x), Y(p_y) {}
	XY(const XY& copy) : X( copy.getX() ), Y( copy.getY() ) {} //copy c-tor
   	size_t getX() const
   	{
   		return X;
   	}
   	size_t getY() const
   	{
   		return Y;
   	}
   	void setX(const int x) { X = x;}
   	void setY(const int y) { Y = y;}
   	void setXY(const XY&);
   	void setXY(const Coord&);
   	void setXY(const int x, const int y);

   	bool isValid(int Boundary);

   	bool operator==(const XY& rhs) const
	{
   		return this->getX() == rhs.getX() && this->getY() == rhs.getY();
	}

   	bool operator!=(const XY& rhs) const
	{
   		return !((*this)==rhs);
	}

protected:
	int X, Y;
};

struct Coord : public XY
{
   	Coord(const size_t mx, int x = 0, int y = 0) : XY(x, y), max(mx) {}
   	void moveDown();
   	void moveUp();
   	void moveLeft();
   	void moveRight();
   	bool onBorder();
   	bool onTopBorder();
   	bool onBottomBorder();
   	bool onLeftBorder();
   	bool onRightBorder();

private:
	const int max;
};

void countShipsParts(const EPart::EPart* const *, const size_t, size_t **);
bool drawShip(EPart::EPart **, const size_t, EShip::EShip, EDirection::EDirection, XY = XY(-1, -1), bool = true);
bool drawShipForced(EPart::EPart **, const size_t Rozmiar); //Forced draw any ship
void drawBoundary(EPart::EPart**, size_t const, EShip::EShip, EDirection::EDirection, size_t, size_t, bool = true);
VecPairXY_Dir * getSth(const EPart::EPart* const * Tablica, const size_t Rozmiar, EShip::EShip Ship);
void prepareStartingPattern(const EPart::EPart* const *, const size_t, EPart::EPart **);
