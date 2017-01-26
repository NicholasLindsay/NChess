#include "stdafx.h"
#include "TTable.h"
#include "board.h"
#include <random>

using namespace TTable;

// Entry in the transposition table
TEntry::TEntry(Zkey _key, Score _score, Square _e_p_sq, unsigned char _castle, char _depth, char side, bool cutoff)
{
	zobrist = _key;
	score = _score;
	e_p_sq = _e_p_sq;
	castle_status = _castle;
	depth = _depth;
	flags = (side == -1) ? BLACK_ACTIVE : ZERO;
	flags = flags | ((cutoff == 1) ? SOFT : ZERO);
}

// Defualt constructor
TEntry::TEntry()
{
	zobrist = 0;
	score = 0;
	e_p_sq = 0;
	castle_status = 0;
	depth = 0;
}

// Transposition/Zobrist table
TZTable::TZTable()
{
	// Create and initilise Zobrist keys

	// The Zobrist table needs 16 different piece types for 128 different squares, each with a 64 bit key
	// 16*128 = 2048 entries * 8 bytes - 16.3kb
	zt = new Zkey[2048];

	// Initilise table
	std::default_random_engine randgen;
	for (int i = 0; i < 2048; i++)
	{
		if ((i & 0xf) == 0) zt[i] = 0; // don't assign zobrist to empty squares
		else zt[i] = randgen() << 32 + randgen();
	}

	// Now create the Transposition Table
	tt = new TEntry[262144]; // 262144 = 2^18
}

// Looks up an entry in the hash table
TEntry* TZTable::FindEntry(Zkey key)
{
	if (tt[key & 0x3ffff].zobrist == key) return &tt[key & 0x3ffff];
	else return nullptr;
}

// Adds an entry to the hash table
void TZTable::AddEntry(Zkey key, Score score, Square ep_square, unsigned char _castle, char depth, char side, bool cutoff)
{
	TEntry *entry_p = &tt[key & 0x3ffff];
	entry_p->zobrist = key;
	entry_p->score = score;
	entry_p->e_p_sq = ep_square;
	entry_p->castle_status = _castle;
	entry_p->depth = depth;
	entry_p->flags = (side == -1) ? TEntry::BLACK_ACTIVE : 0;
	entry_p->flags |= (cutoff ? TEntry::SOFT : 0);
}

// Returns the key associated with a certain piece and square
Zkey TZTable::GetKey(Piece pc, Square sq)
{
	int index = (pc & 0xf) + (sq << 4);
	return zt[index];
}





