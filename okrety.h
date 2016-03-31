#pragma once

#include "StdInc.h"

struct XY;
struct Coord;

namespace spData
{
	enum EPart
	{
		EP1x1 = 4,
		EPMiddle = 254,
		EPLeftEnd = 17,
		EPRightEnd = 16,
		EPTopEnd = 30,
		EPBotomEnd = 31,
		EPWaterField = 176,
		EPStriked = 45,
		EPEmpty = 32,
		EPPlaceHolder = 120
	};

	enum EDirection
	{
		EDRandom = -1,
		EDVertical,
		EDHorizontal
	};

	enum EShip
	{
		ESRandom = 0,
		ES1Cutter,
		ES2Destroyer,
		ES3Cruiser,
		ES4Battleship
	};
};

/* Vector of Pairs containing both XY struct and EDirection Enum*/
typedef std::vector<std::pair<XY, spData::EDirection>> VecPairXY_Dir;

struct XY
{
	XY() : X(-1), Y(-1) {} //c-tor
	XY(size_t p_x, size_t p_y) : X(p_x), Y(p_y) {}
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
   	void setXY(const int x, const int y);

   	bool isValid(size_t Boundary);

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
   	bool isCursonOnBorder();
   	bool isCursorOnTopBorder();
   	bool isCursorOnBottomBorder();
   	bool isCursorOnLeftBorder();
   	bool iscursorOnRightBorder();

private:
	const size_t max;
};

void countShipsParts(const spData::EPart* const *, const size_t, size_t **);
bool drawShip(spData::EPart **, const size_t, spData::EShip, spData::EDirection, XY = XY(-1, -1));
bool drawShipForced(spData::EPart **, const size_t Rozmiar); //Forced draw any ship
void drawBoundary(spData::EPart**, size_t const, spData::EShip, spData::EDirection, size_t, size_t, bool = true);
VecPairXY_Dir * getSth(const spData::EPart* const * Tablica, const size_t Rozmiar, spData::EShip Ship);
void prepareStartingPattern(const spData::EPart* const*, const size_t, spData::EPart **);
