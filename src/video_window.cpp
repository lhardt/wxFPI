#include "gui.h"
#include "effects.h"

VideoWindow::VideoWindow(Application* app, cv::VideoCapture * vc, wxString window_name)
							: wxFrame(NULL, wxID_ANY, window_name, wxDefaultPosition, wxSize(500,500)), m_image(1,1) {
	m_app = app;
	m_refresh_rate_ms = DEFAULT_FRAME_RATE;
	m_video_source = vc;

	// Get camera dimensions before layout
	InitCamera();
	InitLayout();
	InitMenuBar();
	InitStatusBar();

	m_refresh_timer.Bind(wxEVT_TIMER, &VideoWindow::OnTimerRefresh, this);
	m_refresh_timer.Start(m_refresh_rate_ms);
	Bind(wxEVT_CLOSE_WINDOW, &VideoWindow::OnClosing, this);
}

void VideoWindow::InitMenuBar() {
	wxMenuBar* menu_bar = new wxMenuBar();

	{
		wxMenu* menu_file = new wxMenu();
		wxMenuItem* item_file_new = new wxMenuItem(menu_file, wxID_ANY, wxT("New Window\tCtrl+N"), wxT("Create a new file to edit"));

		menu_file->Append(item_file_new);

		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnNewWindowClicked, this, item_file_new->GetId());
		//menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnSaveImageClicked, this, item_file_save->GetId());

		menu_bar->Append(menu_file, wxT("&File"));
	}
	
	{
		wxMenu* menu_image = new wxMenu();
		wxMenuItem* item_image_zoom_in2x = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom In (2x)"), wxT(" "));
		wxMenuItem* item_image_zoom_out2x = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom Out (2x)"), wxT(" "));
		wxMenuItem* item_image_resize = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom"), wxT(" "));
		wxMenuItem* item_image_rot_l = new wxMenuItem(menu_image, wxID_ANY, wxT("Rotate Left\tCtrl+,"), wxT("Rotate the image to the left"));
		wxMenuItem* item_image_rot_r = new wxMenuItem(menu_image, wxID_ANY, wxT("Rotate Right\tCtrl+.	"), wxT("Rotate the image to the right"));
		wxMenuItem* item_image_flip_h = new wxMenuItem(menu_image, wxID_ANY, wxT("Flip Horizontally"), wxT("Flip the image horizontally"));
		wxMenuItem* item_image_flip_v = new wxMenuItem(menu_image, wxID_ANY, wxT("Flip Vertically"), wxT("Flip the image vertically"));
		wxMenuItem* item_image_clear = new wxMenuItem(menu_image, wxID_ANY, wxT("Reset\tCtrl+R"), wxT("Clear all effects"));

		menu_image->Append(item_image_zoom_in2x);
		menu_image->Append(item_image_zoom_out2x);
		menu_image->Append(item_image_resize);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_rot_l);
		menu_image->Append(item_image_rot_r);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_flip_h);
		menu_image->Append(item_image_flip_v);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_clear);

		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnZoomInEffect, this, item_image_zoom_in2x->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnZoomOutEffect, this, item_image_zoom_out2x->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnResizeEffect, this, item_image_resize->GetId());
		//item_image_zoom_out2x->Enable(false);
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnRotateLeft, this, item_image_rot_l->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnRotateRight, this, item_image_rot_r->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnHorizontalButtonClicked, this, item_image_flip_h->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnVerticalButtonClicked, this, item_image_flip_v->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnClearEffectStack, this, item_image_clear->GetId());

		menu_bar->Append(menu_image, wxT("&Image"));
	}

	wxMenu* menu_color = new wxMenu();
	{
		wxMenuItem* item_color_brightness = new wxMenuItem(menu_color, wxID_ANY, wxT("Brightness Enh."), wxT(" "));
		wxMenuItem* item_color_contrast = new wxMenuItem(menu_color, wxID_ANY, wxT("Contrast Enh."), wxT(" "));
		wxMenuItem* item_color_invert = new wxMenuItem(menu_color, wxID_ANY, wxT("Invert Image"), wxT(" "));
		wxMenuItem* item_color_grey = new wxMenuItem(menu_color, wxID_ANY, wxT("Convert to Greyscale"), wxT(" "));
		wxMenuItem* item_color_equalize = new wxMenuItem(menu_color, wxID_ANY, wxT("Equalize Image"), wxT(" "));

		menu_color->Append(item_color_brightness);
		menu_color->Append(item_color_contrast);
		menu_color->AppendSeparator();
		menu_color->Append(item_color_invert);
		menu_color->Append(item_color_grey);
		menu_color->AppendSeparator();
		menu_color->Append(item_color_equalize);

		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnBrightnessEnhClicked, this, item_color_brightness->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnContrastEnhClicked, this, item_color_contrast->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnInvertButtonClicked, this, item_color_invert->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnGreyButtonClicked, this, item_color_grey->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnEqualizeClicked, this, item_color_equalize->GetId());

		menu_bar->Append(menu_color, wxT("&Color"));
	}

	{
		wxMenu* menu_conv = new wxMenu();
		wxMenuItem* item_conv_gauss = new wxMenuItem(menu_conv, wxID_ANY, wxT("Gaussian Conv.\tCtrl+G"), wxT(" "));
		wxMenuItem* item_conv_canny = new wxMenuItem(menu_conv, wxID_ANY, wxT("Canny Conv."), wxT(" "));
		wxMenuItem* item_conv_sobel = new wxMenuItem(menu_conv, wxID_ANY, wxT("Sobel Conv."), wxT(" "));

		menu_conv->Append(item_conv_gauss);
		menu_conv->Append(item_conv_canny);
		menu_conv->Append(item_conv_sobel);

		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnGaussianEffect, this, item_conv_gauss->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnCannyEffect, this, item_conv_canny->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnSobelEffect, this, item_conv_sobel->GetId());

		menu_bar->Append(menu_conv, wxT("&Convolution"));
	}


	
	SetMenuBar(menu_bar);
}

