#pragma once

class Manager :
	public REX::Singleton<Manager>,
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
	bool                                 startNewGameOnDeath{ true };
	bool                                 deleteSaves{ true };
	bool                                 recycleSaves{ true };
	bool                                 initNewGame{ false };
	std::optional<std::filesystem::path> saveDirectory;
};
