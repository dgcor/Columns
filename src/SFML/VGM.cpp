#include "VGM.h"
#include <SFML/System/InputStream.hpp>
#include "Utils/Log.h"
#include <vector>

static gme_type_t getVGMType(const void* data)
{
	const char* format = nullptr;
	if (!(format = gme_identify_header(data)))
	{
		return nullptr;
	}
	return gme_identify_extension(format);
}

VGMFileReader::~VGMFileReader()
{
	if (music != nullptr)
	{
		gme_delete(music);
		music = nullptr;
	}
}

bool VGMFileReader::check(sf::InputStream& stream)
{
	unsigned char header[4]{};
	stream.read(header, sizeof(header));
	return getVGMType(header) != nullptr;
}

bool VGMFileReader::open(sf::InputStream& stream, Info& info)
{
	std::vector<unsigned char> fileBytes;
	fileBytes.resize((size_t)stream.getSize());
	stream.read(fileBytes.data(), (sf::Int64)fileBytes.size());

	auto fileType = getVGMType(fileBytes.data());
	if (fileType == nullptr)
	{
		SPDLOG_ERROR("getVGMType error.");
		return false;
	}

	music = gme_new_emu(fileType, 44100);
	if (music == nullptr)
	{
		SPDLOG_ERROR("gme_new_emu error.");
		return false;
	}

	if (gme_load_data(music, fileBytes.data(), (long)fileBytes.size()) != nullptr)
	{
		SPDLOG_ERROR("gme_load_data error.");
		return false;
	}

	auto count = gme_track_count(music);
	if (count < 1)
	{
		SPDLOG_ERROR("gme_track_count error.");
		return false;
	}

	if (gme_start_track(music, 0) != nullptr)
	{
		SPDLOG_ERROR("gme_start_track error.");
		return false;
	}

	gme_info_t* trackInfo{};
	if (gme_track_info(music, &trackInfo, 0) != nullptr)
	{
		SPDLOG_ERROR("gme_track_info error.");
		return false;
	}

	info.sampleCount = (sf::Uint64)(((double)(trackInfo->play_length) / 1000.0) * 44100.0 * 2.0);
	info.channelCount = 2;
	info.sampleRate = 44100;

	gme_free_info(trackInfo);

	return true;
}

void VGMFileReader::seek(sf::Uint64 sampleOffset)
{
	if (music != nullptr)
	{
		gme_seek_samples(music, (int)sampleOffset);
	}
}

sf::Uint64 VGMFileReader::read(sf::Int16* samples, sf::Uint64 maxCount)
{
	if (music != nullptr &&
		gme_track_ended(music) == 0 &&
		gme_play(music, (int)maxCount, (short*)samples) == nullptr)
	{
		return maxCount;
	}
	return 0;
}