void VideoWindow::InitCamera() {
	cv::VideoCapture cap;
	m_opened = m_video_source->isOpened() || m_video_source->open(0);
	GetNextFrame();
}

void VideoWindow::InitLayout() {
	wxScrolledWindow* scroller_frame = new wxScrolledWindow(this, wxID_ANY);
	m_image_container = new wxWindow(scroller_frame, wxID_ANY);

	m_image_container->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	m_image_container->SetMinSize(wxSize(m_image.GetW(), m_image.GetH()));
	m_image_container->SetMaxSize(wxSize(m_image.GetW(), m_image.GetH()));
	scroller_frame->SetBackgroundColour(wxColor(BKG_COLOR));
	scroller_frame->SetScrollRate(5, 5);
	scroller_frame->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT);


	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(scroller_frame, 1, wxEXPAND);
	SetSizerAndFit(topSizer);

	wxSizer* scrollerSizer = new wxBoxSizer(wxVERTICAL);
	scrollerSizer->AddStretchSpacer();
	scrollerSizer->Add(m_image_container, 0, wxALIGN_CENTER);
	scrollerSizer->AddStretchSpacer();
	scroller_frame->SetSizerAndFit(scrollerSizer);

	SetSize(wxSize(
		scroller_frame->GetSize().x + 100,
		scroller_frame->GetSize().y + 100
	));

	m_image_container->Bind(wxEVT_PAINT, &VideoWindow::OnPaintImage, this);
}

void VideoWindow::InitStatusBar() {
	m_status_bar = new wxStatusBar(this);
	m_status_bar->SetFieldsCount(2);
	SetStatusBar(m_status_bar);
	UpdateStatusBarText();
}


void VideoWindow::OnNewWindowClicked(wxEvent& evt) {
	m_app->CreateVideoWindow();

}

void VideoWindow::OnTimerRefresh(wxEvent& evt) {
	m_refresh_timer.Stop();
	GetNextFrame();
	ApplyEffects();
	Refresh();
	m_refresh_timer.Start(m_refresh_rate_ms);
}

void VideoWindow::GetNextFrame() {
	cv::Mat frame;
	*m_video_source >> frame;
	m_image = Image(frame);
}

void VideoWindow::ApplyEffects() {
	for (auto& eff : m_effect_stack) {
		eff->applyToImage(m_image);
	}
	wxSize oldSize = m_image_container->GetMinSize();
	m_image_container->SetMinSize(wxSize(m_image.GetW(), m_image.GetH()));
	m_image_container->SetMaxSize(wxSize(m_image.GetW(), m_image.GetH()));
	wxSize newSize = m_image_container->GetMinSize();
	
	if (oldSize != newSize) {
		Layout();
	}
	m_image.refreshWxView();
}

void VideoWindow::OnPaintImage(wxEvent & evt) {
	wxPaintDC dc(m_image_container);
	dc.DrawBitmap(m_image.GetWxBitmap(), 0, 0);
}

void VideoWindow::OnGaussianEffect(wxEvent& evt) {
	int range = wxGetNumberFromUser(wxT(""), wxT("Please type in the range value (ODD VALUE!):"), wxT("Gaussian Blur"), 1, 1, 30);
	if(range == -1) return;
	if (range % 2 == 0) {
		wxLogError("Tried to insert an even value!\n");
		return;
	}
	int odd_range = 2 * range - 1;

	AddEffect(new GaussianEffect(odd_range));
}

