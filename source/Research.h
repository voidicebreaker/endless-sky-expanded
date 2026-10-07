/* Research.h
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

#pragma once

#include <string>
#include <utility>
#include <vector>

class DataNode;



// A research project that the player's research labs can work on. Labs produce
// research points every day, which go to the project the player has chosen.
// Finishing a project gives the player the condition "research: <name>", which
// facilities, blueprints, missions and other projects can require.
class Research {
public:
	// A commodity and an amount in tons.
	using Amounts = std::vector<std::pair<std::string, int>>;


public:
	Research() = default;

	void Load(const DataNode &node);
	bool IsDefined() const;

	const std::string &TrueName() const;
	const std::string &Description() const;
	// The research points needed to finish this project.
	int Cost() const;
	// Materials used up when the project is started, taken from the place where
	// the player starts it.
	const Amounts &Materials() const;
	// Conditions the player must have before starting this project.
	const std::vector<std::string> &Requirements() const;
	// The percentage by which finishing this project cuts all pirate losses.
	int RiskReduction() const;
	// Whether finishing this project lets freight routes cross the shear.
	bool IsFoldRelay() const;
	// An event to apply when the project is finished, if any.
	const std::string &Event() const;

	// The condition given to the player when this project is finished.
	std::string Condition() const;


private:
	std::string trueName;
	std::string description;
	int cost = 1;
	Amounts materials;
	std::vector<std::string> requirements;
	int riskReduction = 0;
	bool foldRelay = false;
	std::string event;
};
