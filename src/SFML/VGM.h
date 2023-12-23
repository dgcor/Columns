#pragma once

#include <gme/gme.h>
#include <SFML/Audio/SoundFileReader.hpp>

#undef check

class VGMFileReader : public sf::SoundFileReader
{
private:
	Music_Emu* music{};

public:
	VGMFileReader() = default;
	~VGMFileReader() override;

	static bool check(sf::InputStream& stream);

	bool open(sf::InputStream& stream, Info& info) override;
	void seek(sf::Uint64 sampleOffset) override;
	sf::Uint64 read(sf::Int16* samples, sf::Uint64 maxCount) override;
};
