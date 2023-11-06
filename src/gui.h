/**
 * gui.h - Main window for Editor application.
 *
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#ifndef WXFPI_GUI_H
#define WXFPI_GUI_H

#include "wx.h"

#include "image.h"

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
	wxButton* m_btn_reset;
	wxButton* m_btn_invert;

	wxSpinCtrl* m_spin_quant;

	bool has_image;
	std::unique_ptr<Image> m_image;
	std::unique_ptr<Image> m_original_image;

	void InitMenuBar();
	void InitControls();
	void ShowImage();
public:
	EditorWindow(Application * app);

	void OnMenuItemClicked(wxEvent & evt);
	void OnOpenFileClicked(wxEvent& evt);
	void OnSaveImageClicked(wxEvent& evt);

	/* Transformations */
	void OnInvertButtonClicked(wxEvent& evt);
	void OnVerticalButtonClicked(wxEvent& evt);
	void OnHorizontalButtonClicked(wxEvent& evt);
	void OnQuantizedButtonClicked(wxEvent& evt);
	void OnGreyButtonClicked(wxEvent & evt);
	void OnResetButtonClicked(wxEvent& evt);
};

#endif /* WXFPI_GUI_H */