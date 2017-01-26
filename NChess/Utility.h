#pragma once
#include "tdef.h"
#include "const.h"

/*
	This file contains a list of small utility functions which don't belong anywhere else
*/

// PieceToString(Piece) - returns standard text representation of piece.
inline char PieceToChar(Piece pc)
{
	return (pc >= 0) ? p_names[pc] : (p_names[-pc] + 32);
}

// CharToPiece(char) - returns piece corresponding to char, or oob if unsuccessful
Piece CharToPiece(char pc);

// SquareToString(Square) - returns file/rank address of square
extern std::string SquareToString(Square sq);

// StringToSquare(str) - returns the Mailbox square denoted by the string
Square StringToSquare(std::string str);

// RelPc(pc, side) - returns the piece relative to current player
inline Piece RelPC(Piece pc, signed char side)
{
	return (side >= White) ? pc : -pc;
}

// IsOwn(pc, side) - checks to see if piece belongs to player
inline int IsOwn(Piece pc, signed char side)
{
	return (side >= White) ? (pc > 0 && pc != oob) : (pc < 0);
}

// PcMul(pc, side) - if black flips sign of piece
inline Piece PcMul(Piece pc, signed char side)
{
	return (side >= White) ? pc : -pc;
}

// PcAbs(pc) - returns White equivalent of piece
inline Piece PcAbs(Piece pc)
{
	return (pc >= 0) ? pc : -pc;
}

// RelFlip(Square, side) - if black, returns transform of square under rotation of 180 degrees
inline Square RelFlip(Square sq, signed char side)
{
	return (side >= White) ? sq : (119 - sq);
}

// GetSide(pc) - returns which player the piece belongs too
inline signed char GetSide(Piece pc)
{
	if (pc > 0 && pc != oob) return White;
	if (pc < 0) return Black;
	return 0;
}

// GetRank(sq) - returns the rank of the selected square
inline signed char GetRank(Square sq)
{
	return (sq / 10) - 1;
}

// GetFile(sq) - returns the file of the selected square
inline signed char GetFile(Square sq)
{
	return (sq % 10);
}