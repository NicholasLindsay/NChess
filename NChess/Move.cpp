#include "stdafx.h"
#include "move.h"
#include "const.h"
#include "tdef.h"
#include "Utility.h"
#include <string>

Move::Move()
{
	_src = 0;
	_dest = 0;
	_pc_prev = 0;
	_ppiece_flags = MoveFlags::Normal;
}
Move::Move(Square src, Square dest)
{
	_src = src;
	_dest = dest;
	_pc_prev = 0;
	_ppiece_flags = MoveFlags::Normal;
}
Move::Move(Square src, Square dest, Piece pc_prev)
{
	_src = src;
	_dest = dest;
	_pc_prev = pc_prev;
	_ppiece_flags = MoveFlags::Normal;
}
Move::Move(Square src, Square dest, Piece pc_prev, MoveFlags ppiece_flags)
{
	_src = src;
	_dest = dest;
	_pc_prev = pc_prev;
	_ppiece_flags = ppiece_flags;
}

// Returns simple algebraic notation of move eg A1C3 or O-O-O
// NEEEEEEEDDDDDSSSSS MORE WORK
std::string Move::ToString()
{
	// Check for king and queenside castle
	if (_ppiece_flags == MoveFlags::CastleK) return "O-O";
	if (_ppiece_flags == MoveFlags::CastleQ) return "O-O-O";

	std::string ostring = SquareToString(_src);
	ostring += SquareToString(_dest);

	// Check for promotions
	if (_ppiece_flags == MoveFlags::PawnWB) ostring += "B";
	else if (_ppiece_flags == MoveFlags::PawnWN) ostring += "N";
	else if (_ppiece_flags == MoveFlags::PawnWQ) ostring += "Q";
	else if (_ppiece_flags == MoveFlags::PawnWR) ostring += "R";
	else if (_ppiece_flags == MoveFlags::PawnBB) ostring += "b";
	else if (_ppiece_flags == MoveFlags::PawnBN) ostring += "n";
	else if (_ppiece_flags == MoveFlags::PawnBQ) ostring += "q";
	else if (_ppiece_flags == MoveFlags::PawnBR) ostring += "r";

	// Check for en_passant
	if (_ppiece_flags == MoveFlags::EPB || _ppiece_flags == MoveFlags::EPW) ostring += "e.p.";

	return ostring;
}
