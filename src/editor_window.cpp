/**
 * editor_window.cpp - Main window for Editor application.
 * 
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#include "gui.h"

enum  {
	DO_NOT_USE = wxID_HIGHEST,

	ID_MENU_NEW,
	ID_MENU_OPEN,
	ID_MENU_EXIT,
	ID_MENU_ABOUT
};

EditorWindow::EditorWindow(Application* app) : wxFrame(NULL, wxID_ANY, "wxFPI Image Editor") {
	InitMenuBar();
}

void EditorWindow::InitMenuBar() {
	wxMenuBar* menu_bar = new wxMenuBar();
	wxMenu* menu_file = new wxMenu();
	wxMenu* menu_info = new wxMenu();

	menu_bar->Append(menu_file, wxT("&File"));
	menu_bar->Append(menu_info, wxT("&Info"));

	menu_file->Append(ID_MENU_NEW, wxT("New"), wxT("Create a new file to edit"));
	menu_file->Append(ID_MENU_OPEN, wxT("Open"), wxT("Open an existing file to edit"));
	menu_file->AppendSeparator();
	menu_file->Append(ID_MENU_EXIT, wxT("Exit\tCtrl+W"), wxT("Close this Editor window"));

	menu_info->Append(ID_MENU_ABOUT, wxT("About"), wxT("About this program"));

	menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnMenuItemClicked, this);
	SetMenuBar(menu_bar);
}

void EditorWindow::OnMenuItemClicked(wxEvent& evt) {
	switch (evt.GetId()) {
	case ID_MENU_NEW:
		wxLogInfo("Add!\n");
		break;
	case ID_MENU_OPEN:
		wxLogInfo("Open!\n");
		break;
	case ID_MENU_ABOUT:
		wxLogInfo("About!\n");
		break;
	case ID_MENU_EXIT:
		wxLogInfo("Close!\n");
		this->Close();
		break;
	}
}
