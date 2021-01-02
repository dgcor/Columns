#include "ParseImage.h"
#include "Game.h"
#include "Game/Level.h"
#include "GameUtils.h"
#include "Panel.h"
#include "Utils/ParseUtils.h"

namespace Parser
{
	using namespace rapidjson;
	using namespace std::literals;

	void parsePanel(Game& game, const Value& elem)
	{
		if (isValidString(elem, "id") == false)
		{
			return;
		}
		auto id = elem["id"sv].GetStringView();
		if (isValidId(id) == false)
		{
			return;
		}

		auto panel = std::make_shared<Panel>(getBoolKey(elem, "relativePositions"));

		auto anchor = getAnchorKey(elem, "anchor");
		panel->setAnchor(anchor);
		sf::Vector2f size;
		auto pos = getPositionKey(elem, "position", size, game.RefSize());
		if (getBoolKey(elem, "relativeCoords", true) == true &&
			game.RefSize() != game.DrawRegionSize())
		{
			GameUtils::setAnchorPosSize(anchor, pos, size, game.RefSize(), game.DrawRegionSize());
		}
		panel->Position(pos);
		panel->Visible(getBoolKey(elem, "visible", true));

		bool manageObjDrawing = true;

		game.Resources().addDrawable(
			id, panel, manageObjDrawing, getStringViewKey(elem, "resource")
		);
	}
}
