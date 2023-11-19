/**
 * editor_window.cpp - Main window for Editor application.
 * 
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#include "gui.h"

static const wxString APP_TITLE = "wxFPI Image Editor";

EditorWindow::EditorWindow(Application* app) : wxFrame(NULL, wxID_ANY, APP_TITLE, wxDefaultPosition, wxSize(500,500)) {
	// SetBackgroundColour(wxColor("#2D2D30"));
	has_image = false;
	InitMenuBar();
	InitControls();
	InitStatusBar();
}

void EditorWindow::InitControls() {
	m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition);

	m_btn_invert = new wxButton(m_panel, wxID_ANY, wxT("I"));
	m_btn_grey = new wxButton(m_panel, wxID_ANY, wxT("G"));
	m_btn_hor = new wxButton(m_panel, wxID_ANY, wxT("H"));
	m_btn_ver = new wxButton(m_panel, wxID_ANY, wxT("V"));
	m_btn_quant = new wxButton(m_panel, wxID_ANY, wxT("Q"));
	m_btn_reset = new wxButton(m_panel, wxID_ANY, wxT("R"));

	m_btn_invert->SetToolTip("(I)nvert each channel of the Image");
	m_btn_grey->SetToolTip("(G)reyscale: transform image into greyscale");
	m_btn_grey->SetToolTip("(H)orizontal: Mirror image horizontally");
	m_btn_ver->SetToolTip("(V)ertical: Mirror image vertically");
	m_btn_quant->SetToolTip("(Q)uantize: Reduce the amount of shades of grey in the image");
	m_btn_reset->SetToolTip("(R)eset: Undo all transformations to the current image");

	m_spin_quant = new wxSpinCtrl(m_panel);
	m_spin_quant->SetMin(1);
	m_spin_quant->SetMax(255);

	m_image_container = new wxScrolledWindow(m_panel, wxID_ANY);
	m_image_container->SetBackgroundColour(wxColor("#2D2D30"));
		
	wxSizer* root_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* controls_sizer = new wxBoxSizer(wxVERTICAL);

	controls_sizer->Add(m_btn_hor, 0, wxEXPAND);
	controls_sizer->Add(m_btn_ver, 0, wxEXPAND);
	controls_sizer->Add(m_btn_invert, 0, wxEXPAND);
	controls_sizer->Add(m_btn_grey, 0, wxEXPAND);
	controls_sizer->Add(new wxStaticLine(m_panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
	controls_sizer->Add(m_spin_quant, 0, wxEXPAND);
	controls_sizer->Add(m_btn_quant, 0, wxEXPAND);
	controls_sizer->Add(new wxStaticLine(m_panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
	controls_sizer->Add(m_btn_reset, 0, wxEXPAND);

	root_sizer->Add(controls_sizer, 0, wxEXPAND);
	root_sizer->Add(m_image_container, 1, wxEXPAND);
	m_panel->SetSizer(root_sizer);

	m_image_container->SetScrollRate(5, 5);
	m_image_container->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT);

	m_btn_hor->Bind(wxEVT_BUTTON, &EditorWindow::OnHorizontalButtonClicked, this);
	m_btn_ver->Bind(wxEVT_BUTTON, &EditorWindow::OnVerticalButtonClicked, this);
	m_btn_grey->Bind(wxEVT_BUTTON, &EditorWindow::OnGreyButtonClicked, this);
	m_btn_quant->Bind(wxEVT_BUTTON, &EditorWindow::OnQuantizedButtonClicked, this);
	m_btn_reset->Bind(wxEVT_BUTTON, &EditorWindow::OnResetButtonClicked, this);
	m_btn_invert->Bind(wxEVT_BUTTON, &EditorWindow::OnInvertButtonClicked, this);
}

void EditorWindow::InitStatusBar() {
	m_status_bar = new wxStatusBar(this);
	SetStatusBar(m_status_bar);
}

void EditorWindow::OnInvertButtonClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Invert' Operation with no Image!");
		return;
	}
	wxLogInfo("Perform 'Invert' Operation!");
	m_image->applyInvertTransform();
	ShowImage();
}

void EditorWindow::OnGreyButtonClicked(wxEvent & evt) {
	if (!has_image) { 
		wxLogInfo("Perform 'Grey' Operation with no Image!");
		return;
	}
	wxLogInfo("Perform 'Grey' Operation!");
	m_image->applyGreyTransform();
	ShowImage();
}

void EditorWindow::OnHorizontalButtonClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Horizontal' Operation with no Image!");
		return;
	}
	wxLogInfo("Perform 'Horizontal' Operation!");
	m_image->applyHorTransform();
	ShowImage();
}

void EditorWindow::OnVerticalButtonClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Vertical' Operation with no Image!");
		return;
	}
	wxLogInfo("Perform 'Vertical' Operation!");
	m_image->applyVerTransform();
	ShowImage();
}

void EditorWindow::OnQuantizedButtonClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Quantize' Operation with no Image!");
		return;
	}
	int amount = m_spin_quant->GetValue();
	wxLogInfo("Perform 'Quantize' Operation with q=%d!", amount);
	m_image->applyQuantTranform(amount);
	ShowImage();
}

void EditorWindow::OnResetButtonClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Reset' Operation with no Image!");
		return;
	}

	wxLogInfo("Perform 'Reset' Operation!");
	m_image.reset(); /* unique_ptr free method */
	m_image = std::unique_ptr<Image>(new Image(*m_original_image));
	ShowImage();

}


