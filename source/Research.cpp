/* Research.cpp
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

#include "Research.h"

#include "DataNode.h"

#include <algorithm>

using namespace std;



void Research::Load(const DataNode &node)
{
	if(node.Size() < 2)
		return;
	trueName = node.Token(1);
	materials.clear();
	requirements.clear();

	for(const DataNode &child : node)
	{
		const string &key = child.Token(0);
		bool hasValue = child.Size() >= 2;
		if(key == "description" && hasValue)
			description = child.Token(1);
		else if(key == "cost" && hasValue)
			cost = max(1, static_cast<int>(child.Value(1)));
		else if(key == "material" && child.Size() >= 3)
		{
			int amount = static_cast<int>(child.Value(2));
			if(amount > 0)
				materials.emplace_back(child.Token(1), amount);
			else
				child.PrintTrace("Skipping non-positive amount:");
		}
		else if(key == "requires" && hasValue)
			requirements.push_back(child.Token(1));
		else if(key == "risk reduction" && hasValue)
			riskReduction = clamp(static_cast<int>(child.Value(1)), 0, 100);
		else if(key == "fold relay")
			foldRelay = true;
		else if(key == "event" && hasValue)
			event = child.Token(1);
		else
			child.PrintTrace("Skipping unrecognized attribute:");
	}
}



bool Research::IsDefined() const
{
	return !trueName.empty();
}



const string &Research::TrueName() const
{
	return trueName;
}



const string &Research::Description() const
{
	return description;
}



int Research::Cost() const
{
	return cost;
}



const Research::Amounts &Research::Materials() const
{
	return materials;
}



const vector<string> &Research::Requirements() const
{
	return requirements;
}



int Research::RiskReduction() const
{
	return riskReduction;
}



bool Research::IsFoldRelay() const
{
	return foldRelay;
}



const string &Research::Event() const
{
	return event;
}



string Research::Condition() const
{
	return "research: " + trueName;
}
