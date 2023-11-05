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
	wxPanel* m_panel;
	wxScrolledWindow * m_image_container;

	wxButton* m_btn_hor;
	wxButton* m_btn_ver;
	wxButton* m_btn_grey;
	wxButton* m_btn_quant;

	void InitMenuBar();
	void InitControls();
public:
	EditorWindow(Application * app);

	void OnMenuItemClicked(wxEvent & evt);
	void OnOpenFileClicked(wxEvent& evt);

	/* Transformations */
	void OnVerticalButtonClicked(wxEvent& evt);
	void OnHorizontalButtonClicked(wxEvent& evt);
	void OnQuantizedButtonClicked(wxEvent& evt);
	void OnGreyButtonClicked(wxEvent & evt);
};

#endif /* WXFPI_GUI_H */