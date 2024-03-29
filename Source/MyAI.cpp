#include "MyAI.h"

#include "../../ExampleAIClient/Tools.h"


using namespace BWAPI;
using namespace Filter;
using namespace std;

void MyAI::onStart()
{
	Broodwar->setLocalSpeed(10);
	Broodwar->enableFlag(Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	cout << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() <<endl;
	distributeWorkers();
}

void MyAI::onEnd(bool isWinner)
{
	if (isWinner)
	{
		cout << "victory" << endl;
	}
	else
	{
		cout << "lose" << endl;
	}
}

void MyAI::onFrame()
{
	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Dont run on every frame
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;



	//building
	cout <<"cap: "<< supply_cap <<" used: " << supply_use << " scvs: "<< SCV_count << endl;
	if((supply_cap-supply_use)<2)
	{
		const UnitType supplyProviderType = Broodwar->self()->getRace().getSupplyProvider();
		if (buildBuilding(supplyProviderType)) {
			supply_cap += supplyProviderType.supplyProvided();
			cout << supply_cap << supplyProviderType.supplyProvided()<<endl;
		}
	}

	// Unit managment
	for (auto& u : Broodwar->self()->getUnits())
	{
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
			continue;


		// Finally make the unit do some stuff!
		

		// worker
		if (u->getType().isWorker())
		{
			worker(u);
		}
		else if (u->getType().isResourceDepot())
		{
			// Order the depot to construct more workers! But only when it is idle.
			if (SCV_count<16 && u->isIdle())
			{
				if(u->train(u->getType().getRace().getWorker()))
				{
					SCV_count++;
					supply_use+=2;
				}
			}
		}
	} // closure: unit iterator
}

void MyAI::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void MyAI::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void MyAI::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void MyAI::onNukeDetect(BWAPI::Position target)
{
	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void MyAI::onUnitDiscover(BWAPI::Unit unit)
{
}

void MyAI::onUnitEvade(BWAPI::Unit unit)
{
}

void MyAI::onUnitShow(BWAPI::Unit unit)
{
}

void MyAI::onUnitHide(BWAPI::Unit unit)
{
}

void MyAI::onUnitCreate(BWAPI::Unit unit)
{
	cout << "created" << unit->getType() << endl;
}

void MyAI::onUnitDestroy(BWAPI::Unit unit)
{
}

void MyAI::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(),
			                   unit->getType().c_str());
		}
	}
}

void MyAI::onUnitRenegade(BWAPI::Unit unit)
{
}

void MyAI::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void MyAI::onUnitComplete(BWAPI::Unit unit)
{
}

void MyAI::distributeWorkers() {
	auto& minerals = BWAPI::Broodwar->getMinerals();
	int index = 0;
	for (auto& m : minerals) {
		int i = 0;
		for (auto& u : Broodwar->self()->getUnits())
		{
			if (u->getType().isWorker() && i == index) {
				if (u->getDistance(m) < 1000.0)
				{
					index++;
					u->rightClick(m);
					break;
				}
			}
			if (u->getType().isWorker())
			{
				i++;
			}
		}

	}
}


void MyAI::worker(BWAPI::Unit u)
{
	if (u->isIdle())
	{
		Broodwar->drawTextMap(u->getPosition(),"Unit");
		if (u->isCarryingGas() || u->isCarryingMinerals())
		{
			cout << "returning" << endl;
			u->returnCargo();
		}
		else{
			auto& minerals = BWAPI::Broodwar->getMinerals();
			BWAPI::Unit best = nullptr;
			for (auto& m : minerals)
			{
				if (!best || (u->getDistance(m))<u->getDistance(best))
				{
					best = m;
					
				}
			}
			Broodwar->drawTextMap(best->getPosition(), "Unit");
			u->rightClick(best);
		}

	}
}


