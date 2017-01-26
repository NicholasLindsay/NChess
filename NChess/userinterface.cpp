#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string>
#include "move.h"
#include "board.h"
#include "const.h"
#include <list>

// The main user interface loop: enter here once everything has been initilised
void UIloop()
{
	// Create a board to hold the current game - configure it as starting position
	Board game_board;
	game_board.Clean();

	// Create a stats object for search statistics
	SearchStats stats;

	// Create a variable to hold search depth
	int depth = 4;

	// Enter main loop
	while (true)
	{
		std::cout << ((game_board.ActiveSide() == 1) ? "White> " : "Black> ");

		// Get a line of user input
		std::string user_input;
		std::getline(std::cin, user_input);
		std::stringstream user_input_s;
		user_input_s << user_input;

		// Parse initial word
		std::string command_s;
		user_input_s >> command_s;
		
		// Branch depending on command
		if (command_s == "new")
		{
			game_board.Clean();
		}

		// Do the move, if valid
		else if (command_s == "move")
		{
			// First, grab the move from the input stream
			std::string move_str;
			user_input_s >> move_str;

			// Then, see if it is a pseudolegal move
			std::list<Move> pseudomoves;
			int move_no = game_board.MoveGen(pseudomoves, MoveFilter::All, game_board.ActiveSide());

			// Make a variable to keep track if successful
			int success = 0;

			for (std::list<Move>::iterator i = pseudomoves.begin(); i != pseudomoves.end(); i++)
			{
				if ((*i).ToString() == move_str)
				{

					game_board.DoMove((*i));

					// Make sure move is legal - after move we cannot be in check
					if (game_board.InCheck(game_board.ActiveSide()))
					{
						game_board.UndoMove();
					}
					else
					{
						success = 1;
					}

					break;
				}
			}

			if (success == 0) std::cout << "Move is not legal.\n";
		}

		// if exit, quit
		if (command_s == "exit") return;

		// if print, show the board
		if (command_s == "print") std::cout << game_board.ToString() << "\n";

		// if stateval, display static evaluation
		if (command_s == "stateval") std::cout << std::dec << "Score: " << game_board.Evaluate() << "\n";

		// if think, search to depth and return best move
		if (command_s == "think")
		{
			stats.Clear();

			int input_depth = -99;
			user_input_s >> std::dec >> input_depth;

			if (input_depth < 10 && input_depth > 0) depth = input_depth;

			std::cout << "Best move: " << game_board.BestMoveSearch(depth, stats).ToString() << "\n";
		}

		// if stats, show search stats from latest search
		if (command_s == "stats")
		{
			for (int i = depth; i >= 0; i--)
			{
				std::cout << "Depth: " << i << ". Nodes: " << stats.node_count[i] << ". AB Cutoffs: " << stats.ab_cutoffs[i] << ".\n";
			}

			std::cout << "QSearch. Nodes: " << stats.q_nodes << ". AB Cutoffs: " << stats.q_cutoffs << "\n";
			//std::cout << "TTable. Hits: " << stats.tt_hits << ". Misses: " << stats.tt_misses << "\n";
		}
	}
}