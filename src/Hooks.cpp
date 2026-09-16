#include "Hooks.h"

#include "Manager.h"

namespace Hooks
{
	template <std::size_t N>
	struct HasMostRecentSave
	{
		static bool thunk(RE::BGSSaveLoadManager* a_this)
		{
			return Manager::GetSingleton()->OnPlayerDeath(a_this);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(39661, 40748) };  // PlayerCharacter::UpdatedWhenDeadOrAIControlled

		stl::write_thunk_call<HasMostRecentSave<0>>(target.address() + OFFSET(0x354, 0x367));
		stl::write_thunk_call<HasMostRecentSave<1>>(target.address() + OFFSET(0x3B3, 0x3C1));
		stl::write_thunk_call<HasMostRecentSave<2>>(target.address() + OFFSET(0x40B, 0x419));
	}
}
