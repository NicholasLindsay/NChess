// NChess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "board.h"
#include "tdef.h"
#include "const.h"
#include "Utility.h"
#include "eval.h"
#include <string>
#include "move.h"
#include <list>
#include "userinterface.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Evaluation::CleanPSQT();

	UIloop();

	std::string dummy_string;
	std::getline(std::cin, dummy_string);

	return 0;
}

