#include "Game2.h"
#include <mutex>
#include <SFML/Audio/SoundFileFactory.hpp>
#if DGENGINE_VGM_MUSIC
#include "SFML/VGM.h"
#endif

Game2::Game2(bool reset_) : Game(reset_)
{
#if DGENGINE_VGM_MUSIC
	static std::once_flag initVGM;
	std::call_once(initVGM, []() { sf::SoundFileFactory::registerReader<VGMFileReader>(); });
#endif
}

bool Game2::getGameProperty(const std::string_view prop1, const std::string_view prop2, Variable& var) const
{
#if DGENGINE_VGM_MUSIC
	if (prop1 == "VGM")
	{
		var = Variable(true);
		return true;
	}
#endif
	return Game::getGameProperty(prop1, prop2, var);
}
