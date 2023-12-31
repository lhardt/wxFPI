/**
 * editor_window.cpp - Main window for Editor application.
 * 
 * Project wxFPI: Image Editing Tools
 * (C) 2023 L�o Hardt <leom.hardt@gmail.com>.
 */
#include "gui.h"

/*********************************************************/
/*                    INITIALIZATION                     */
/*********************************************************/

EditorWindow::EditorWindow(Application* app) : wxFrame(NULL, wxID_ANY, APP_TITLE, wxDefaultPosition, wxSize(500,500)) {
	has_image = false;
	InitMenuBar();
	InitControls();
	InitStatusBar();
	InitDialogs();
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

	//m_spin_quant = new wxSpinCtrl(m_panel);
	//m_spin_quant->SetMin(1);
	//m_spin_quant->SetMax(255);

	m_image_container = new wxScrolledWindow(m_panel, wxID_ANY);
	m_image_container->SetBackgroundColour(wxColor(BKG_COLOR));
		
	wxSizer* root_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* controls_sizer = new wxBoxSizer(wxVERTICAL);

	controls_sizer->Add(m_btn_hor, 0, wxEXPAND);
	controls_sizer->Add(m_btn_ver, 0, wxEXPAND);
	controls_sizer->Add(m_btn_invert, 0, wxEXPAND);
	controls_sizer->Add(m_btn_grey, 0, wxEXPAND);
	controls_sizer->Add(new wxStaticLine(m_panel), 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
	//controls_sizer->Add(m_spin_quant, 0, wxEXPAND);
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

void EditorWindow::InitMenuBar() {
	wxMenuBar* menu_bar = new wxMenuBar();

	wxMenu* menu_file = new wxMenu();
	{
		wxMenuItem* item_file_new = new wxMenuItem(menu_file, wxID_ANY, wxT("New\tCtrl+N"), wxT("Create a new file to edit"));
		wxMenuItem* item_file_save = new wxMenuItem(menu_file, wxID_ANY, wxT("Save\tCtrl+S"), wxT("Save the current image to a file"));
		wxMenuItem* item_file_open = new wxMenuItem(menu_file, wxID_ANY, wxT("Open\tCtrl+O"), wxT("Open an existing file to edit"));
		wxMenuItem* item_file_exit = new wxMenuItem(menu_file, wxID_ANY, wxT("Exit\tCtrl+W"), wxT("Close this Editor window"));

		menu_file->Append(item_file_new);
		menu_file->Append(item_file_save);
		menu_file->Append(item_file_open);
		menu_file->AppendSeparator();
		menu_file->Append(item_file_exit);

		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnNewFileClicked, this, item_file_new->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnSaveImageClicked, this, item_file_save->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnOpenFileClicked, this, item_file_open->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnExitClicked, this, item_file_exit->GetId());
		
		menu_bar->Append(menu_file, wxT("&File"));
	}

	wxMenu* menu_image = new wxMenu();
	{
		wxMenuItem* item_image_zoom_in2x = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom In (2x)"), wxT(" "));
		wxMenuItem* item_image_zoom_out2x = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom Out (2x)"), wxT(" "));
		wxMenuItem* item_image_rot_l = new wxMenuItem(menu_image, wxID_ANY, wxT("Rotate Left\tCtrl+,"), wxT("Rotate the image to the left"));
		wxMenuItem* item_image_rot_r = new wxMenuItem(menu_image, wxID_ANY, wxT("Rotate Right\tCtrl+.	"), wxT("Rotate the image to the right"));
		wxMenuItem* item_image_flip_h = new wxMenuItem(menu_image, wxID_ANY, wxT("Flip Horizontally"), wxT("Flip the image horizontally"));
		wxMenuItem* item_image_flip_v = new wxMenuItem(menu_image, wxID_ANY, wxT("Flip Vertically"), wxT("Flip the image vertically"));
		wxMenuItem* item_image_reset = new wxMenuItem(menu_image, wxID_ANY, wxT("Reset\tCtrl+R"), wxT("Reset the image as originally loaded"));

		menu_image->Append(item_image_zoom_in2x);
		menu_image->Append(item_image_zoom_out2x);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_rot_l);
		menu_image->Append(item_image_rot_r);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_flip_h);
		menu_image->Append(item_image_flip_v);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_reset);

		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnZoomIn, this, item_image_zoom_in2x->GetId());
		item_image_zoom_out2x->Enable(false);
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnRotateLeft, this, item_image_rot_l->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnRotateRight, this, item_image_rot_r->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnHorizontalButtonClicked, this, item_image_flip_h->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnVerticalButtonClicked, this, item_image_flip_v->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnResetButtonClicked, this, item_image_reset->GetId());

		menu_bar->Append(menu_image, wxT("&Image"));
	}

	wxMenu* menu_color = new wxMenu();
	{
		wxMenuItem* item_color_histogram = new wxMenuItem(menu_color, wxID_ANY, wxT("Show Histogram"), wxT(" "));
		wxMenuItem* item_color_brightness = new wxMenuItem(menu_color, wxID_ANY, wxT("Brightness Enh."), wxT(" "));
		wxMenuItem* item_color_contrast = new wxMenuItem(menu_color, wxID_ANY, wxT("Contrast Enh."), wxT(" "));
		wxMenuItem* item_color_invert = new wxMenuItem(menu_color, wxID_ANY, wxT("Invert Image"), wxT(" "));
		wxMenuItem* item_color_grey = new wxMenuItem(menu_color, wxID_ANY, wxT("Convert to Greyscale"), wxT(" "));
		wxMenuItem* item_color_equalize = new wxMenuItem(menu_color, wxID_ANY, wxT("Equalize Image"), wxT(" "));
		wxMenuItem* item_color_match = new wxMenuItem(menu_color, wxID_ANY, wxT("Match Histogram"), wxT(" "));

		menu_color->Append(item_color_histogram);
		menu_color->AppendSeparator();
		menu_color->Append(item_color_brightness);
		menu_color->Append(item_color_contrast);
		menu_color->AppendSeparator();
		menu_color->Append(item_color_invert);
		menu_color->Append(item_color_grey);
		menu_color->AppendSeparator();
		menu_color->Append(item_color_equalize);
		menu_color->Append(item_color_match);

		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnShowHistogramWindow, this, item_color_histogram->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnBrightnessEnhClicked, this, item_color_brightness->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnContrastEnhClicked, this, item_color_contrast->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnInvertButtonClicked, this, item_color_invert->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnGreyButtonClicked, this, item_color_grey->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnEqualizeClicked, this, item_color_equalize->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::onHistogramMatchClicked, this, item_color_match->GetId());

		menu_bar->Append(menu_color, wxT("&Color"));
	}

	wxMenu* menu_conv = new wxMenu();
	{
		wxMenuItem* item_conv_show = new wxMenuItem(menu_conv, wxID_ANY, wxT("Custom Convolution"), wxT(" "));
		wxMenuItem* item_conv_gauss = new wxMenuItem(menu_conv, wxID_ANY, wxT("Gaussian Conv.\tCtrl+G"), wxT(" "));
		wxMenuItem* item_conv_lapl = new wxMenuItem(menu_conv, wxID_ANY, wxT("Laplacian Conv."), wxT(" "));
		wxMenuItem* item_conv_high = new wxMenuItem(menu_conv, wxID_ANY, wxT("High-Pass Conv."), wxT(" "));
		wxMenuItem* item_conv_prew_hx = new wxMenuItem(menu_conv, wxID_ANY, wxT("Prewitt Hx Conv."), wxT(" "));
		wxMenuItem* item_conv_prew_hy = new wxMenuItem(menu_conv, wxID_ANY, wxT("Prewitt Hy Conv."), wxT(" "));
		wxMenuItem* item_conv_sobel_hx = new wxMenuItem(menu_conv, wxID_ANY, wxT("Sobel Hx Conv."), wxT(" "));
		wxMenuItem* item_conv_sobel_hy = new wxMenuItem(menu_conv, wxID_ANY, wxT("Sobel Hy. Conv"), wxT(" "));

		menu_conv->Append(item_conv_show);
		menu_conv->AppendSeparator();
		menu_conv->Append(item_conv_gauss);
		menu_conv->Append(item_conv_lapl);
		menu_conv->Append(item_conv_high);
		menu_conv->Append(item_conv_prew_hx);
		menu_conv->Append(item_conv_prew_hy);
		menu_conv->Append(item_conv_sobel_hx);
		menu_conv->Append(item_conv_sobel_hy);

		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnShowConvWindow, this, item_conv_show->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvGauss, this, item_conv_gauss->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvLaplace, this, item_conv_lapl->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvHigh, this, item_conv_high->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvPrewHx, this, item_conv_prew_hx->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvPrewHy, this, item_conv_prew_hy->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvSobelHx, this, item_conv_sobel_hx->GetId());
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnConvSobelHy, this, item_conv_sobel_hy->GetId());

		menu_bar->Append(menu_conv, wxT("&Convolution"));
	}


	wxMenu* menu_help = new wxMenu();
	{
		wxMenuItem* item_help_about = new wxMenuItem(menu_help, wxID_ANY, wxT("About"), wxT("About this program"));
		menu_help->Append(item_help_about);
		menu_bar->Bind(wxEVT_MENU, &EditorWindow::OnAboutClicked, this, item_help_about->GetId());

		menu_bar->Append(menu_help, wxT("&Help"));
	}

	SetMenuBar(menu_bar);
}

