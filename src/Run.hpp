#pragma once

#include "Bimap.h"
#include <SimpleIni.h>

namespace HEAL
{
	constexpr std::string_view PLUGIN_NAME = Plugin::NAME;

	bool BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);
	RE::SpellItem* GetSourceSpell(RE::StaticFunctionTag*, RE::ActiveEffect* effect);
	RE::SpellItem* GetRealSpell(RE::StaticFunctionTag*, RE::SpellItem* effect);
	RE::SpellItem* GetFakeSpell(RE::StaticFunctionTag*, RE::SpellItem* realSpell, int hand);

	static bool CanLinkify(RE::SpellItem* const& theSpell);
	RE::SpellItem* LinkifySpellFull(RE::SpellItem* const& theSpell, RE::ConcreteFormFactory<RE::SpellItem, RE::FormType::Spell>* spellFactory, int hand);
	RE::Effect* CreateDummyEffect(RE::Effect* mimic);

	namespace CONFIG
	{
		class Plugin
		{
		private:
			const std::string_view iniPath = std::format("Data/SKSE/Plugins/{}.ini", PLUGIN_NAME);
			CSimpleIniA Ini;

			Plugin()
			{
				Ini.SetUnicode();
				Ini.LoadFile(iniPath.data());
			}

		public:
			static Plugin& GetSingleton()
			{
				static Plugin instance;
				return instance;
			}

			bool HasKey(const std::string& section, const std::string& key)
			{
				return Ini.KeyExists(section.c_str(), key.c_str());
			}

			bool HasSection(const std::string& section)
			{
				return Ini.SectionExists(section.c_str());
			}

			const std::vector<std::pair<std::string, std::string>> GetAllKeyValuePairs(const std::string& section) const
			{
				std::vector<std::pair<std::string, std::string>> ret;
				CSimpleIniA::TNamesDepend keys;
				Ini.GetAllKeys(section.c_str(), keys);
				for (auto& key : keys) {
					auto val = Ini.GetValue(section.c_str(), key.pItem);
					ret.push_back({ key.pItem, val });
				}

				return ret;
			}

			void DeleteSection(const std::string& section)
			{
				Ini.Delete(section.c_str(), nullptr, true);
			}

			void DeleteKey(const std::string& section, const std::string& key)
			{
				Ini.Delete(section.c_str(), key.c_str());
			}

			std::string GetValue(const std::string& section, const std::string& key)
			{
				return Ini.GetValue(section.c_str(), key.c_str());
			}

			long GetLongValue(const std::string& section, const std::string& key)
			{
				return Ini.GetLongValue(section.c_str(), key.c_str());
			}

			bool GetBoolValue(const std::string& section, const std::string& key)
			{
				return Ini.GetBoolValue(section.c_str(), key.c_str());
			}

			void SetValue(const std::string& section, const std::string& key, const std::string& value, const std::string& comment = std::string())
			{
				Ini.SetValue(section.c_str(), key.c_str(), value.c_str(), comment.length() > 0 ? comment.c_str() : (const char*)0);
			}

			void SetBoolValue(const std::string& section, const std::string& key, const bool value, const std::string& comment = std::string())
			{
				Ini.SetBoolValue(section.c_str(), key.c_str(), value, comment.length() > 0 ? comment.c_str() : (const char*)0);
			}

			void SetLongValue(const std::string& section, const std::string& key, const long value, const std::string& comment = std::string())
			{
				Ini.SetLongValue(section.c_str(), key.c_str(), value, comment.length() > 0 ? comment.c_str() : (const char*)0);
			}

			void Save()
			{
				Ini.SaveFile(iniPath.data());
			}

			Plugin(Plugin const&) = delete;
			void operator=(Plugin const&) = delete;
		};
	}

	namespace CACHE
	{
		inline BiMap<std::pair<RE::SpellItem*, int>, RE::SpellItem*> SpellToHealerSpell;
	}

	class FORMS
	{
	public:
		static const RE::FormID FORMID_OFFSET_BASE = 0xFF01FF00;
		RE::FormID CurrentOffset;

		void SetOffset(RE::FormID offset)
		{
			CurrentOffset = offset;
		}

		RE::FormID NextFormID() const
		{
			static RE::FormID current{ 0x0 };
			return FORMID_OFFSET_BASE + (++current) + CurrentOffset;
		}

		static FORMS& GetSingleton()
		{
			static FORMS instance;
			return instance;
		}

		RE::SpellItem* SpelDummyEffectTemplate = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x80A, "EmpathicLink.esp"sv);

		RE::SpellItem* SpelHealerTemplateFFLeft = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x800, "EmpathicLink.esp"sv);
		RE::SpellItem* SpelHealerTemplateFFRight = RE::TESDataHandler::GetSingleton()->LookupForm<RE::SpellItem>(0x80C, "EmpathicLink.esp"sv);
		RE::BGSKeyword* KywdHealerSpellFF = RE::TESDataHandler::GetSingleton()->LookupForm<RE::BGSKeyword>(0x802, "EmpathicLink.esp"sv);
		RE::BGSEquipSlot* EquipSlotBoth = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x13F45);

	private:
		FORMS() {}
		FORMS(const FORMS&) = delete;
		FORMS& operator=(const FORMS&) = delete;
	};
}
