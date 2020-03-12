#pragma once
class config
{
private:
	bool firstInit = true;
	bool cfg_filelog_switch = false;
	int cfg_filelog_size = 300;
	bool cfg_filelog_eztrans_loc = true;
	bool cfg_filelog_startup_clear = true;
	bool cfg_console_switch = true;
	int cfg_console_maxline = 300;
	wchar_t cfg_console_fontname[255];
	int cfg_console_fontsize = 12;

public:
	bool LoadConfig();
	bool SaveConfig();
	config();
	~config();

	bool GetFileLogSwitch() { return cfg_filelog_switch; }
	void SetFileLogSwitch(bool b) { cfg_filelog_switch = b; }

	int GetFileLogSize() { return cfg_filelog_size; }
	void SetFileLogSize(int n) { cfg_filelog_size = n; }

	bool GetFileLogEztLoc() { return cfg_filelog_eztrans_loc; }
	void SetFileLogEztLoc(bool b) { cfg_filelog_eztrans_loc = b; }

	bool GetFileLogStartupClear() { return cfg_filelog_startup_clear; }
	void SetFileLogStartupClear(bool b) { cfg_filelog_startup_clear = b; }

	bool GetConsoleSwitch() { return cfg_console_switch; }
	void SetConsoleSwitch(bool b) { cfg_console_switch = b; ShowLogWin(b); }

	wchar_t *GetConsoleFontName() { return cfg_console_fontname; }
	void SetConsoleFontName(wchar_t *str) { wcscpy_s(cfg_console_fontname, str); }

	int GetConsoleMaxLine() { return cfg_console_maxline; }
	void SetConsoleMaxLine(int n) { cfg_console_maxline = n; }

	int GetConsoleFontSize() { return cfg_console_fontsize; }
	void SetConsoleFontSize(int n) { cfg_console_fontsize = n; }

};

