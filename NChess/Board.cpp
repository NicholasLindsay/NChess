#include "stdafx.h"
#include "board.h"
#include "movegen.h"
#include "TTable.h"
#include <sstream>


Board::Board()
{
	mailbox = new Piece[120];

	// TODO: Create the tztable
	//tzt = new TTable::TZTable();

	// Then simply call clean to initilise the board, we are done!
	Clean();
}

Board::~Board()
{
	delete[] mailbox;
}

// Clean() resets chess board to starting position, white to move
void Board::Clean()
{
	// First off, set all squares to out of bounds
	for (int i = 0; i < 120; i++)
	{
		mailbox[i] = oob;
	}

	// Fill in board
	for (int i = 40; i < 80; i += 10)
	{
		for (int j = 1; j < 9; j++)
		{
			mailbox[i + j] = empty;
		}
	}

	// Put in pieces
	mailbox[91] = br;	mailbox[92] = bn;	mailbox[93] = bb;	mailbox[94] = bq;	mailbox[95] = bk;	mailbox[96] = bb;	mailbox[97] = bn;	mailbox[98] = br;
	mailbox[81] = bp;	mailbox[82] = bp;	mailbox[83] = bp;	mailbox[84] = bp;	mailbox[85] = bp;	mailbox[86] = bp;	mailbox[87] = bp;	mailbox[88] = bp;
	// ...
	mailbox[31] = wp;	mailbox[32] = wp;	mailbox[33] = wp;	mailbox[34] = wp;	mailbox[35] = wp;	mailbox[36] = wp;	mailbox[37] = wp;	mailbox[38] = wp;
	mailbox[21] = wr;	mailbox[22] = wn;	mailbox[23] = wb;	mailbox[24] = wq;	mailbox[25] = wk;	mailbox[26] = wb;	mailbox[27] = wn;	mailbox[28] = wr;

	// castlerights, enpassant square and movecount
	status.cap_ply = 0;
	status.castle_rights = 0xcc;
	status.active_side = 1;
	status.ep_square = 0;
	status.cur_move_cnt = 1;

	//CreateZobrist();

	return;
}

// FromFEN() loads the Board object with data from a FEN string. Returns -1 if unsuccessful
int Board::FromFEN(std::string fen)
{
	std::stringstream sstream;
	sstream << fen;

	// First, oob initilise board
	for (int i = 0; i < 120; i++) mailbox[i] = oob;

	// First block of text we read out is the position data
	std::string pos_data;
	sstream >> std::skipws >> pos_data;

	int row = 90;
	int col = 1;

	for (unsigned int i = 0; i < pos_data.size(); i++)
	{
		if (row < 20) return -1; // too many rows

		if (pos_data[i] == '/' && col == 9)
		{
			col = 1;
			row -= 10;
		}
		else if (col == 9 || pos_data[i] == '/') return -1; // no slash/ slash in wrong place
		else if (pos_data[i] >= '1' && pos_data[i] <= '9')
		{
			for (int j = pos_data[i] - '0'; j > 0; j--)
			{
				if (col >= 9) return -1; // too many empties
				mailbox[col++ + row] = empty;
			}
		}
		else
		{
			Piece pc = CharToPiece(pos_data[i]);
			if (pc == oob) return -1;
			mailbox[col++ + row] = pc;
		}
	}

	if (row != 20) return -1;

	// The next string is the side to move
	std::string stm;
	sstream >> stm;

	// Next is castling info
	std::string castles;
	sstream >> castles;

	if (castles.size() == 0) return -1;

	status.castle_rights = 0;

	// If castle rights do exist
	if (castles != "-")
	{
		for (unsigned int i = 0; i < castles.size(); i++)
		{
			if (castles[i] == 'K') SetCastle(White, Kingside);
			else if (castles[i] == 'Q') SetCastle(White, Queenside);
			else if (castles[i] == 'k') SetCastle(Black, Kingside);
			else if (castles[i] == 'q') SetCastle(Black, Queenside);
			else return -1;
		}
	}

	if (stm == "w" || stm == "W") status.active_side = White;
	else if (stm == "b" || stm == "B") status.active_side = Black;
	else return -1;

	// Next up, ep square
	std::string eps;
	sstream >> eps;

	if (eps.size() == 0) return -1;

	if (eps == "-")
	{
		status.ep_square = 0;
	}
	else
	{
		status.ep_square = StringToSquare(eps);
		if (status.ep_square == 0) return -1;
	}
	
	// Now, the plycount and movecount
	std::string plycounts, movecounts;
	sstream >> plycounts >> movecounts;

	if (plycounts.size() == 0 || movecounts.size() == 0) return -1;

	status.cap_ply = atoi(plycounts.c_str());
	status.cur_move_cnt = atoi(movecounts.c_str());

	//CreateZobrist();

	return 0;
}

