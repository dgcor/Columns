#include "ParseFile2.h"
#include "Parser/ParseFile.h"
#include "Parser/Jewel/ParseJewel.h"
#include "Parser/Jewel/ParseJewelClass.h"
#include "Parser/Level/ParseLevel.h"
#include "Utils/StringHash.h"

namespace Parser2
{
	using namespace Parser;
	using namespace rapidjson;

	bool parseDocumentElem(Game& game, uint16_t nameHash16, const Value& elem,
		ReplaceVars& replaceVars, MemoryPoolAllocator<CrtAllocator>& allocator)
	{
		switch (nameHash16)
		{
		case str2int16("jewel"):
		{
			parseDocumentElemArray(parseJewel, game, nameHash16, elem, replaceVars, allocator);
			break;
		}
		case str2int16("jewelClass"):
		{
			parseDocumentElemArray(parseJewelClass, game, nameHash16, elem, replaceVars, allocator);
			break;
		}
		case str2int16("level"):
		{
			parseDocumentElemArray(parseLevel, game, nameHash16, elem, replaceVars, allocator);
			break;
		}
		default:
			return false;
		}
		return true;
	}
}
