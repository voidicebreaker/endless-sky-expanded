/* IndustryPanel.h
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

#include "Panel.h"

#include <functional>
#include <string>
#include <vector>

class Blueprint;
class Facility;
class Planet;
class PlayerInfo;
class Point;
class Rectangle;



// Panel for the player's industry, drawn in the planet description area like
// the bank, or on its own in flight. It has five views, switched with Tab:
// facilities at one place (build, supply and collect, use the warehouse,
// found a station), fabrication from blueprints, freight routes, finances
// (taxes and market saturation), and an overview of all holdings. The first
// two can show any place where the player owns facilities, but goods can only
// be moved in and out of the place where the player is landed.
class IndustryPanel : public Panel {
public:
	// Without a planet, the panel starts at the first place where the player
	// owns facilities. A remote panel is opened in flight.
	IndustryPanel(PlayerInfo &player, const Planet *planet, bool remote = false);

	// Check whether the Industry button should be shown on this planet: there is
	// a facility that can be built or is owned here, or the player owns
	// facilities elsewhere that they may want to review.
	static bool IsAvailable(const PlayerInfo &player, const Planet &planet);
	// Check whether there is anything to manage from flight.
	static bool CanOpenRemotely(const PlayerInfo &player);

	virtual void Draw() override;


protected:
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
	virtual bool Scroll(double dx, double dy) override;


private:
	enum class View {
		PLANET,
		FABRICATION,
		ROUTES,
		FINANCES,
		OVERVIEW
	};


private:
	// The facility types that can be built or are owned on this planet.
	// Stations can only be founded if canFound is set (the player is there).
	static std::vector<const Facility *> Facilities(const PlayerInfo &player, const Planet &planet,
		bool canFound = true);
	// Whether the player already has a station in the given planet's system.
	static bool HasStationInSystem(const PlayerInfo &player, const Planet &planet);
	// The rows of the planet view: the warehouse (as nullptr) if there is one,
	// then the facilities.
	std::vector<const Facility *> Rows() const;
	bool HasWarehouse() const;
	const Facility *Selected() const;
	bool IsWarehouseSelected() const;

	Rectangle Box() const;
	// Whether the player is landed at the place this panel is showing.
	bool IsHere() const;
	// If the player is not landed here, say so and return false.
	bool RequireHere();
	void ChangeLocation(int step);
	void DrawLocationSwitcher();

	void DrawPlanetView();
	void DrawFabrication();
	void DrawWarehouse(double left, double top);
	void DrawRoutes();
	void DrawFinances();
	void DrawOverview();
	// Draw a button and make it clickable. Moves the corner to the right.
	void DrawButton(Point &corner, const std::string &label, bool enabled, std::function<void()> action);

	// Actions on the selected facility.
	void Build();
	void FoundStation(const std::string &name);
	void Supply();
	void Collect();
	void ToggleAutoSell();
	bool CanAutoSell() const;
	// Warehouse actions.
	void StoreCargo();
	void LoadWarehouse();
	// Fabrication: the blueprints the player has unlocked, and ordering one.
	std::vector<const Blueprint *> Blueprints() const;
	void Order();
	// Freight route actions.
	void NewRoute();
	void DeleteRoute();
	void ChangeRoute(int field, int step);
	// Planets that freight routes can use: wherever the player has facilities, and here.
	std::vector<std::string> Locations() const;
	// Commodities that freight routes can carry.
	static std::vector<std::string> Commodities();
	std::string DefaultStationName() const;


private:
	PlayerInfo &player;
	const Planet *planet = nullptr;
	bool remote = false;
	View view = View::PLANET;
	int selectedRow = 0;
	int selectedRoute = 0;
	int selectedBlueprint = 0;
	int scroll = 0;
	std::string status;
	// The station type waiting for the player to choose a name.
	const Facility *pendingStation = nullptr;
};
