#include "SaveLevel.h"
#include "FileUtils.h"
#include "Game/Level.h"
#include "SaveUtils.h"

using namespace rapidjson;
using namespace SaveUtils;

void Save::save(const std::string_view filePath, const Properties& props,
	const Game& game, const Level& level)
{
	StringBuffer buffer(0, std::numeric_limits<uint16_t>::max());
	PrettyWriter<StringBuffer> writer(buffer);
	writer.SetIndent(' ', 2);

	serialize(&writer, props, game, level);

	FileUtils::saveText(filePath, { buffer.GetString(), buffer.GetSize() });
}

void Save::serialize(void* serializeObj, const Properties& props,
	const Game& game, const Level& level)
{
	auto& writer = *((PrettyWriter<StringBuffer>*)serializeObj);

	// root
	writer.StartObject();

	writeKeyStringView(writer, "level");

	// level
	writer.StartObject();

	writeStringView(writer, "id", level.levelId);

	// only save high scores
	writeKeyStringView(writer, "highScores");
	writer.StartArray();
	for (const auto& score : level.highScores)
	{
		writer.StartObject();
		writeString(writer, "name", score.name);
		writeUInt(writer, "level", score.level);
		writeUInt64(writer, "score", score.score);
		writeUInt64(writer, "jewels", score.jewels);
		writer.EndObject();
	}
	writer.EndArray();

	// level
	writer.EndObject();

	// root
	writer.EndObject();
}
