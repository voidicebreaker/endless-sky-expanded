/* Blueprint.h
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

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class DataNode;
class Facility;
class Outfit;
class Ship;



// A design that the player's fabrication bays can build: an outfit or a ship,
// paid for with credits and with materials taken from the player's facilities
// and warehouse at the station where it is ordered. Finished items are left in
// storage (outfits) or parked (ships) at that station.
class Blueprint {
public:
	// A commodity and an amount in tons.
	using Amounts = std::vector<std::pair<std::string, int>>;


public:
	Blueprint() = default;

	void Load(const DataNode &node);
	// A blueprint is defined once it has been loaded and names something to build.
	bool IsDefined() const;

	const std::string &TrueName() const;
	// What this blueprint builds. Exactly one of these is set: an outfit, a
	// ship, or the hull of a carrier (a mobile station).
	const Outfit *GetOutfit() const;
	const Ship *GetShip() const;
	const Facility *GetCarrier() const;
	// The display name of the outfit or ship.
	std::string ItemName() const;
	int64_t Cost() const;
	const Amounts &Materials() const;
	int Days() const;
	// The condition the player must have before this can be ordered, if any.
	const std::string &Requirement() const;


private:
	std::string trueName;
	const Outfit *outfit = nullptr;
	const Ship *ship = nullptr;
	const Facility *carrier = nullptr;
	int64_t cost = 0;
	Amounts materials;
	int days = 1;
	std::string requirement;
};