// CanCastle(pl_side, cs_side) - can 'pl' castle on 'cs' side?
int Board::CanCastle(signed char pl_side, char cs_side)
{
	char mask = (cs_side == Kingside) ? 4 : 8;
	if (pl_side == White) mask = mask << 4;

	return (mask & status.castle_rights);
}

// SetCastle(pl_side, cs_side) - pl can castle on cs side
void Board::SetCastle(signed char pl_side, char cs_side)
{
	char mask = ((cs_side == Kingside) ? 4 : 8);
	if (pl_side == White) status.castle_rights |= (mask << 4);
	else if (pl_side == Black) status.castle_rights |= mask;
}

// UnsetCastle(pl_side, cs_side) - pl can not castle on cs side
void Board::UnsetCastle(signed char pl_side, char cs_side)
{
	char mask = ~((cs_side == Kingside) ? 4 : 8);
	if (pl_side == White) status.castle_rights &= ((mask << 4) + 15);
	else if (pl_side == Black) status.castle_rights &= mask;
}

// IsAttacking(Sq, side) - returns 1 if sq is attacked by side, 0 otherwise
int Board::IsAttacking(Square sq, signed char side)
{
	// Check for attack by slider pieces
	// For each direction
	for (int index = 0; index < 8; index++)
	{
		Square temp_sq = sq;
		// Used for detecting king moves as they can only move by one
		char first_move = 1;

		// Slide in direction, looking for sliding pieces
		while (true)
		{
			temp_sq += svector[index];
			Piece dest_pc = mailbox[temp_sq];
			Piece abs_pc = PcAbs(dest_pc);

			// If piece is oob, stop searching in this direction
			if (dest_pc == oob) break;

			// If the piece is ours
			else if (IsOwn(dest_pc, side))
			{
				if ((abs_pc == wb || abs_pc == wq) && (index < 4)) return 1;
				if ((abs_pc == wr || abs_pc == wq) && (index >= 4)) return 1;
				else if (abs_pc == wk && first_move) return 1;
			}

			// Otherwise, it is our opponents
			else if (dest_pc != empty) break;

			// We have searched at least one increment in direction - no longer one move away
			first_move = 0;
		}
	}

	// Check for knight attacks
	for (int index = 0; index < 8; index++)
	{
		if (RelPC(mailbox[sq + knvector[index]], side) == wn) return 1;
	}

	// Check for pawn attacks
	int pawn_direction = (side == White) ? -11 : 11;
	if (RelPC(mailbox[sq + pawn_direction], side) == wp) return 1;
	pawn_direction = (side == White) ? -9 : 9;
	if (RelPC(mailbox[sq + pawn_direction], side) == wp) return 1;

	// Otherwise, square not attacked
	return 0;
}

// InCheck(side) - returns 1 if side is in check
int Board::InCheck(signed char side)
{
	// First, find the king
	for (int i = 21; i < 99; i++)
	{
		if (RelPC(mailbox[i], side) == wk) return IsAttacking(i, -side);
	}

	// If for some bizarre reason we do not have a king, return 0
	return 0;
}

// Returns the active side
signed char Board::ActiveSide(){
	return status.active_side;
};

// ToString() returns a string representation of the board, from the top to the bottom
std::string Board::ToString()
{
	std::string current_string;
	int current_index = 0;

	// Scroll through each square from top to bottom, and print piece
	for (int i = 90; i >= 20; i -= 10)
	{
		for (int j = 1; j < 9; j++)
		{
			Piece current_piece = mailbox[i + j];
			current_string  += PieceToChar(current_piece);
		}
		current_string += '\n';
	}

	return current_string;
}

