#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define DIRECTINPUT_VERSION 0x0800
#define IMGUI_DEFINE_MATH_OPERATORS

#include "RE/Skyrim.h"
#include "REX/REX/Singleton.h"
#include "SKSE/SKSE.h"

#include <d3d11.h>
#include <dxgi.h>
#include <shellapi.h>

#include "ClibUtil/simpleINI.hpp"
#include <spdlog/sinks/basic_file_sink.h>

#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_stdlib.h>

#define DLLEXPORT __declspec(dllexport)

namespace logger = SKSE::log;

using namespace clib_util;
using namespace std::literals;
using namespace RE::literals;

namespace stl
{
	using namespace SKSE::stl;

	template <class T>
	void write_thunk_call(std::uintptr_t a_src)
	{
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class F, size_t offset, class T>
	void write_vfunc()
	{
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[offset] };
		T::func = vtbl.write_vfunc(T::idx, T::thunk);
	}

	template <class F, class T>
	void write_vfunc()
	{
		write_vfunc<F, 0, T>();
	}
}

#include "Version.h"

#ifdef SKYRIM_AE
#	define OFFSET(se, ae) ae
#else
#	define OFFSET(se, ae) se
#endif
