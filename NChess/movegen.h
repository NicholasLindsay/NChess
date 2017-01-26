#pragma once
#include "move.h"

// This file contains a bunch of stuff used by the move generator

// PieceRule - tells the move generator details about the piece such as
//				a) List of direction vectors - null terminated
//				b) Maximum run length
//				c) More to come as I think of them
struct PieceRule
{
	const char* direction_vectors;
	int max_run_length;

	PieceRule(const char* dv, const int max_run)
	{
		direction_vectors = dv;
		max_run_length = max_run;
	}
};

// Arrays for different types of direction vectors
const char nvector[] = { -21, -19, -12, -8, 8, 12, 19, 21, 0 };
const char bvector[] = { -11, -9, 9, 11, 0 };
const char rvector[] = {10, -10, -1, 1, 0 };
const char qkvector[] = { -11, -9, 9, 11, 10, -10, -1, 1, 0 };

// Definition of PieceRules
PieceRule PieceRuleArray[] = {PieceRule(nullptr,0), // Empty square
							  PieceRule(nullptr,0), // Pawn is special - ignore it
							  PieceRule(nvector,1), // Knight moves in kvector directions max of 1
							  PieceRule(bvector,9), // Bishop moves in bvector directions with no max
							  PieceRule(rvector,9), // Rook moves in rvector directions with no max
							  PieceRule(qkvector,9), // Queen moves in qkvector direction with no max
							  PieceRule(qkvector,1)}; // King moves in qkvector direction with max of 1