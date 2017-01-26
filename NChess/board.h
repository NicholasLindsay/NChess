#pragma once
#include "tdef.h"
#include "const.h"
#include "move.h"
#include "Utility.h"
#include <list>
#include <stack>
#include <memory>

namespace TTable
{
	extern class TZTable;
}

/*
This file is for the Board class which describes every component of the current position relative to the rules:
1.	Mailbox representation of board
2.	Castling rights
3.	Movecount
4.	En passant squares

The board class is typically passed as an argument during search, so should aim to minimise space to improve speed.
*/

// This is used to keep track of search information such as:
//		node count
//		a-b cut-offs
struct SearchStats
{
	int* node_count;
	int* ab_cutoffs;

	int q_nodes;
	int q_cutoffs;

	int tt_hits;
	int tt_misses;

	SearchStats()
	{
		node_count = new int[20];
		ab_cutoffs = new int[20];

		Clear();
	}

	void Clear()
	{
		for (int i = 0; i < 20; i++)
		{
			node_count[i] = 0;
			ab_cutoffs[i] = 0;
		}

		q_nodes = 0;
		q_cutoffs = 0;
		tt_hits = 0;
		tt_misses = 0;
	}


};

class Board
{
public:
	Board();
	~Board();

	// Don't allow copy/move:
	Board(const Board&) = delete;
	void operator=(const Board&) = delete;

	// Board setup
	void Clean();
	int FromFEN(std::string fen);

	// Status commands
	int CanCastle(signed char pl_side, char cs_side); // pl_side = white or black, cs_side = kingside or queenside
	void SetCastle(signed char pl_side, char cs_side);
	void UnsetCastle(signed char pl_side, char cs_side);
	int IsAttacking(Square sq, signed char side_attacking); // Sees if square is attacked by side_attacking
	int InCheck(signed char side); // returns 1 if side is in check
	signed char ActiveSide();

	// Move and evaluate
	void DoMove(Move mv);
	void UndoMove();
	Score Evaluate();
	
	// Move Generation
	int MoveGen(std::list<Move> &MoveList, MoveFilter filter, char side);

	// Search
	Score ABSearch(int depth, Score alpha, Score beta, SearchStats &stats);
	Move BestMoveSearch(int depth, SearchStats &stats);
	Score QSearch(Score alpha, Score beta, SearchStats &stats);

	// User Interface
	std::string ToString();
	std::string ToFEN();


private:
	// Mailbox -> mailbox representation of chessboard
	Mailbox mailbox;

	struct Status {
		Square ep_square;
		char cap_ply; // half moves without capture or pawn
		signed char active_side;
		int cur_move_cnt; // starts at 1, increments after black moves
		unsigned char castle_rights;	// QK..qk.. 1 = right remains
	} status;	

	// Status and move history:
	std::stack<Move> move_history;
	std::stack<Status> status_history;

	//TTable::TZTable* tzt;
	//Zkey zobrist; // our current key
	
	// Evaluation functions
	Score EvaluatePawn(Square sq, signed char side);

	// Zobrist
	//void CreateZobrist(); // Creates a zobrist key to represent the board
	//void Board::UpdateZobrist(Piece pc, Square sq); // Updates the zobrist key

	// Utility
	void PromotePawn(Piece pc, Square sq);
};