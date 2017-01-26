#pragma once
#include "tdef.h"

/*
	This file describes the "Move" class which stores all the information about a chess move
	a)	Source Square (byte)
	b)	Destination Square (byte)
	c)	Piece Taken (byte)
	d)	Any special flags (byte) OR the piece for pawn promotions
*/
class Board;

enum MoveFlags : unsigned char
{
	Normal = 0,
	PawnWN = 120,
	PawnWB = 121,
	PawnWR = 122,
	PawnWQ = 123,
	PawnBN = 124,
	PawnBB = 125,
	PawnBR = 126,
	PawnBQ = 127,
	EPW = 128,
	EPB = 129,
	CastleQ = 130,
	CastleK = 131
};

class Move
{
	friend class Board; // Board class can access members
public:
	Move();
	Move(Square src, Square dest);
	Move(Square src, Square dest, Piece pc_prev);
	Move(Square src, Square dest, Piece pc_prev, MoveFlags ppiece_flags);

	std::string ToString();
private:
	Square _src, _dest;
	Piece _pc_prev;

	// This is a special value. For 0 < int < 120, it is the e.p. square
	// -> the move was a pawn double move
	// Otherwise, it is a flag from the MoveFlags enum
	MoveFlags _ppiece_flags;
};



enum MoveFilter
{
	All,
	PromCapt,
	KingCapt
};