void EditorWindow::InitDialogs() {
	m_kernel_dialog = new KernelDialog(this);
	m_kernel_dialog_id = m_kernel_dialog->GetId();

	m_histogram_dialog = new HistogramDialog(this);
	m_histogram_dialog_id = m_histogram_dialog->GetId();
}

/*********************************************************/
/*                       IMAGE I/O                       */
/*********************************************************/

void EditorWindow::ShowImage() {
	m_image_container->Freeze();
	m_image_container->DestroyChildren();
	wxStaticBitmap* image = new wxStaticBitmap(m_image_container, wxID_ANY, m_image->GetWxBitmap());

	m_status_bar->PushStatusText(wxString::Format("%s - %d x %d", "Image", m_image->GetW(), m_image->GetH()));

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddStretchSpacer(1);
	sizer->Add(image, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 50);
	sizer->AddStretchSpacer(1);
	m_image_container->SetSizer(sizer);
	m_image_container->FitInside();
	m_image_container->Thaw();

	UpdateHistogram();
}

void EditorWindow::OnNewFileClicked(wxEvent&) {
	wxLogInfo("New file!!");
	m_image = std::unique_ptr<Image>(new Image(500, 500));
	m_original_image = std::unique_ptr<Image>(new Image(500, 500));
	ShowImage();
	has_image = true;
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

void EditorWindow::OnAboutClicked(wxEvent& evt) {
	wxLogInfo("About!");
	wxMessageDialog dialog(this, wxT(
		"wxFPI Image Editor.\n\n"
		"Image Special Effects Editor.\n"
		"(c) 2023 L�o Hardt."
	), wxT("About wxFPI"), wxOK);
	dialog.ShowModal();

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

void EditorWindow::OnExitClicked(wxEvent& evt) {
	Close();
}

/*********************************************************/
/*                   COLOR TRANSFORM.                    */
/*********************************************************/

void EditorWindow::OnShowHistogramWindow(wxEvent& evt) {
	OpenHistogramWindow();
}

bool EditorWindow::IsUsingHistogramWindow() {
	return m_histogram_dialog_id != 0
		&& wxWindow::FindWindowById(m_histogram_dialog_id) != NULL
		&& m_histogram_dialog->IsVisible();
}

void EditorWindow::OpenHistogramWindow() {
	if (m_histogram_dialog_id == 0 || wxWindow::FindWindowById(m_histogram_dialog_id) == NULL) {
		m_histogram_dialog = new HistogramDialog(this);
		m_histogram_dialog_id = m_kernel_dialog->GetId();
	}
	m_histogram_dialog->Show();
}

void EditorWindow::UpdateHistogram() {
	if (IsUsingHistogramWindow()) {
		wxLogInfo(wxT("Updating Histogram!"));
		m_histogram_dialog->UpdateHistogram(*m_image);
	}
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

void EditorWindow::OnQuantizedButtonClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Quantize' Operation with no Image!");
		return;
	}
	int amount = wxGetNumberFromUser(wxT(""),wxT("Please type in the number of colors (0-255):"), wxT("Quantize"),128,0,255); //m_spin_quant->GetValue();
	if (amount == -1) return;
	wxLogInfo("Perform 'Quantize' Operation with q=%d!", amount);
	m_image->applyQuantTranform(amount);
	ShowImage();
}

void EditorWindow::OnBrightnessEnhClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Brightness Enh' Operation with no Image!");
		return;
	}
	int amount = wxGetNumberFromUser(wxT(""), wxT("Please type in the enhancement value:"), wxT("Brightness Enhancement"), 0, -255, 255); //m_spin_quant->GetValue();
	if (amount == -1) return;
	m_image->applyBrightnessEnh(amount);
	ShowImage();
}

