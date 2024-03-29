#include <BWAPI.h>

using namespace BWAPI;
using namespace Filter;
using namespace std;

Unit getUnitOfType(BWAPI::UnitType type)
{
	for (auto& unit : Broodwar->self()->getUnits())
	{
		if (unit->getType() == type && unit->isCompleted())
		{
			return unit;
		}
	}

	return nullptr;
}

bool buildBuilding(UnitType type)
{
	UnitType const builderType = type.whatBuilds().first;

	Unit builder = getUnitOfType(builderType);
	if (!builder) { return false; }

	TilePosition desiredPos = Broodwar->self()->getStartLocation();

	int maxBuildRange = 64;
	bool buildingOnCreep = type.requiresCreep();
	TilePosition buildPos = Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);
	return builder->build(type, buildPos);
}