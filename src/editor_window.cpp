/**
 * editor_window.cpp - Main window for Editor application.
 * 
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#include "gui.h"

enum wxID_EditorWindow {
	DO_NOT_USE = wxID_HIGHEST,

	ID_MENU_NEW,
	ID_MENU_OPEN,
	ID_MENU_EXIT,
	ID_MENU_ABOUT
};

EditorWindow::EditorWindow(Application* app) : wxFrame(NULL, wxID_ANY, "wxFPI Image Editor") {
	// SetBackgroundColour(wxColor("#2D2D30"));
	InitMenuBar();
	InitControls();
}

void EditorWindow::InitControls() {
	m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition);

	m_btn_grey = new wxButton(m_panel, wxID_ANY, wxT("G"));
	m_btn_hor = new wxButton(m_panel, wxID_ANY, wxT("H"));
	m_btn_ver = new wxButton(m_panel, wxID_ANY, wxT("V"));
	m_btn_quant = new wxButton(m_panel, wxID_ANY, wxT("Q"));

	m_image_container = new wxScrolledWindow(m_panel, wxID_ANY);
	m_image_container->SetBackgroundColour(wxColor("#2D2D30"));
		
	wxSizer* root_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* controls_sizer = new wxBoxSizer(wxVERTICAL);

	controls_sizer->Add(m_btn_hor, 0, 0);
	controls_sizer->Add(m_btn_ver, 0, 0);
	controls_sizer->Add(m_btn_grey, 0, 0);
	controls_sizer->Add(m_btn_quant, 0, 0);

	root_sizer->Add(controls_sizer, 0, wxEXPAND);
	root_sizer->Add(m_image_container, 1, wxEXPAND);
	m_panel->SetSizer(root_sizer);

	m_image_container->SetScrollRate(5, 5);
	m_image_container->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT);

	m_btn_hor->Bind(wxEVT_BUTTON, &EditorWindow::OnHorizontalButtonClicked, this);
	m_btn_ver->Bind(wxEVT_BUTTON, &EditorWindow::OnVerticalButtonClicked, this);
	m_btn_grey->Bind(wxEVT_BUTTON, &EditorWindow::OnGreyButtonClicked, this);
	m_btn_quant->Bind(wxEVT_BUTTON, &EditorWindow::OnQuantizedButtonClicked, this);
}

void EditorWindow::OnGreyButtonClicked(wxEvent & evt) {
	wxLogInfo("Perform 'Grey' Operation!");
}

void EditorWindow::OnHorizontalButtonClicked(wxEvent& evt) {
	wxLogInfo("Perform 'Horizontal' Operation!");
}

void EditorWindow::OnVerticalButtonClicked(wxEvent& evt) {
	wxLogInfo("Perform 'Vertical' Operation!");
}

void EditorWindow::OnQuantizedButtonClicked(wxEvent& evt) {
	wxLogInfo("Perform 'Quantize' Operation!");
}

void EditorWindow::OnResetButtonClicked(wxEvent& evt) {
	wxLogInfo("Perform 'Reset' Operation!");
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

void EditorWindow::ShowImage() {
	m_image_container->DestroyChildren();
	wxStaticBitmap* image = new wxStaticBitmap(m_image_container, wxID_ANY, m_image->GetWxBitmap());

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddStretchSpacer(1);
	sizer->Add(image, 0, wxALIGN_CENTER);
	sizer->AddStretchSpacer(1);
	m_image_container->SetSizer(sizer);
	m_image_container->FitInside();
}


void EditorWindow::OnOpenFileClicked(wxEvent&) {
	wxLogInfo("Open new file!");
	wxFileDialog dialog(this, wxT("Select Image"), ".\\res", "test.png", "*.png|*.jpg");
	int result = dialog.ShowModal();

	if (wxID_CANCEL == result) {
		wxLogInfo("Canceled operation!");
		return;
	}

	wxString filename = dialog.GetPath();
	wxLogInfo("Chosen file: <%s>!", filename);
	m_image = std::unique_ptr<Image>(new Image(filename.ToStdString()));

	ShowImage();
}

void EditorWindow::OnMenuItemClicked(wxEvent& evt) {
	switch (evt.GetId()) {
	case ID_MENU_NEW:
		wxLogInfo("New!");
		break;
	case ID_MENU_OPEN:
		OnOpenFileClicked(evt);
		break;
	case ID_MENU_ABOUT:
		wxLogInfo("About!");
		break;
	case ID_MENU_EXIT:
		wxLogInfo("Close!");
		this->Close();
		break;
	}
}