void EditorWindow::OnContrastEnhClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Brightness Enh' Operation with no Image!");
		return;
	}
	double amount = 0;
	wxString amount_text = wxGetTextFromUser(wxT(""), wxT("Please type in the enhancement value:"), wxT("Contrast Enhancement"));
	if (!amount_text.ToCDouble(&amount)) {
		wxLogInfo("Invalid Contrast Enh. Value! <%s>", amount_text);
		return;
	}
	m_image->applyContrastEnh(amount);
	ShowImage();
}

void EditorWindow::OnEqualizeClicked(wxEvent& evt) {
	if (!has_image) {
		wxLogInfo("Perform 'Equalize' Operation with no Image!");
		return;
	}
	m_image->applyEqualize();
	ShowImage();
}

void EditorWindow::onHistogramMatchClicked(wxEvent& evt) {
	wxLogInfo("Open new file!");
	wxFileDialog dialog(this, wxT("Select Image"), ".\\res", "test.png", "Image Files|*");
	int result = dialog.ShowModal();

	if (wxID_CANCEL == result) {
		wxLogInfo("Canceled operation!");
		return;
	}

	wxString filename = dialog.GetPath();
	wxLogInfo("Chosen file: <%s>!", filename);
	Image image(filename.ToStdString());
	Histogram histogram(image);

	m_image->applyHistogram(histogram);

	ShowImage();
	has_image = true;
}

