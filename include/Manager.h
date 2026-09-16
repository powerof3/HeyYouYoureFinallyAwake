#pragma once

class Manager :
	public REX::TSingleton<Manager>,
	public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	void Register();
	void LoadSettings();

	bool ShouldHideMainMenu() const;
	bool OnPlayerDeath(RE::BGSSaveLoadManager* a_saveLoadManager);

	void Draw();

private:
	static void StartNewGame();
	static bool RecycleFile(const std::wstring& a_filePath);

	void DeleteSaves(RE::BGSSaveLoadManager* a_saveLoadManager);

	RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

	// members
	static constexpr auto path = R"(Data\SKSE\Plugins\po3_HeyYouYoureFinallyAwake.ini)"sv;

	REX::TIniSetting<bool> startNewGameOnDeath{ "Settings", "bStartNewGameOnDeath", true };
	REX::TIniSetting<bool> deleteSaves{ "Settings", "bDeletePreviousCharacterSaves", true };
	REX::TIniSetting<bool> recycleSaves{ "Settings", "bSendSavesToRecycleBin", true };

	bool                                 initNewGame{ false };
	std::optional<std::filesystem::path> saveDirectory;
};
