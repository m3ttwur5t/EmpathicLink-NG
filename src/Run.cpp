#include "Run.hpp"
#include "FFIDMAN_API.hpp"

static void FreeFormIDs()
{
	logger::info("Clearing formid space");
	const auto idman = FFIDMAN_API::Manager::GetInstance();
	if (idman->IsLoaded()) {
		idman->ReleaseAllFormIDs(HEAL::PLUGIN_NAME.data());
	}
	logger::info("Done clearing");
}
void OnInit(SKSE::MessagingInterface::Message* const a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		FreeFormIDs();
		break;
	case SKSE::MessagingInterface::kSaveGame:
		break;
	}
}

bool Load()
{
	SKSE::GetPapyrusInterface()->Register(HEAL::BindPapyrusFunctions);
	return true;
}