/*********************************************************/
/*                   IMAGE TRANSFORM.                    */
/*********************************************************/

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

void EditorWindow::OnRotateLeft(wxEvent& evt) {
	wxLogInfo("Perform 'RotLeft' Operation!");

	try {
		m_image->applyRotLeftTranform();
	}
	catch (std::exception& ex) {
		wxLogError("Exception! %s", ex.what());
	}
	ShowImage();
}

void EditorWindow::OnRotateRight(wxEvent& evt) {
	wxLogInfo("Perform 'RotRight' Operation!");
	try {
		m_image->applyRotRightTranform();
	}
	catch (std::exception& ex) {
		wxLogError("Exception! %s", ex.what());
	}
	ShowImage();
}

void EditorWindow::OnZoomIn(wxEvent& evt) {
	try {
		m_image->applyZoomInTransform();
	}
	catch (std::exception& ex) {
		wxLogError("Exception! %s", ex.what());
	}
	ShowImage();
}

/*********************************************************/
/*             CONVOLUTION TRANSFORM.                    */
/*********************************************************/

void EditorWindow::OnShowConvWindow(wxEvent& evt) {
	OpenConvWindow();
}

bool EditorWindow::IsUsingConvWindow() {
	return m_kernel_dialog_id != 0 
		&& wxWindow::FindWindowById(m_kernel_dialog_id) != NULL
		&& m_kernel_dialog->IsVisible();
}

void EditorWindow::OpenConvWindow() {
	if (m_kernel_dialog_id == 0 || wxWindow::FindWindowById(m_kernel_dialog_id) == NULL) {
		m_kernel_dialog = new KernelDialog(this);
		m_kernel_dialog_id = m_kernel_dialog->GetId();
	}
	m_kernel_dialog->Show();
}

void EditorWindow::OnApplyConv(const Kernel& kernel) {
	if (!has_image) {
		wxLogInfo("Perform 'Convolution' Operation with no Image!");
		return;
	}
	try {
		wxLogInfo("Applying Kernel to Image!");
		m_image->applyKernel(kernel);
	}
	catch (std::exception& ex) {
		wxLogError("Exception! %s", ex.what());
	}
	ShowImage();
}

void EditorWindow::OnConvMenuItem(const Kernel& kernel) {
	if (IsUsingConvWindow()) {
		wxLogInfo("Setting Kernel of the Convolution Dialog!");
		m_kernel_dialog->SetKernel(kernel);
	} else {
		OnApplyConv(kernel);
	}
}

void EditorWindow::OnConvGauss(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_GAUSS);
}
void EditorWindow::OnConvLaplace(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_LAPLACE);
}
void EditorWindow::OnConvHigh(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_HIGH);
}
void EditorWindow::OnConvPrewHx(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_PREW_HX);
}
void EditorWindow::OnConvPrewHy(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_PREW_HY);
}
void EditorWindow::OnConvSobelHx(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_SOBEL_HX);
}
void EditorWindow::OnConvSobelHy(wxEvent& evt) {
	this->OnConvMenuItem(KERNEL_SOBEL_HY);
}