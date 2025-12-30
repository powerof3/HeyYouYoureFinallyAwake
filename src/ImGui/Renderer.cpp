#include "ImGui/Renderer.h"
#include "Manager.h"

namespace ImGui::Renderer
{
	struct CreateD3DAndSwapChain
	{
		static void thunk()
		{
			func();

			if (const auto renderer = RE::BSGraphics::Renderer::GetSingleton()) {
				const auto swapChain = reinterpret_cast<IDXGISwapChain*>(renderer->data.renderWindows[0].swapChain);
				if (!swapChain) {
					logger::error("couldn't find swapChain");
					return;
				}

				DXGI_SWAP_CHAIN_DESC desc{};
				if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
					logger::error("IDXGISwapChain::GetDesc failed.");
					return;
				}

				const auto device = reinterpret_cast<ID3D11Device*>(renderer->data.forwarder);
				const auto context = reinterpret_cast<ID3D11DeviceContext*>(renderer->data.context);

				logger::info("Initializing ImGui..."sv);

				ImGui::CreateContext();

				auto& io = ImGui::GetIO();
				io.IniFilename = nullptr;

				auto& style = ImGui::GetStyle();
				style.WindowPadding = ImVec2();
				style.WindowBorderSize = 0.0f;
				style.ImageBorderSize = 0.0f;
				style.Colors[ImGuiCol_WindowBg] = ImVec4();

				if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
					logger::error("ImGui initialization failed (Win32)");
					return;
				}
				if (!ImGui_ImplDX11_Init(device, context)) {
					logger::error("ImGui initialization failed (DX11)"sv);
					return;
				}

				auto screenSize = RE::BSGraphics::Renderer::GetScreenSize();
				io.DisplaySize.x = static_cast<float>(screenSize.width);
				io.DisplaySize.y = static_cast<float>(screenSize.height);

				logger::info("ImGui initialized.");

				initialized.store(true);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	// DXGIPresentHook
	struct StopTimer
	{
		static void thunk(std::uint32_t a_timer)
		{
			func(a_timer);

			// Skip if Imgui is not loaded
			if (!initialized.load() || !Manager::GetSingleton()->ShouldHideMainMenu()) {
				return;
			}

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			{
				Manager::GetSingleton()->Draw();
			}
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(75595, 77226), OFFSET(0x9, 0x275) };  // BSGraphics::InitD3D
		stl::write_thunk_call<CreateD3DAndSwapChain>(target.address());

		REL::Relocation<std::uintptr_t> target2{ RELOCATION_ID(75461, 77246), 0x9 };  // BSGraphics::Renderer::End
		stl::write_thunk_call<StopTimer>(target2.address());
	}
}
