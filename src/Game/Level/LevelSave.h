#pragma once

#include <string_view>

class Level;
struct LevelSaveObject;

class LevelSave
{
private:
	static bool load(Level& level, const LevelSaveObject& levelState);

	static void serialize(const Level& level, void* serializeObj);

public:
	static bool load(Level& level, const std::string_view filePath);

	static void save(const Level& level, const std::string_view filePath);
};
