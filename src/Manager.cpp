#include "Manager.h"

#include "ImGui/Util.h"

void Manager::Register()
{
	RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(this);
}

void Manager::LoadSettings()
{
	constexpr auto path = "Data/SKSE/Plugins/po3_HeyYouYoureFinallyAwake.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini::get_value(ini, startNewGameOnDeath, "Settings", "bStartNewGameOnDeath", nullptr);
	ini::get_value(ini, deleteSaves, "Settings", "bDeletePreviousCharacterSaves", nullptr);
	ini::get_value(ini, recycleSaves, "Settings", "bSendSavesToRecycleBin", nullptr);

	(void)ini.SaveFile(path);
}

bool Manager::ShouldHideMainMenu() const
{
	return initNewGame;
}

bool Manager::OnPlayerDeath(RE::BGSSaveLoadManager* a_saveLoadManager)
{
	logger::info("Player {:X} has died.", a_saveLoadManager->currentCharacterID);
	DeleteSaves(a_saveLoadManager);
	if (startNewGameOnDeath) {
		initNewGame = true;
	}
	return initNewGame ? false : a_saveLoadManager->lastFileNameLength != 0;
}

void Manager::Draw()
{
	const static auto screenSize = ImGui::GetNativeViewportSize();
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0, 0), screenSize, IM_COL32_BLACK);
}

void Manager::StartNewGame()
{
	using func_t = decltype(&StartNewGame);
	static REL::Relocation<func_t> func{ RELOCATION_ID(51246, 52118) };
	return func();
}

// https://stackoverflow.com/questions/70257751/move-a-file-or-folder-to-the-recyclebin-trash-c17
bool Manager::RecycleFile(const std::wstring& a_filePath)
{
	const std::wstring widestr = a_filePath + L'\0';

	SHFILEOPSTRUCT fileOp;
	fileOp.hwnd = nullptr;
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = widestr.c_str();
	fileOp.pTo = nullptr;
	fileOp.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI;

	return SHFileOperation(&fileOp) == 0;
}

void Manager::DeleteSaves(RE::BGSSaveLoadManager* a_saveLoadManager)
{
	if (!deleteSaves) {
		return;
	}

	const auto currentPlayerID = a_saveLoadManager->currentCharacterID;

	if (currentPlayerID == 0) {
		logger::warn("\tCurrent Player ID is 0, skipping save deletion.");
		return;
	}

	constexpr auto get_save_directory = []() -> std::optional<std::filesystem::path> {
		if (auto path = logger::log_directory()) {
			path->remove_filename();  // remove "/SKSE"
			path->append("sLocalSavePath:General"_ini.value());
			return path;
		}
		return std::nullopt;
	};

	if (!saveDirectory) {
		saveDirectory = get_save_directory();
	}

	std::error_code ec;
	if (saveDirectory && std::filesystem::exists(*saveDirectory, ec)) {
		std::vector<std::filesystem::path> savesToDelete;

		for (const auto& entry : std::filesystem::directory_iterator(*saveDirectory)) {
			if (entry.exists() && entry.path().extension() == ".ess") {
				if (const auto save = string::split(entry.path().filename().string(), "_"); save.size() == 9) {
					const auto saveID = string::to_num<std::uint64_t>(save[1], true);
					if (saveID == currentPlayerID) {
						savesToDelete.push_back(entry.path());
					}
				}
			}
		}
		
		for (const auto& save : savesToDelete) {
			logger::info("\tDeleting save {}", save.filename().string());
			if (recycleSaves) {
				RecycleFile(save.wstring());
			} else {
				std::filesystem::remove(save);
			}
		}
	}
}

RE::BSEventNotifyControl Manager::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
{
	if (!a_event || a_event->menuName != RE::MainMenu::MENU_NAME) {
		return RE::BSEventNotifyControl::kContinue;
	}

	if (a_event->opening) {
		if (startNewGameOnDeath && initNewGame) {
			logger::info("\tRestarting game...");
			StartNewGame();
		}
	} else {
		initNewGame = false;
	}

	return RE::BSEventNotifyControl::kContinue;
}