void EditorWindow::InitMenuBar() {
	wxMenuBar* menu_bar = new wxMenuBar();

	wxMenu* menu_file = new wxMenu();
	{
		wxMenuItem* item_file_new = new wxMenuItem(menu_file, wxID_ANY, wxT("New\tCtrl+N"), wxT("Create a new file to edit"));
		wxMenuItem* item_file_save = new wxMenuItem(menu_file, wxID_ANY, wxT("Save\tCtrl+S"), wxT("Save the current image to a file"));
		wxMenuItem* item_file_open = new wxMenuItem(menu_file, wxID_ANY, wxT("Open\tCtrl+O"), wxT("Open an existing file to edit"));
		wxMenuItem* item_file_exit = new wxMenuItem(menu_file, wxID_ANY, wxT("Exit\tCtrl+W"), wxT("Close this Editor window"));
		menu_bar->Append(menu_file, wxT("&File"));

		menu_file->Append(item_file_new);
		menu_file->Append(item_file_save);
		menu_file->Append(item_file_open);
		menu_file->AppendSeparator();
		menu_file->Append(item_file_exit);

		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnOpenFileClicked, this, item_file_open->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnNewFileClicked, this, item_file_new->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnSaveImageClicked, this, item_file_save->GetId());
	}

	wxMenu* menu_help = new wxMenu();
	{
		wxMenuItem* item_help_about = new wxMenuItem(menu_file, wxID_ANY, wxT("About"), wxT("About this program"));
		menu_bar->Append(menu_help, wxT("&Help"));
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnAboutClicked, this, item_help_about->GetId());
	}

	SetMenuBar(menu_bar);
}

void EditorWindow::ShowImage() {
	m_image_container->DestroyChildren();
	wxStaticBitmap* image = new wxStaticBitmap(m_image_container, wxID_ANY, m_image->GetWxBitmap());

	m_status_bar->PushStatusText(wxString::Format("%s - %d x %d", "Image", m_image->GetW(), m_image->GetH()));

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddStretchSpacer(1);
	sizer->Add(image, 0, wxALIGN_CENTER);
	sizer->AddStretchSpacer(1);
	m_image_container->SetSizer(sizer);
	m_image_container->FitInside();
}


void EditorWindow::OnNewFileClicked(wxEvent&) {
	wxLogInfo("New file!");
}

void EditorWindow::OnOpenFileClicked(wxEvent&) {
	wxLogInfo("Open new file!");
	wxFileDialog dialog(this, wxT("Select Image"), ".\\res", "test.png", "Image Files|*");
	int result = dialog.ShowModal();

	if (wxID_CANCEL == result) {
		wxLogInfo("Canceled operation!");
		return;
	}

	wxString filename = dialog.GetPath();
	wxLogInfo("Chosen file: <%s>!", filename);
	m_image = std::unique_ptr<Image>(new Image(filename.ToStdString()));
	m_original_image = std::unique_ptr<Image>(new Image(filename.ToStdString()));

	ShowImage();
	has_image = true;
}

void EditorWindow::OnAboutClicked(wxEvent& evt) {
	wxLogInfo("About!");
}
void EditorWindow::OnSaveImageClicked(wxEvent& evt) {
	wxLogInfo("Save!");
	wxFileDialog dialog(this, wxT("Save As"), ".\\res", "result.png", "Image Files|*");
	int result = dialog.ShowModal();

	if (wxID_CANCEL == result) {
		wxLogInfo("Canceled operation!");
		return;
	}

	wxString filename = dialog.GetPath();
	m_image->SaveAs(filename.ToStdString());
	wxLogInfo("Saved image as: <%s>!", filename);
}