/* Blueprint.cpp
Copyright (c) 2026 by Endless Sky Expanded contributors

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Blueprint.h"

#include "DataNode.h"
#include "Facility.h"
#include "GameData.h"
#include "Outfit.h"
#include "Ship.h"

#include <algorithm>

using namespace std;



void Blueprint::Load(const DataNode &node)
{
	if(node.Size() < 2)
		return;
	trueName = node.Token(1);
	materials.clear();

	for(const DataNode &child : node)
	{
		const string &key = child.Token(0);
		bool hasValue = child.Size() >= 2;
		if(key == "outfit" && hasValue)
		{
			outfit = GameData::Outfits().Get(child.Token(1));
			ship = nullptr;
			carrier = nullptr;
		}
		else if(key == "ship" && hasValue)
		{
			ship = GameData::Ships().Get(child.Token(1));
			outfit = nullptr;
			carrier = nullptr;
		}
		else if(key == "carrier" && hasValue)
		{
			carrier = GameData::Facilities().Get(child.Token(1));
			outfit = nullptr;
			ship = nullptr;
		}
		else if(key == "cost" && hasValue)
			cost = max<int64_t>(0, child.Value(1));
		else if(key == "material" && child.Size() >= 3)
		{
			int amount = static_cast<int>(child.Value(2));
			if(amount > 0)
				materials.emplace_back(child.Token(1), amount);
			else
				child.PrintTrace("Skipping non-positive amount:");
		}
		else if(key == "days" && hasValue)
			days = max(1, static_cast<int>(child.Value(1)));
		else if(key == "requires" && hasValue)
			requirement = child.Token(1);
		else
			child.PrintTrace("Skipping unrecognized attribute:");
	}
}



bool Blueprint::IsDefined() const
{
	return !trueName.empty() && (outfit || ship || carrier);
}



const string &Blueprint::TrueName() const
{
	return trueName;
}



const Outfit *Blueprint::GetOutfit() const
{
	return outfit;
}



const Ship *Blueprint::GetShip() const
{
	return ship;
}



string Blueprint::ItemName() const
{
	if(outfit)
		return outfit->DisplayName();
	if(ship)
		return ship->DisplayModelName();
	if(carrier)
		return carrier->TrueName();
	return trueName;
}



const Facility *Blueprint::GetCarrier() const
{
	return carrier;
}



int64_t Blueprint::Cost() const
{
	return cost;
}



const Blueprint::Amounts &Blueprint::Materials() const
{
	return materials;
}



int Blueprint::Days() const
{
	return days;
}



const string &Blueprint::Requirement() const
{
	return requirement;
}