// ToFEN() returns the FEN representation of the board
std::string Board::ToFEN()
{
	std::string current_string;
	int current_index = 0;

	// Scroll through each square from top to bottom, and print piece
	for (int i = 90; i >= 20; i -= 10)
	{
		int e_count = 0;
		for (int j = 1; j < 9; j++)
		{
			Piece current_piece = mailbox[i + j];
			char piece_char = PieceToChar(current_piece);

			if (piece_char != '.')
			{
				if (e_count > 0) current_string += (e_count + '0');
				e_count = 0;
				current_string += piece_char;
			}
			else e_count++;
		}

		if (e_count > 0) current_string += (e_count + '0');
		if (i != 20) current_string += '/';
	}

	current_string += " ";

	// Side to move
	current_string += (status.active_side == White) ? "w " : "b ";

	//	Castling Rights
	if (CanCastle(White, Kingside)) current_string += 'K';
	if (CanCastle(White, Queenside)) current_string += 'Q';
	if (CanCastle(Black, Kingside)) current_string += 'k';
	if (CanCastle(Black, Queenside)) current_string += 'q';
	if (current_string.back() == ' ') current_string += '-';

	current_string += ' ';

	// En passant square
	if (status.ep_square != 0) current_string += SquareToString(status.ep_square) + " ";
	else current_string += "- ";

	// Capture ply count
	std::stringstream current_ss;
	current_ss << current_string;
	if (status.cap_ply == 0) current_ss << "0 ";
	else current_ss << std::dec << (int)status.cap_ply << " ";
	
	// Full move count
	current_ss << (int)status.cur_move_cnt;

	return current_ss.str();
}

// DoMove(mv) - Moves the piece on the chessboard according to official FIDE rules. Updates board and status. Board must be reset by using
//				UndoMove() in order to return to original position. 
void Board::DoMove(Move mv)
{
	// Push status and current move onto stack
	status_history.push(status);
	move_history.push(mv);

	// Update side
	status.active_side = -status.active_side;

	// 50 moves rule: if piece is not a pawn and we are not capturing, add one to count
	if (mv._pc_prev == 0 && PcAbs(mailbox[mv._src]) != wp) status.cap_ply++;
	else status.cap_ply = 0;

	// reset e.p. square - if this changes will be updated later
	status.ep_square = 0;

	// Total move count
	if (status.active_side == White) status.cur_move_cnt++;

	// Castling rights detection
	// rook moves
	if (mv._src == 21) UnsetCastle(White, Queenside);
	else if (mv._src == 28) UnsetCastle(White, Kingside);
	else if (mv._src == 91) UnsetCastle(Black, Queenside);
	else if (mv._src == 98) UnsetCastle(Black, Kingside);
	// king moves
	else if (mv._src == 25)
	{
		UnsetCastle(White, Queenside);
		UnsetCastle(White, Kingside);
	}
	else if (mv._src == 95)
	{
		UnsetCastle(Black, Queenside);
		UnsetCastle(Black, Kingside);
	}

	// Move piece and clear starting square
	Piece mv_pc = mailbox[mv._src];
	//UpdateZobrist(mv._pc_prev, mv._dest); // XOR out taken piece
	mailbox[mv._dest] = mailbox[mv._src];
	//UpdateZobrist(mv_pc, mv._dest); // XOR in moved piece
	mailbox[mv._src] = empty;
	//UpdateZobrist(mv_pc, mv._src); // XOR out moved piece

	// If movetype is normal, we are done
	if (mv._ppiece_flags == MoveFlags::Normal) return;

	// Check for pawn double move -> set e.p. square
	if (mv._ppiece_flags < 120) status.ep_square = mv._ppiece_flags;

	// Check for other move types
	else
	{
		if (mv._ppiece_flags == MoveFlags::PawnWQ) PromotePawn(wq, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnWN) PromotePawn(wn, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnWR) PromotePawn(wr, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnWB) PromotePawn(wb, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnBQ) PromotePawn(bq, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnBN) PromotePawn(bn, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnBR) PromotePawn(br, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::PawnBB) PromotePawn(bb, mv._dest);
		else if (mv._ppiece_flags == MoveFlags::EPW) {
			mailbox[mv._dest - 10] = empty;
			//UpdateZobrist(mv._pc_prev, mv._dest - 10);
		}
		else if (mv._ppiece_flags == MoveFlags::EPB) {
			mailbox[mv._dest + 10] = empty;
			//UpdateZobrist(mv._pc_prev, mv._dest + 10);
		}
		else if (mv._ppiece_flags == MoveFlags::CastleQ)
		{
			Piece rook = mailbox[mv._dest - 2]; // Get the rook
			mailbox[mv._dest + 1] = rook; // Move the rook
			//UpdateZobrist(rook, mv._dest + 1); // XOR in the rook
			mailbox[mv._dest - 2] = 0;
			//UpdateZobrist(rook, mv._dest - 2); // XOR out the rook
		}
		else if (mv._ppiece_flags == MoveFlags::CastleK)
		{
			Piece rook = mailbox[mv._dest + 1]; // Get the rook
			mailbox[mv._dest - 1] = rook; // Move the rook
			//UpdateZobrist(rook, mv._dest - 1); // XOR in the rook
			mailbox[mv._dest + 1] = 0;
			//UpdateZobrist(rook, mv._dest + 1); // XOR out the rook
		}
	}
}

