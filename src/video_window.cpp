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
	
	wxMenu* menu_image = new wxMenu();
	{
		wxMenuItem* item_image_zoom_in2x = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom In (2x)"), wxT(" "));
		wxMenuItem* item_image_zoom_out2x = new wxMenuItem(menu_image, wxID_ANY, wxT("Zoom Out (2x)"), wxT(" "));
		wxMenuItem* item_image_rot_l = new wxMenuItem(menu_image, wxID_ANY, wxT("Rotate Left\tCtrl+,"), wxT("Rotate the image to the left"));
		wxMenuItem* item_image_rot_r = new wxMenuItem(menu_image, wxID_ANY, wxT("Rotate Right\tCtrl+.	"), wxT("Rotate the image to the right"));
		wxMenuItem* item_image_flip_h = new wxMenuItem(menu_image, wxID_ANY, wxT("Flip Horizontally"), wxT("Flip the image horizontally"));
		wxMenuItem* item_image_flip_v = new wxMenuItem(menu_image, wxID_ANY, wxT("Flip Vertically"), wxT("Flip the image vertically"));
		wxMenuItem* item_image_clear = new wxMenuItem(menu_image, wxID_ANY, wxT("Reset\tCtrl+R"), wxT("Clear all effects"));

		menu_image->Append(item_image_zoom_in2x);
		menu_image->Append(item_image_zoom_out2x);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_rot_l);
		menu_image->Append(item_image_rot_r);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_flip_h);
		menu_image->Append(item_image_flip_v);
		menu_image->AppendSeparator();
		menu_image->Append(item_image_clear);

		//menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnZoomIn, this, item_image_zoom_in2x->GetId());
		//item_image_zoom_out2x->Enable(false);
		//menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnRotateLeft, this, item_image_rot_l->GetId());
		//menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnRotateRight, this, item_image_rot_r->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnHorizontalButtonClicked, this, item_image_flip_h->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnVerticalButtonClicked, this, item_image_flip_v->GetId());
		menu_bar->Bind(wxEVT_MENU, &VideoWindow::OnClearEffectStack, this, item_image_clear->GetId());

		menu_bar->Append(menu_image, wxT("&Image"));
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
	m_image.refreshWxView();
}

void VideoWindow::OnPaintImage(wxEvent & evt) {
	wxPaintDC dc(m_image_container);
	dc.DrawBitmap(m_image.GetWxBitmap(), 0, 0);
}


void VideoWindow::OnHorizontalButtonClicked(wxEvent& evt) {
	AddEffect(new HorizontalEffect());
}

void VideoWindow::OnVerticalButtonClicked(wxEvent& evt) {
	AddEffect(new VerticalEffect());
}


void VideoWindow::UpdateStatusBarText() {
	int fps = 1000 / m_refresh_rate_ms;
	wxString status_text = wxString::Format("Video: %d x %d - %d FPS - %zu Effects", m_image.GetW(), m_image.GetH(), fps, m_effect_stack.size());
	m_status_bar->SetStatusText(status_text, 1);
}
void VideoWindow::AddEffect(Effect * eff) {
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