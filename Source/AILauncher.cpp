#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

#include <Windows.h>

#include <BWAPI/Client.h>
#include <BWAPI.h>

#include "../../ExampleAIModule/Source/MyAI.h"


using namespace BWAPI;

void reconnect()
{
    while (!BWAPIClient.connect())
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
}

int main(int argc, const char* argv[])
{
    reconnect();
    auto client = new MyAI;
    std::cout << "starting match!" << std::endl;
    while (BWAPI::BWAPIClient.isConnected()) 
    {
	    std::cout << "waiting for game"<<std::endl;
        while (BWAPI::BWAPIClient.isConnected() && !BWAPI::Broodwar->isInGame())
        {
            BWAPI::BWAPIClient.update();
        }
        while (Broodwar->isInGame())
        {

            for (std::list<Event>::const_iterator e = Broodwar->getEvents().begin(); e != Broodwar->getEvents().end(); ++e)
            {
                EventType::Enum et = e->getType();
                switch (et)
                {
                case EventType::MatchStart:
                    client->onStart();
                    break;
                case EventType::MatchEnd:
                    client->onEnd(e->isWinner());
                    break;
                case EventType::MatchFrame:
                    client->onFrame();
                    break;
                case EventType::MenuFrame:
                    break;
                case EventType::SendText:
                    client->onSendText(e->getText());
                    break;
                case EventType::ReceiveText:
                    client->onReceiveText(e->getPlayer(), e->getText());
                    break;
                case EventType::PlayerLeft:
                    client->onPlayerLeft(e->getPlayer());
                    break;
                case EventType::NukeDetect:
                    client->onNukeDetect(e->getPosition());
                    break;
                case EventType::UnitDiscover:
                    client->onUnitDiscover(e->getUnit());
                    break;
                case EventType::UnitEvade:
                    client->onUnitEvade(e->getUnit());
                    break;
                case EventType::UnitShow:
                    client->onUnitShow(e->getUnit());
                    break;
                case EventType::UnitHide:
                    client->onUnitHide(e->getUnit());
                    break;
                case EventType::UnitCreate:
                    client->onUnitCreate(e->getUnit());
                    break;
                case EventType::UnitDestroy:
                    client->onUnitDestroy(e->getUnit());
                    break;
                case EventType::UnitMorph:
                    client->onUnitMorph(e->getUnit());
                    break;
                case EventType::UnitRenegade:
                    client->onUnitRenegade(e->getUnit());
                    break;
                case EventType::SaveGame:
                    client->onSaveGame(e->getText());
                    break;
                case EventType::UnitComplete:
                    client->onUnitComplete(e->getUnit());
                    break;
                default:
                    break;
                }
            }
            BWAPI::BWAPIClient.update();
            if (!BWAPI::BWAPIClient.isConnected())
            {
                std::cout << "Reconnecting..." << std::endl;
                reconnect();
            }
        }
    }
    delete client;
}
