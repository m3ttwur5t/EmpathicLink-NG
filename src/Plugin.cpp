#include "Run.hpp"
#include "PCH.h"

namespace HEAL
{
	bool BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		vm->RegisterFunction("WhichSpellCastMe", "AllylinkScriptExtender", WhichSpellCastMe);
		vm->RegisterFunction("GetRealSpell", "AllylinkScriptExtender", GetRealSpell);
		vm->RegisterFunction("GetFakeSpell", "AllylinkScriptExtender", GetFakeSpell);

		return true;
	}

	RE::SpellItem* WhichSpellCastMe(RE::StaticFunctionTag*, RE::ActiveEffect* effect)
	{
		if (!effect || !effect->spell) {
			logger::error("WhichSpellCastMe: EFFECT/SPELL WAS NULL");
		}
		const auto& ret = effect->spell->As<RE::SpellItem>();
		logger::info("WhichSpellCastMe: 0x{:08X}", ret ? ret->GetFormID() : 0x0);
		return ret;
	}

	RE::SpellItem* GetRealSpell(RE::StaticFunctionTag*, RE::SpellItem* fakeSpell)
	{
		const auto& [realSpell, hand] = HEAL::CACHE::SpellToHealerSpell.getKeyOrNull(fakeSpell);
		if (realSpell)
			logger::info("GetRealSpell({}, 0x{:08X}) = {}, 0x{:08X}", fakeSpell->GetName(), fakeSpell->GetFormID(), realSpell->GetName(), realSpell->GetFormID());
		else
			logger::warn("GetRealSpell({}, 0x{:08X}) = NULL", fakeSpell->GetName(), fakeSpell->GetFormID());
		return realSpell;
	}

	RE::SpellItem* GetFakeSpell(RE::StaticFunctionTag*, RE::SpellItem* realSpell, int hand)
	{
		auto fakeSpell = HEAL::CACHE::SpellToHealerSpell.getValueOrNull({ realSpell, hand });
		if (!fakeSpell) {
			static const auto& spellFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::SpellItem>();
			if (!spellFactory) {
				logger::error("GetFakeSpell Failed to fetch IFormFactory: SPEL!");
				return nullptr;
			}
			fakeSpell = LinkifySpellFull(realSpell, spellFactory, hand);
			HEAL::CACHE::SpellToHealerSpell.insert({ realSpell, hand }, fakeSpell);
		}
		if (fakeSpell) {
			logger::info("GetFakeSpell({}, 0x{:08X}) = {}, 0x{:08X}", realSpell->GetName(), realSpell->GetFormID(), fakeSpell->GetName(), fakeSpell->GetFormID());
		} else {
			logger::info("GetFakeSpell({}, 0x{:08X}) = NULL", realSpell->GetName(), realSpell->GetFormID());
		}
		return fakeSpell;
	}

	RE::SpellItem* LinkifySpellFull(RE::SpellItem* const& theSpell, RE::ConcreteFormFactory<RE::SpellItem, RE::FormType::Spell>* spellFactory, int hand)
	{
		if (!CanLinkify(theSpell))
			return nullptr;

		auto healerSpell = spellFactory->Create();
		healerSpell->SetFormID(HEAL::FORMS::GetSingleton().NextFormID(), false);

		healerSpell->SetFormEditorID(std::format("{}_fake{}", theSpell->GetFormEditorID(), hand).c_str());

		healerSpell->avEffectSetting = theSpell->avEffectSetting;
		healerSpell->boundData = theSpell->boundData;
		healerSpell->data = theSpell->data;
		for (const auto& eff : theSpell->effects)
			healerSpell->effects.emplace_back(CreateDummyEffect(eff));

		healerSpell->equipSlot = theSpell->equipSlot;
		healerSpell->fileOffset = theSpell->fileOffset;
		healerSpell->formFlags = theSpell->formFlags;
		healerSpell->fullName = std::format("{} - Ally", theSpell->GetFullName());
		healerSpell->hostileCount = theSpell->hostileCount;
		healerSpell->inGameFormFlags = theSpell->inGameFormFlags;

		for (uint32_t i = 0; i < theSpell->GetNumKeywords(); i++)
			if (theSpell->GetKeywordAt(i).has_value())
				healerSpell->AddKeyword(theSpell->GetKeywordAt(i).value());
		if (hand == 1)
			healerSpell->effects.emplace_back(HEAL::FORMS::GetSingleton().SpelHealerTemplateFFRight->effects.front());
		else
			healerSpell->effects.emplace_back(HEAL::FORMS::GetSingleton().SpelHealerTemplateFFLeft->effects.front());

		healerSpell->AddKeyword(HEAL::FORMS::GetSingleton().KywdHealerSpellFF);

		logger::info("Link-ifyed Spell: {} (0x{:08X}~{}) = 0x{:08X}", theSpell->GetName(), theSpell->GetFormID(), theSpell->GetFile(0)->GetFilename(), healerSpell->GetFormID());

		return healerSpell;
	}

	bool CanLinkify(RE::SpellItem* const& theSpell)
	{
		if (theSpell->HasKeyword(HEAL::FORMS::GetSingleton().KywdHealerSpellFF))
			return false;
		if (theSpell->equipSlot == HEAL::FORMS::GetSingleton().EquipSlotBoth)
			return false;
		if (theSpell->data.delivery != RE::MagicSystem::Delivery::kSelf)
			return false;
		if (theSpell->effects.empty())
			return false;
		if (theSpell->data.castingType != RE::MagicSystem::CastingType::kFireAndForget)
			return false;

		return true;
	}

	RE::Effect* CreateDummyEffect(RE::Effect* mimic)
	{
		static const auto& effFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::EffectSetting>();
		static const auto& dummyEff = HEAL::FORMS::GetSingleton().SpelDummyEffectTemplate->effects[0];

		RE::Effect* newEff = new RE::Effect();
		newEff->conditions = mimic->conditions;
		newEff->cost = mimic->cost;
		newEff->effectItem = mimic->effectItem;
		newEff->pad0C = mimic->pad0C;
		newEff->pad1C = mimic->pad1C;

		newEff->baseEffect = effFactory->Create();
		newEff->baseEffect->data = RE::EffectSetting::EffectSettingData{ dummyEff->baseEffect->data };
		newEff->baseEffect->data.baseCost = mimic->baseEffect->data.baseCost;
		newEff->baseEffect->data.spellmakingChargeTime = mimic->baseEffect->data.spellmakingChargeTime;
		newEff->baseEffect->data.archetype = RE::EffectSetting::Archetype::kScript;

		newEff->baseEffect->data.explosion = mimic->baseEffect->data.explosion;
		newEff->baseEffect->data.projectileBase = mimic->baseEffect->data.projectileBase;
		newEff->baseEffect->data.castingArt = mimic->baseEffect->data.castingArt;
		newEff->baseEffect->data.associatedSkill = mimic->baseEffect->data.associatedSkill;
		newEff->baseEffect->data.resistVariable = mimic->baseEffect->data.resistVariable;
		newEff->baseEffect->data.hitEffectArt = nullptr;
		newEff->baseEffect->data.effectShader = nullptr;
		newEff->baseEffect->data.hitVisuals = nullptr;
		newEff->baseEffect->data.castingSoundLevel = mimic->baseEffect->data.castingSoundLevel;
		newEff->baseEffect->data.impactDataSet = mimic->baseEffect->data.impactDataSet;
		newEff->baseEffect->effectSounds = mimic->baseEffect->effectSounds;
		newEff->baseEffect->fullName = mimic->baseEffect->fullName;
		return newEff;
	}
}