void VideoWindow::OnHorizontalButtonClicked(wxEvent& evt) {
	AddEffect(new HorizontalEffect());
}

void VideoWindow::OnVerticalButtonClicked(wxEvent& evt) {
	AddEffect(new VerticalEffect());
}

void VideoWindow::OnRotateLeft(wxEvent& evt) {
	AddEffect(new LeftRotationEffect());
}
void VideoWindow::OnRotateRight(wxEvent& evt) {
	AddEffect(new RightRotationEffect());
}

void VideoWindow::OnZoomInEffect(wxEvent& evt) {
	int w = m_image.GetW() * 2;
	int h = m_image.GetH() * 2;
	AddEffect(new ResizeEffect(w, h));
}
void VideoWindow::OnZoomOutEffect(wxEvent& evt) {
	int w = m_image.GetW() / 2;
	int h = m_image.GetH() / 2;
	AddEffect(new ResizeEffect(w, h));
}
void VideoWindow::OnResizeEffect(wxEvent& evt) {
	int curr_w = m_image.GetW();
	int curr_h = m_image.GetH();
	int w = wxGetNumberFromUser(wxT(""), wxT("Please type in the new Width:"), wxT("Resize Image"), curr_w, 1, 2500);
	if (w == -1) return;
	int h = wxGetNumberFromUser(wxT(""), wxT("Please type in the new Height:"), wxT("Resize Image"), curr_h, 1, 2500);
	if (h == -1) return;

	AddEffect(new ResizeEffect(w, h));
}


void VideoWindow::OnBrightnessEnhClicked(wxEvent& evt) {
	int x = wxGetNumberFromUser(wxT(""), wxT("Please type in the enhancement value:"), wxT("Brightness Enhancement"), 0, -255, 255);
	if (x == -1) return; // TODO: how to distinguish input -1
	AddEffect(new LinearEffect(0, x));
}
void VideoWindow::OnContrastEnhClicked(wxEvent& evt) {
	int x = wxGetNumberFromUser(wxT(""), wxT("Please type in the enhancement value:"), wxT("Contrast Enhancement"), 0, -255, 255);
	if (x == -1) return; // TODO: how to distinguish input -1
	AddEffect(new LinearEffect(x, 0));
}
void VideoWindow::OnInvertButtonClicked(wxEvent& evt) {
	AddEffect(new InvertEffect());
}
void VideoWindow::OnGreyButtonClicked(wxEvent& evt) {
	AddEffect(new GreyscaleEffect());
}
void VideoWindow::OnEqualizeClicked(wxEvent& evt) {
	AddEffect(new EqualizeEffect());
}

void VideoWindow::OnCannyEffect(wxEvent& evt) {
	int t1 = wxGetNumberFromUser(wxT(""), wxT("Please type in the T1 value:"), wxT("Canny"), 1, 1, 30);
	if (t1 == -1) return;
	int t2 = wxGetNumberFromUser(wxT(""), wxT("Please type in the T2 value:"), wxT("Canny"), 1, 1, 30);
	if (t2 == -1) return;

	AddEffect(new GreyscaleEffect());
	AddEffect(new CannyEffect(t1,t2));
}
void VideoWindow::OnSobelEffect(wxEvent& evt) {
	int d = wxGetNumberFromUser(wxT(""), wxT("Please type in the size value (1, 3, 5 or 7):"), wxT("Sobel"), 1, 1, 30);
	if (d == -1) return;
	if (d != 1 && d != 3 && d != 5 && d != 7) {
		wxLogError("Input was different than 1,3,5,7. Input: %d ", d);
	}

	AddEffect(new SobelEffect(2, 2, d));
}
void VideoWindow::OnGreyscaleEffect(wxEvent& evt) {
	AddEffect(new GreyscaleEffect());

}



void VideoWindow::UpdateStatusBarText() {
	int fps = 1000 / m_refresh_rate_ms;
	wxString status_text = wxString::Format("Video: %d x %d - %d FPS - %zu Effects", m_image.GetW(), m_image.GetH(), fps, m_effect_stack.size());
	m_status_bar->SetStatusText(status_text, 1);
}
void VideoWindow::AddEffect(Effect * eff) {
	wxLogInfo("Adding Effect [%s]", eff->describe());
	m_effect_stack.push_back(std::unique_ptr<Effect>(eff));
	UpdateStatusBarText();
}

void VideoWindow::OnClosing(wxEvent & evt) {
	Freeze();
	m_refresh_timer.Stop();
	evt.Skip();
}

void VideoWindow::OnClearEffectStack(wxEvent& evt) {
	m_effect_stack.clear();
	UpdateStatusBarText();
}