// UndoMove(mv) - Undoes the last played move and rewinds the status
void Board::UndoMove()
{
	// Grab move from history
	Move mv = move_history.top();
	move_history.pop();

	// Move piece back to src
	mailbox[mv._src] = mailbox[mv._dest];
	mailbox[mv._dest] = mv._pc_prev;

	// Restore status
	status = status_history.top();
	status_history.pop();


	// If normal or double move, we are done
	if (mv._ppiece_flags < 120) return;

	// Special moves
	if (mv._ppiece_flags == MoveFlags::PawnWQ) mailbox[mv._src] = wp;
	else if (mv._ppiece_flags == MoveFlags::PawnWN) mailbox[mv._src] = wp;
	else if (mv._ppiece_flags == MoveFlags::PawnWR) mailbox[mv._src] = wp;
	else if (mv._ppiece_flags == MoveFlags::PawnWB) mailbox[mv._src] = wp;
	else if (mv._ppiece_flags == MoveFlags::PawnBQ) mailbox[mv._src] = bp;
	else if (mv._ppiece_flags == MoveFlags::PawnBN) mailbox[mv._src] = bp;
	else if (mv._ppiece_flags == MoveFlags::PawnBR) mailbox[mv._src] = bp;
	else if (mv._ppiece_flags == MoveFlags::PawnBB) mailbox[mv._src] = bp;
	else if (mv._ppiece_flags == MoveFlags::EPW)	mailbox[mv._dest - 10] = bp;
	else if (mv._ppiece_flags == MoveFlags::EPB)    mailbox[mv._dest + 10] = wp;
	else if (mv._ppiece_flags == MoveFlags::CastleQ)
	{
		mailbox[mv._dest - 2] = mailbox[mv._dest + 1];
		mailbox[mv._dest + 1] = empty;
	}
	else if (mv._ppiece_flags == MoveFlags::CastleK)
	{
		mailbox[mv._dest + 1] = mailbox[mv._dest - 1];
		mailbox[mv._dest - 1] = empty;
	}
}


