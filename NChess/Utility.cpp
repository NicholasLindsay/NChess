#include "stdafx.h"
#include "Utility.h"
/*
This file contains a list of small utility functions which don't belong anywhere else
*/

// SquareToString(Square) - returns file/rank address of square
std::string SquareToString(Square sq)
{
	std::string str;
	str += (sq % 10) - 1 + 'A';
	str += (sq / 10) - 2 + '1';
	return str;
}

// CharToPiece(Piece) - returns text representation of piece. Returns oob if failed
Piece CharToPiece(char pc)
{
	int black = islower(pc);
	if (black) pc = toupper(pc); // We always look up white pieces and change result at end

	for (int i = 0; i < (sizeof(p_names) / sizeof(p_names[0])); i++)
	{
		if (p_names[i] == pc) return (black ? -i : i);
	}

	return oob;
}

// StringToSquare(str) - returns mailbox index of square or 0 if unsuccesful
Square StringToSquare(std::string str)
{
	if (str.size() != 2) return 0;

	char char0 = toupper(str[0]);
	char char1 = str[1];

	if (char0 < 'A' || char0 > 'H') return 0;
	if (char1 < '1' || char1 > '8') return 0;

	Square sq = (char0 - 'A' + 1);
	sq += (10 * (char1 - '1')) + 20;

	return sq;
}