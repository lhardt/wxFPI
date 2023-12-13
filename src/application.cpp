/**
 * Application.cpp - Entry point for application.
 *
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#include "gui.h"
//#include <opencv2/opencv.hpp>
#include <iostream>

bool Application::OnInit() {
	InitLogging();
	// InitEditorWindow();
	InitVideoWindow();
	return true;
}
void Application::InitEditorWindow() {
	EditorWindow* window = new EditorWindow(this);
	window->SetPosition(wxPoint(20, 20));
	window->Show();
	SetTopWindow(window);
}

void Application::InitVideoWindow() {
	VideoWindow* window = new VideoWindow(this);
	window->SetPosition(wxPoint(20, 20));
	window->Show();
	SetTopWindow(window);
}

void Application::InitLogging() {
	wxLog* logger = new wxLogStream(&std::cout);
	wxLog::SetActiveTarget(logger);

	wxLogWindow* logger_window = new wxLogWindow(NULL, wxT("Logger Information"), false);
	logger_window->GetFrame()->SetPosition(wxPoint(1200, 20));
	logger_window->Show();
}

wxIMPLEMENT_APP(Application);
