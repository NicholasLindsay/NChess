#include "stdafx.h"
#include "eval.h"
#include "tdef.h"
#include "Utility.h"

/*
	This file contains the Evaluation routine and corresponding subroutines.
*/
Score Evaluation::psqt[7][120];

// Evaluates the pawn structure for the current side, relative score results
Score Board::EvaluatePawn(Square sq, Piece current_piece)
{
	Score rel_score = 0;

	// This is always calculated going down the board i.e. from rank 8 to rank 1
	// This ensures that each pawn structure property is only included once in the evaluation
	// If both directions were used, pieces would be included twice, once for each pass

	// Check to see if we have diagonal neigbours. If so, are pawn structure
	// is stronger so we should award points
	for (int i = 0; i < 2; i++)
	{
		if (mailbox[sq + svector[i]] == current_piece)
			rel_score += Evaluation::DiagonalPawn;
	}

	// Check for a pawn immeditely behind us. If so, impose double pawn penalty
	if (mailbox[sq - 10] == current_piece)
		rel_score += Evaluation::DoublePawn;

	return rel_score;
}

Score Board::Evaluate()
{
	Score eval_score = 0;

	// Scroll through board
	for (int i = 21; i < 99; i++)
	{
		// Find the piece located in square i
		Piece current_piece = mailbox[i];
		
		// Check to make sure piece is actually a piece
		if (current_piece != empty && current_piece != oob)
		{
			// Find a) the absolute value of the piece
			//      b) which side owns the piece
			Piece abs_piece = PcAbs(current_piece);
			char belongs_to = (current_piece > 0 ? White : Black);

			// abs_score is the score relative to the belongs_to
			Score abs_score = 0; 

			// Individual piece subroutines
			if (abs_piece == wp) abs_score += EvaluatePawn(i, current_piece);

			// Look up the piece-square table and the value table
			abs_score += Evaluation::PSQTLookup(abs_piece, i, belongs_to);
			abs_score += Evaluation::piece_values[abs_piece];

			// Add the score relative to White, as opposed to colour of piece
			eval_score += (current_piece > 0) ? abs_score : -abs_score;
		}
	}

	return eval_score;
}

void Evaluation::CleanPSQT()
{
	// Simple for this now, gives every piece same weighting
	for (int i = 20; i < 100; i += 10)
	{
		for (int j = 1; j < 9; j++)
		{
			for (int k = 0; k <= 6; k++)
			{
				// Work out distances from center
				double x_distance = abs(4.5 - j);
				double y_distance = abs(4.5 - 0.1 * (i - 10)); // 0.5 - 3.5

				// Calculate Manhatten distance
				double manhattan_distance = x_distance + y_distance;

				// Favour good positions: negate distance to penalise long distance and add 3.5 to zero
				double score = 3.5 - x_distance + 3.5 - y_distance;

				// Apply weighting factor and save score
				psqt[k][i + j] = PSQTWF * score;
			}
		}
	}
}