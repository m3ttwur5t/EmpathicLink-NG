#include "Run.hpp"

void OnInit(SKSE::MessagingInterface::Message* const a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
	case SKSE::MessagingInterface::kDataLoaded:
	case SKSE::MessagingInterface::kSaveGame:
		break;
	}
}

bool Load()
{
	SKSE::GetPapyrusInterface()->Register(HEAL::BindPapyrusFunctions);
	return true;
}