// int MoveGen(std::list<move>, MoveFilter filter, char side) - generates list of moves, returning the number of generated moves or -1 if king captured
//								   detected. Filter allows only certain move types to be generated - useful for quiscient search
//								   and mate detection.
int Board::MoveGen(std::list<Move> &movelist,MoveFilter filter, char side)
{

	// Integer to count number of generated moves
	int mv_count = 0;

	// Scroll through all pieces on board and check for own pieces
	for (Square sq = 21; sq < 99; sq++)
	{
		// Get the piece on the current square
		Piece signed_piece = mailbox[sq];

		// Check to see if own piece
		if (IsOwn(signed_piece, side))
		{
			// Get the absolute value of the piece - this is required by look-up tables
			Piece abs_piece = PcAbs(signed_piece);

			// If the piece is a pawn, we do something special
			if (abs_piece == wp)
			{
				// First, check for standard forward move
				int dest_sq = (side == White) ? (sq + 10) : (sq -10);
				Piece dest_piece = mailbox[dest_sq];
				if (dest_piece == empty && filter != MoveFilter::KingCapt)
				{
					// Check for promotion square
					if (GetRank(dest_sq) == 1 || GetRank(dest_sq) == 8)
					{
						movelist.push_front(Move(sq, dest_sq, empty, (side == White) ? PawnWB : PawnBB));
						movelist.push_front(Move(sq, dest_sq, empty, (side == White) ? PawnWR : PawnBR));
						movelist.push_front(Move(sq, dest_sq, empty, (side == White) ? PawnWN : PawnBN));
						movelist.push_front(Move(sq, dest_sq, empty, (side == White) ? PawnWQ : PawnBQ));
						mv_count += 4;
					}

					// Otherwise, only add move if we are searching for all moves
					else if (filter == MoveFilter::All)
					{
						movelist.push_back(Move(sq, dest_sq, empty));
						mv_count++;
					}

					// Check for double move
					if (filter == MoveFilter::All && (GetRank(sq) == 2 || GetRank(sq) == 7))
					{
						Square dest_sq2 = (side == White) ? (dest_sq + 10) : (dest_sq - 10);
						
						if (mailbox[dest_sq2] == empty)
						{
							movelist.push_back(Move(sq, dest_sq2, empty, (MoveFlags) dest_sq));
							mv_count++;
						}
					}
				}

				// Now we need to check for captures
				for (int dir = 9; dir <= 11; dir += 2)
				{
					Square dest_sq = sq + ((side == White) ? dir : -dir);
					Piece dest_piece = mailbox[dest_sq];

					// Check to see if enemy piece
					if (IsOwn(dest_piece, -side))
					{
						// King capture detection
						if (dest_piece == wk || dest_piece == bk) return -1;
						if (filter == MoveFilter::PromCapt || filter == MoveFilter::All)
						{
							// Again, check for promotion
							if (GetRank(dest_sq) == 1 || GetRank(dest_sq) == 8)
							{
								movelist.push_front(Move(sq, dest_sq, dest_piece, (side == White) ? PawnWB : PawnBB));
								movelist.push_front(Move(sq, dest_sq, dest_piece, (side == White) ? PawnWR : PawnBR));
								movelist.push_front(Move(sq, dest_sq, dest_piece, (side == White) ? PawnWN : PawnBN));
								movelist.push_front(Move(sq, dest_sq, dest_piece, (side == White) ? PawnWQ : PawnBQ));
								mv_count += 4;
							}

							else
							{
								movelist.push_front(Move(sq, dest_sq, dest_piece));
								mv_count++;
							}

						}
					}

					// Check for e.p. square
					if ((filter == MoveFilter::PromCapt || filter == MoveFilter::All) && dest_sq == status.ep_square)
					{
						// Decide which flag to use based on our side
						MoveFlags flags = (side == White) ? MoveFlags::EPW : MoveFlags::EPB;

						// Add move to list
						movelist.push_front(Move(sq, dest_sq, empty, flags));

						// Update move_count
						mv_count++;
					}
				}
			}

			// Otherwise, we can just run the generic move routine
			else
			{
				// dir - index of current vector in vector table
				int dir = 0;

				// We need to go through each possible direction the piece can move in
				while (true)
				{
					int current_vector = PieceRuleArray[abs_piece].direction_vectors[dir++];

					// If the current vector is null we have exhausted all directions for this piece
					if (current_vector == 0) break;

					// Otherwise, we need to slide through the current direction until 
					// a) We reach our own piece. Since we cannot move there, we are done with this slide
					// b) We reach an opponents piece. We capture then end the current slide
					// c) We reach out of bounds. We cannot move out of bounds so end the current slide
					// d) We reach max run count. We have run out of allowed moves
					int dest = sq;

					for (int run_length = 0; run_length < PieceRuleArray[abs_piece].max_run_length; run_length++)
					{
						// Our new destination square is from our last destination square + vector
						dest = dest + current_vector;

						// Find what piece, if any, lies in the destination square
						Piece dest_piece = mailbox[dest];

						if (dest_piece == oob) break;
						if (IsOwn(dest_piece, side)) break;
						if (dest_piece == empty)
						{
							// Check for castling
							if (abs_piece == wk)
							{
								// Queenside
								if (current_vector == -1 && CanCastle(side, Queenside) && mailbox[dest - 1] == empty && (IsAttacking(dest, -side) != 1) && (IsAttacking(sq, -side) != 1))
								{
									movelist.push_back(Move(sq, dest - 1, empty, MoveFlags::CastleQ));
									mv_count++;
								}

								// Kingside
								if (current_vector == 1 && CanCastle(side, Kingside) && mailbox[dest + 1] == empty && (IsAttacking(dest, -side) != 1) && (IsAttacking(sq, -side) != 1))
								{
									movelist.push_back(Move(sq, dest + 1, empty, MoveFlags::CastleK));
									mv_count++;
								}
							}

							// Add the move to the list if we are looking for all moves
							if (filter == MoveFilter::All)
							{
								movelist.push_back(Move(sq, dest, empty));
								mv_count++;
							}

							// Then resume search for next square in slide direction
						}

						else // Capture
						{
							// Check for king capture. If found, immidietly return. Leave move list preserved
							if (dest_piece == wk || dest_piece == bk)
							{
								return -1;
							}

							if (filter == MoveFilter::PromCapt || filter == MoveFilter::All)
							{
								movelist.push_front(Move(sq, dest, dest_piece));
								mv_count++;
							}

							// Stop looking in current direction
							break;
								
						}
					}

				}
			}


		}
	}


	// Return count
	return mv_count;

}

