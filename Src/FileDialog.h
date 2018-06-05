#pragma once

#include "inc.h"

enum eFileDialogUsage
{
	eFileDialogUsage_OpenFile,
	eFileDialogUsage_SaveFile,
	eFileDialogUsage_OpenFolder
};

class FileDialog
{
public:
	FileDialog(const char* name, const char* ext, eFileDialogUsage usage);
	// 设置文件对话框默认文件夹;
	void SetDefaultDirectory(const std::string& szDir);
	// 调用Open()函数打开文件对话框;
	void Open();
	// 点击确定返回true，点击取消返回false，Render里每帧调用;
	bool DoModal();
	// 当前文件对话框是否打开;
	bool IsOpen();
	// DoModal()返回true时候，可以获得文件名;
	const char* GetFileName();
	// DoModal()返回true时候，可以获得文件夹名;
	const char* GetDirectory();
	// 文件名是否是指定格式的;
	static bool IsExt(const std::string& filename, const std::string& ext);
private:
	void GetFiles(const std::string& path);
private:
	std::string fileName;
	char directory[MAX_PATH];
	char cur_directory[MAX_PATH];
	bool open;
	std::string dlgName;
	eFileDialogUsage mUsage;
	std::string mExt;
	char cur_filename[MAX_PATH];
	int selFileIndex;
	int selFolderIndex;
	std::vector<std::string> files;
	std::vector<std::string> folders;
};