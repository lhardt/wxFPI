/**
 * Application.cpp - Entry point for application.
 *
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#include "gui.h"
#include <iostream>

bool Application::OnInit() {
	InitLogging();
	// InitEditorWindow();
	CreateVideoWindow();
	return true;
}
bool Application::OnExceptionInMainLoop() {
	try { throw; }
	catch (std::exception& e)
	{
		wxLogInfo("Unhandled Exception!");
		wxLogInfo(e.what());
		wxString text = wxString("Unhandled Exception!\n\n") + e.what();
		wxMessageDialog dialog(NULL, text, wxT("Unhandled Exception!"), wxOK);
		dialog.ShowModal();
	}
	return true;   // continue on. Return false to abort program
}

void Application::CreateEditorWindow() {
	EditorWindow* window = new EditorWindow(this);
//	window->SetPosition(wxPoint(20, 20));
	window->Show();
}

void Application::CreateVideoWindow() {
	VideoWindow* window = new VideoWindow(this, &m_video_capture);
//	window->SetPosition(wxPoint(20, 20));
	window->Show();
}

void Application::InitLogging() {
	wxLog* logger = new wxLogStream(&std::cout);
	wxLog::SetActiveTarget(logger);

	wxLogWindow* logger_window = new wxLogWindow(NULL, wxT("Logger Information"), false);
	logger_window->GetFrame()->SetPosition(wxPoint(1200, 20));
	logger_window->Show();
}

wxIMPLEMENT_APP(Application);