// Score ABSearch(depth, alpha,beta, SearchStats&) - performs an AB search at the current depth using the current status
Score Board::ABSearch(int depth, Score alpha, Score beta, SearchStats &stats)
{
	// First off, update node count
	stats.node_count[depth]++;

	// Keep track of best score found
	Score best_score = -99999;

	// Check for 50 move rule (needs work)
	if (status.cap_ply == 50) return 0;

	// Check the transposition table
	//TTable::TEntry *ttentry = tzt->FindEntry(zobrist);
	//if (ttentry) {
	//	if (ttentry->castle_status == castle_rights && ttentry->e_p_sq == ep_square // if entry is valid
	//		&& ttentry->depth >= depth && ttentry->ActivePlayer() == active_side) {
	//		stats.tt_hits++;
	//		if (ttentry->score > beta) { // AB cutoff
	//			stats.ab_cutoffs[depth]++;
	//			return ttentry->score;
	//		}
	//		else if (!(ttentry->SOFT & ttentry->flags)) { // Hard score
	//			return ttentry->score;
	//		}
	//		else { // Soft score - at least we have a baseline lowest score
	//			best_score = ttentry->score;
	//		}
	//	}
	//}

	// If we are at a root, perform Q search (tbi)
	if (depth == 0) return QSearch(alpha, beta, stats);

	// Otherwise, enter main code
	else
	{
		// First, we will generate a list of moves
		std::list<Move> move_list;
		int no_moves = MoveGen(move_list, MoveFilter::All, status.active_side);

		// If -1, we have a king capture situation: return a big number + depth
		// - this ensures that quicker mates will be favoured
		if (no_moves == -1)
		{
			// std::cout << ToString() << "\n";
			return (1000 + depth);
		}
	

		// Otherwise, we need to go through each move looking for a better alpha
		for (std::list<Move>::iterator i = move_list.begin(); i != move_list.end(); i++)
		{
			// Then, do the move
			DoMove((*i));

			// Now, we need to get the score of the board after we have completed the move
			Score mv_score = -ABSearch(depth - 1, -beta, -alpha, stats);

			// Then undo the move
			UndoMove();

			// Check for alpha and beta cut-offs
			if (mv_score > beta)
			{
				stats.ab_cutoffs[depth]++;
				//tzt->AddEntry(zobrist, mv_score, ep_square, castle_rights, depth, active_side, true);
				return mv_score; // fail high
			}
			else if (mv_score > best_score)
			{
				best_score = mv_score;
				if (best_score >= alpha) alpha = best_score;
			}
		}

		// Check for stalemate
		if ((best_score == -1000 - depth + 1) && !InCheck(status.active_side))
			return 0;

		// Add entry into tt
		//tzt->AddEntry(zobrist, best_score, ep_square, castle_rights, depth, active_side, false);

		// Return the best score
		return best_score;
	}
}

