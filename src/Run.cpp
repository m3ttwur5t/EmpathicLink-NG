#include "Run.hpp"

void OnInit(SKSE::MessagingInterface::Message* const a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoadGame:
		break;
	}
}

bool Load()
{
	SKSE::GetPapyrusInterface()->Register(HEAL::BindPapyrusFunctions);
	return true;
}