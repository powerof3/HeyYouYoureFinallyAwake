#include "ImGui/Util.h"

namespace ImGui
{
	ImVec2 GetNativeViewportPos()
	{
		return GetMainViewport()->Pos;
	}

	ImVec2 GetNativeViewportSize()
	{
		return GetMainViewport()->Size;
	}

	ImVec2 GetNativeViewportCenter()
	{
		const auto Size = GetNativeViewportSize();
		return { Size.x * 0.5f, Size.y * 0.5f };
	}
}