// BestMoveSearch(depth,alpha,beta, SearchStats&) - returns the best move for active_side in current position
Move Board::BestMoveSearch(int depth, SearchStats &stats)
{
	// First generate list of moves
	std::list<Move> moves;
	MoveGen(moves, MoveFilter::All, status.active_side);

	// Go through each one, keeping track of the best move found so far
	Move best_move;
	Score best_score = -9999;

	for (std::list<Move>::iterator i = moves.begin(); i != moves.end(); i++)
	{
		DoMove((*i));

		Score move_score = -ABSearch(depth, -9999, -best_score, stats);

		UndoMove();

		if (move_score > best_score)
		{
			best_move = (*i);
			best_score = move_score;
		}
	}

	return best_move;
}

// Score QSearch(alpha, beta, SearchStats&) - performs a quiscence search in which only important moves are considered
Score Board::QSearch(Score alpha, Score beta, SearchStats &stats)
{
	// Increase node count
	stats.q_nodes++;

	// Stand-pat score -> first, see if we can take a score from the TTable
	Score stand_pat;

	// Check transposition table
	//TTable::TEntry *ttentry = tzt->FindEntry(zobrist);
	//if (ttentry) {
	//	if (ttentry->castle_status == castle_rights && ttentry->e_p_sq == ep_square // if entry is valid
	//		&& ttentry->depth >= 0 && ttentry->ActivePlayer() == active_side) {
	//		stats.tt_hits++;
	//		if (ttentry->score > beta) { // AB cutoff
	//			stats.q_cutoffs++;
	//			return ttentry->score;
	//		}
	//		else if (!(ttentry->SOFT & ttentry->flags)) { // Hard score
	//			return ttentry->score;
	//		}
	//		else { // Soft score - at least we have a baseline lowest score
	//			stand_pat = ttentry->score;
	//		}
	//	}
	//	else stand_pat = (active_side == 1) ? Evaluate() : -Evaluate();
	//}
	// If no entry in table, evaluate to provide score
	//else 
	stand_pat = (status.active_side == 1) ? Evaluate() : -Evaluate();

	if (stand_pat > beta) {
		//tzt->AddEntry(zobrist, stand_pat, ep_square, castle_rights, 0, active_side, true);
		return stand_pat;
	}
	if (stand_pat > alpha) alpha = stand_pat;


	// Now, generate a list of captures we can make
	std::list<Move> capture_moves;
	int mv_count = MoveGen(capture_moves, MoveFilter::PromCapt, status.active_side);

	// If king capture, return 1000
	if (mv_count == -1) return 1000;

	// Search each move
	for (std::list<Move>::iterator i = capture_moves.begin(); i != capture_moves.end(); i++)
	{
		// Only consider move if stand_pat + value of piece > alpha - 0.2
		if ((piece_values[PcAbs((*i)._pc_prev)] + stand_pat) > alpha - 0.2)
		{
			DoMove((*i));

			// Perform Q search on new board
			Score mv_score = -QSearch(-beta, -alpha, stats);

			// Undo the move
			UndoMove();

			// Update alpha/beta
			if (mv_score > beta) {
				//tzt->AddEntry(zobrist, mv_score, ep_square, castle_rights, 0, active_side, true);
				return mv_score; // fail high
			}
			else if (mv_score > stand_pat)
			{
				stand_pat = mv_score;
				stats.q_cutoffs++;
			}
		}
	}

	// Update ttable and return score
	//tzt->AddEntry(zobrist, stand_pat, ep_square, castle_rights, 0, active_side, false);
	return stand_pat;
}

// void CreateZobrist() - generates and updates the zobrist key corresponding to the current board
//void Board::CreateZobrist()
/*{
	zobrist = 0;

	for (int i = 0; i < 120; i++)
	{
		zobrist ^= tzt->GetKey(mailbox[i], i);
	}

	std::cout << "Z Key Created: " << zobrist << "!\n";
}

// void UpdateZobrist(Piece, Square) -  updates the Zkey with the piece/square changes
//void Board::UpdateZobrist(Piece pc, Square sq)
{
	zobrist ^= tzt->GetKey(pc, sq);
}
*/
// void PromotePawn(Piece pc, Square sq) - Utility function: replaces a pawn on sq with pc
void Board::PromotePawn(Piece pc, Square sq)
{
	//UpdateZobrist(mailbox[sq], sq); // Remove the pawns zobrist key
	mailbox[sq] = pc; // Replace pawn with promoted piece
	//UpdateZobrist(pc, sq); // Update the zobrist key
}