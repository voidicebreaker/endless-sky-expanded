/* Industry.h
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
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

class Blueprint;
class DataNode;
class DataWriter;
class Facility;
class Research;
template<class Type>
class Set;



// All of the industrial facilities the player owns, the goods stocked in each
// one and in their warehouses, the freight routes that move goods between
// them, and the daily production that turns inputs into outputs.
class Industry {
public:
	// What a facility did on the most recent day.
	enum class Status {
		STARTING,
		RUNNING,
		NO_INPUTS,
		STORAGE_FULL,
		NO_CREDITS
	};

	// One type of facility the player has built on a particular planet.
	struct Holding {
		const Facility *type = nullptr;
		// The true name of the planet the facility is on.
		std::string planet;
		// How many of this facility have been built here. Production, upkeep
		// and storage all scale with this.
		int count = 1;
		// Tons of each commodity (inputs and outputs) stored here.
		std::map<std::string, int> stock;
		// Whether outputs are sold to the local market every day.
		bool autoSell = false;
		Status status = Status::STARTING;
		// The most recent status reports from this facility, oldest first, as
		// pairs of date and text.
		std::vector<std::pair<std::string, std::string>> reports;
		// Fractions of a ton lost to pirates, carried over to the next day.
		std::map<std::string, double> pirateCarry;

		// The most tons of any one commodity this holding can store.
		int Capacity() const;
		int Stock(const std::string &commodity) const;
		// Total tons of outputs waiting to be collected.
		int OutputStock() const;
		bool Uses(const std::string &commodity) const;
		bool Makes(const std::string &commodity) const;
		// Add a status report, forgetting the oldest one if there are too many.
		void AddReport(const std::string &date, const std::string &text);
	};

	// A standing order to move a commodity from one planet to another every day.
	// Goods are taken from the outputs of the player's facilities at the source,
	// then from the warehouse there, then (if allowed) bought on the market.
	// They are delivered to facilities at the destination that use them, then
	// to the warehouse there, then (if allowed) sold on the market.
	struct Route {
		std::string commodity;
		std::string from;
		std::string to;
		int tons = 10;
		bool buy = false;
		bool sell = false;
		// What happened on the most recent day.
		int lastMoved = 0;
		int64_t lastCost = 0;
		std::string lastNote = "Waiting for the first day.";
	};

	// An order for the fabrication bays at a station to build something from a
	// blueprint. Each bay works on one order at a time, oldest first.
	struct Order {
		const Blueprint *blueprint = nullptr;
		// The true name of the planet (station) where the order was placed.
		std::string planet;
		int daysLeft = 1;
		// The name the player chose, for a carrier.
		std::string name;
	};

	// The player's carrier: a mobile station that moves between systems.
	struct Carrier {
		// The true name of the carrier's planet, or empty if the player has none.
		std::string name;
		// The system it is in, or travelling to.
		std::string system;
		// Days until it arrives, or 0 if it is there.
		int daysLeft = 0;
	};

	// The result of a day, in credits.
	struct DayReport {
		int64_t upkeep = 0;
		int64_t sales = 0;
		int64_t purchases = 0;
		int64_t freight = 0;
		// What the sales would have paid if no market had been saturated.
		int64_t saturationLoss = 0;
		// Sales minus upkeep, freight and purchases, and the tax on it.
		int64_t profit = 0;
		int64_t tax = 0;
		// Fabrication orders that were finished today. Delivering them is up to the caller.
		std::vector<Order> finished;
		// Research points produced, and the project finished today, if any.
		int research = 0;
		const Research *finishedResearch = nullptr;
		// Goods lost to pirates, in tons and in their usual value.
		int pirateTons = 0;
		int64_t pirateValue = 0;
		// Whether the carrier arrived at its destination today.
		bool carrierArrived = false;

		// The total change to the player's credits.
		int64_t Net() const;
	};

	// One bracket of the daily industry tax: the rate applies to the part of
	// the day's profit above the given amount (up to the next bracket).
	struct TaxBracket {
		int64_t from;
		double rate;
	};

	// Access to the rest of the universe: markets and travel distances.
	class World {
	public:
		virtual ~World() = default;
		// The price of a commodity on the given planet's market, or 0 if it
		// cannot be bought or sold there.
		virtual int Price(const std::string &planet, const std::string &commodity) const = 0;
		// Record that tons were bought (positive) or sold (negative) on the
		// given planet's market, so that its prices can react.
		virtual void Trade(const std::string &planet, const std::string &commodity, int tons) = 0;
		// The number of hyperspace jumps between two planets, or -1 if one
		// cannot be reached from the other.
		virtual int Jumps(const std::string &from, const std::string &to) const = 0;
		// The fraction of each day's output that pirates take on the given
		// planet, before any defenses.
		virtual double Risk(const std::string &planet) const { return 0.; }
		// The usual price of a commodity, for reporting losses.
		virtual int Value(const std::string &commodity) const { return 0; }
	};

	// Freight costs per ton moved: a flat fee, plus a fee for every jump.
	static constexpr int64_t FREIGHT_BASE = 5;
	static constexpr int64_t FREIGHT_PER_JUMP = 10;
	static constexpr int MAX_ROUTE_TONS = 500;
	// Market saturation: every ton of a commodity that the player's industry
	// sells on a planet adds a ton of saturation there, and saturation halves
	// every day. Each ton pays price / (1 + saturation / MARKET_DEPTH).
	static constexpr double MARKET_DEPTH = 100.;
	static constexpr double SATURATION_DECAY = .5;


public:
	// Load the "industry" node of a saved game.
	void Load(const DataNode &node, const Set<Facility> &facilities, const Set<Blueprint> *blueprints = nullptr,
		const Set<Research> *research = nullptr);
	void Save(DataWriter &out) const;

	const std::vector<Holding> &Holdings() const;
	std::vector<Holding> &Holdings();
	// Find the given type of facility on the given planet, if the player owns one.
	Holding *Find(const Facility &type, const std::string &planet);
	const Holding *Find(const Facility &type, const std::string &planet) const;

	// Add a new facility, or one more of it if the player already owns this type
	// of facility on this planet. Paying for it is up to the caller.
	void Build(const Facility &type, const std::string &planet);
	// Run one day: pay upkeep, produce, move freight, and auto-sell. Upkeep,
	// purchases and freight are only paid (and things only happen) while the
	// credits on hand cover them. The caller must apply the returned report
	// to the player's account. Without a world, there are no markets or routes.
	DayReport AdvanceDay(int64_t credits, World *world = nullptr);

	// Warehouses: shared storage for any commodity, provided by facilities.
	int WarehouseCapacity(const std::string &planet) const;
	int WarehouseUsed(const std::string &planet) const;
	const std::map<std::string, int> &Warehouse(const std::string &planet) const;
	// Add up to the given tons to a planet's warehouse; returns the tons added.
	int Store(const std::string &planet, const std::string &commodity, int tons, bool ignoreCapacity = false);
	// Take up to the given tons out of a planet's warehouse; returns the tons taken.
	int Retrieve(const std::string &planet, const std::string &commodity, int tons);

	// Taxes and market saturation.
	static const std::vector<TaxBracket> &TaxBrackets();
	static int64_t Tax(int64_t profit);
	double Saturation(const std::string &planet, const std::string &commodity) const;
	// The saturation of every market the player's industry has sold to recently.
	const std::map<std::pair<std::string, std::string>, double> &Saturations() const;
	// What selling the given tons on a planet would pay right now, at the given full price.
	int64_t SaleValue(const std::string &planet, const std::string &commodity, int tons, int price) const;
	// What happened on the most recent day.
	const DayReport &LastReport() const;

	// Freight routes.
	const std::vector<Route> &Routes() const;
	std::vector<Route> &Routes();
	void AddRoute(const Route &route);
	void RemoveRoute(size_t index);

	// Fabrication.
	// The number of fabrication bays the player owns on the given planet.
	int FabricationBays(const std::string &planet) const;
	const std::vector<Order> &Orders() const;
	// Tons of a commodity that fabrication can use on a planet: the outputs of the
	// player's facilities there, and the warehouse.
	int Available(const std::string &planet, const std::string &commodity) const;
	bool HasMaterials(const Blueprint &blueprint, const std::string &planet) const;
	// Place an order on a planet with a fabrication bay, using up the materials
	// (from the facilities' outputs first, then the warehouse). A carrier order
	// also takes the name the player chose for it.
	// Paying the credits is up to the caller. Returns false if the order can't be placed.
	bool PlaceOrder(const Blueprint &blueprint, const std::string &planet, const std::string &name = "");

	// Take the given tons of a commodity from the outputs of the facilities on a
	// planet, then its warehouse. Takes nothing and returns false if there is not enough.
	bool Take(const std::string &planet, const std::string &commodity, int tons);

	// Research.
	const Research *ActiveResearch() const;
	bool IsResearched(const Research &project) const;
	const std::set<const Research *> &CompletedResearch() const;
	// Whether the project has been started (and its materials paid), and its progress.
	bool HasStarted(const Research &project) const;
	int Progress(const Research &project) const;
	// Make this the active project. Starting a project for the first time uses
	// up its materials from the given planet; returns false if they are missing.
	bool StartResearch(const Research &project, const std::string &planet);
	// Add research points to the active project. Returns the project if that finished it.
	const Research *AddResearch(int points);
	// Whether any finished project lets freight routes cross the shear.
	bool HasFoldRelay() const;

	// Pirates. The fraction of each day's output lost on a planet, given the
	// risk before defenses, after the player's defenses and research.
	double PirateRisk(const std::string &planet, double baseRisk) const;
	// The fraction of pirate losses left after research.
	double ResearchRiskMultiplier() const;

	// The carrier.
	bool HasCarrier() const;
	const Carrier &GetCarrier() const;
	Carrier &GetCarrier();

	// Take up to the given number of tons of a commodity out of a holding's stock.
	// Returns how many tons were actually taken.
	static int Collect(Holding &holding, const std::string &commodity, int space);
	// Add up to the given number of tons of a commodity to a holding's stock,
	// limited by its capacity. Returns how many tons were actually added.
	static int Supply(Holding &holding, const std::string &commodity, int available);


private:
	void Produce(Holding &holding, int64_t &credits, DayReport &report, double risk, const World *world);
	void RunRoute(Route &route, int64_t &credits, DayReport &report, World &world);
	// Sell goods on a market, applying and adding to its saturation. Returns the income.
	int64_t Sell(const std::string &planet, const std::string &commodity, int tons, int price,
		DayReport &report, World &world);
	// Move fabrication orders along by a day.
	void Fabricate(DayReport &report);


private:
	std::vector<Holding> holdings;
	std::map<std::string, std::map<std::string, int>> warehouses;
	std::vector<Route> routes;
	std::vector<Order> orders;
	const Research *activeResearch = nullptr;
	std::map<const Research *, int> researchProgress;
	std::set<const Research *> completedResearch;
	Carrier carrier;
	// Saturation by planet and commodity.
	std::map<std::pair<std::string, std::string>, double> saturation;
	DayReport lastReport;
};
