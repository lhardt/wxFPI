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

	EditorWindow * window = new EditorWindow(this);
	window->SetPosition(wxPoint(20, 20));
	window->Show();
	SetTopWindow(window);

	return true;
}

void Application::InitLogging() {
	wxLog* logger = new wxLogStream(&std::cout);
	wxLog::SetActiveTarget(logger);

	wxLogWindow* logger_window = new wxLogWindow(NULL, wxT("Test"), false);
	logger_window->GetFrame()->SetPosition(wxPoint(800, 20));
	logger_window->Show();
}

wxIMPLEMENT_APP(Application);
