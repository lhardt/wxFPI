#include "gui.h"



VideoWindow::VideoWindow(Application * app) : wxFrame(NULL, wxID_ANY, APP_TITLE, wxDefaultPosition, wxSize(500,500)), m_image(1,1) {
	m_app = app;
	m_refresh_rate_ms = DEFAULT_FRAME_RATE;

	// Get camera dimensions before layout
	InitCamera();
	InitLayout();

	m_refresh_timer.Bind(wxEVT_TIMER, &VideoWindow::OnTimerRefresh, this);
	m_refresh_timer.Start(m_refresh_rate_ms);
}

void VideoWindow::InitCamera() {
	cv::VideoCapture cap;
	m_opened = m_video_source.open(0);
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

void VideoWindow::OnTimerRefresh(wxEvent& evt) {
	m_refresh_timer.Stop();
	GetNextFrame();
	Refresh();
	m_refresh_timer.Start(m_refresh_rate_ms);
}

void VideoWindow::GetNextFrame() {
	cv::Mat frame;
	m_video_source >> frame;
	m_image = Image(frame);
}


void VideoWindow::OnPaintImage(wxEvent & evt) {
	wxLogInfo("Paint event! ");

	wxLogInfo("Size %d %d ", m_image_container->GetSize().x, m_image_container->GetSize().y);
	wxPaintDC dc(m_image_container);

	dc.DrawBitmap(m_image.GetWxBitmap(), 0, 0);
}