#pragma once
#include "tdef.h"
#include "board.h"

/*
	This file contains constants and variable used by the evaluation routine
*/
namespace Evaluation
{
	// Evaluation Structures
	extern Score psqt[7][120];

	const Score piece_values[] = { 0, 1, 3, 3, 5, 9, 0 }; //	Empty, Pawn, Knight, Bishop, Rook, Queen, King

	// Start of CONSTANTS
		// Pawn
	const Score DiagonalPawn = 0.03f;
	const Score DoublePawn = -0.06f; // two pawns adjacent vertically

		// PSQT
	const Score PSQTWF = 0.08f; // piece square table weighting factor

	// PSQT routines
	extern void CleanPSQT(); // resets PSQT
	inline Score PSQTLookup(Piece pc, Square sq, char side){ return psqt[pc][sq]; } // look up score
}