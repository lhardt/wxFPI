/**
 * EditorWindow.h - Main window for Editor application.
 *
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#ifndef WXFPI_GUI_H
#define WXFPI_GUI_H

#include "wx.h"

class Application : public wxApp {
private:
	void InitLogging();
public:
	virtual bool OnInit();
};

class EditorWindow : public wxFrame {
private:
	Application* m_app;

	void InitMenuBar();
public:
	EditorWindow(Application * app);

	void OnMenuItemClicked(wxEvent & evt);
};

#endif /* WXFPI_GUI_H */