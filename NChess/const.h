#pragma once
#include "tdef.h"

/*
	This file contains a series of constants which make the code more readable.
	None of this code is executable.
	There is a seperate file, eval_const, for evaluation constants
*/

// Square names
const Square A1 = 21;	const Square A2 = 31;	const Square A3 = 41;	const Square A4 = 51; const Square A5 = 61;	const Square A6 = 71;	const Square A7 = 81;	const Square A8 = 91;
const Square B1 = 22;	const Square B2 = 32;	const Square B3 = 42;	const Square B4 = 52; const Square B5 = 62;	const Square B6 = 72;	const Square B7 = 82;	const Square B8 = 92;
const Square C1 = 23;	const Square C2 = 33;	const Square C3 = 43;	const Square C4 = 53; const Square C5 = 63;	const Square C6 = 73;	const Square C7 = 83;	const Square C8 = 93;
const Square D1 = 24;	const Square D2 = 34;	const Square D3 = 44;	const Square D4 = 54; const Square D5 = 64;	const Square D6 = 74;	const Square D7 = 84;	const Square D8 = 94;
const Square E1 = 25;	const Square E2 = 35;	const Square E3 = 45;	const Square E4 = 55; const Square E5 = 65;	const Square E6 = 75;	const Square E7 = 85;	const Square E8 = 95;
const Square F1 = 26;	const Square F2 = 36;	const Square F3 = 46;	const Square F4 = 56; const Square F5 = 66;	const Square F6 = 76;	const Square F7 = 86;	const Square F8 = 96;
const Square G1 = 27;	const Square G2 = 37;	const Square G3 = 47;	const Square G4 = 57; const Square G5 = 67;	const Square G6 = 77;	const Square G7 = 87;	const Square G8 = 97;
const Square H1 = 28;	const Square H2 = 38;	const Square H3 = 48;	const Square H4 = 58; const Square H5 = 68;	const Square H6 = 78;	const Square H7 = 88;	const Square H8 = 98;

// Piece names
const Piece wp = 1;
const Piece wn = 2;
const Piece wb = 3;
const Piece wr = 4;
const Piece wq = 5;
const Piece wk = 6;
const Piece bp = -1;
const Piece bn = -2;
const Piece bb = -3;
const Piece br = -4;
const Piece bq = -5;
const Piece bk = -6;
const Piece empty = 0;
const Piece oob = 7;

// Piece names
const char p_names[] = ".PNBRQKX";

// Piece values
const Score piece_values[] = { 0, 1, 3, 3, 5, 9, 0 };

// Side colours
const signed char White = 1;
const signed char Black = -1;

// Castle sides
const char Kingside = 2;
const char Queenside = 3;

// This is used by the pawn_evaluate routine as list of standard direction vectors
// as well as IsAttacking()
const char svector[] = { -11, -9, 9, 11, 10, -10, -1, 1 };
const char knvector[] = { -21, -19, -12, -8, 8, 12, 19, 21 };

// Maximum search depth
const int MaxDepth = -4;
