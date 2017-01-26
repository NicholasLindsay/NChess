#pragma once
#include "tdef.h"

class Board;

namespace TTable
{
	// Entry in the transposition table
	struct TEntry
	{
		Zkey zobrist;
		Score score;
		Square e_p_sq;
		unsigned char castle_status;
		char depth;

		enum EntryFlags : uint8_t
		{
			ZERO = 0,
			BLACK_ACTIVE = 1,
			SOFT = 2 // Score is equal or greater to value scored
		};

		uint8_t flags;

		TEntry(Zkey _key, Score _score, Square _e_p_sq, unsigned char _castle, char _depth, char side, bool cutoff);
		TEntry();
		
		inline int ActivePlayer() { return (flags && 0x1) ? -1 : 1; };
	};

	// Transposition table and Zobrist table
	class TZTable
	{
	public:
		TZTable(); // constructor

		TEntry* FindEntry(Zkey); // Find entry in table corresponding to the current board
		void AddEntry(Zkey key, Score score, Square ep_square, unsigned char _castle, char depth, char side, bool cutoff);
		Zkey GetKey(Piece pc, Square sq);
		
	private:
		TEntry* tt; // Transposition table
		Zkey* zt; // Key table
	};
}