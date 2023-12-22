#include "RegisterHooks.h"
#include "Game/Game.h"
#include "Hooks.h"
#include "Parser/ParseAction2.h"
#include "Parser/ParseFile2.h"

namespace Hooks
{
	void registerHooks()
	{
		ParseDocumentElem = Parser2::parseDocumentElem;
		ParseActionElem = Parser2::parseActionElem;

		Game::DefaultSizeX = 320;
		Game::DefaultSizeY = 224;

		Game::MinSizeX = 320;
		Game::MinSizeY = 224;

		Game::RefSizeX = 320;
		Game::RefSizeY = 224;
	}
}
