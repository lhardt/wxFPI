#include "gui.h"



VideoWindow::VideoWindow(Application * app) : wxFrame(NULL, wxID_ANY, APP_TITLE, wxDefaultPosition, wxSize(500, 500)), m_image(1,1) {
	m_app = app;
	m_refresh_rate_ms = 100;
	cv::VideoCapture cap;
	m_opened = m_video_source.open(0);

	m_image_container = new wxScrolledWindow(this, wxID_ANY);
	m_image_container->SetBackgroundColour(wxColor("#2D2D30"));
	m_image_container->SetScrollRate(5, 5);
	m_image_container->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_DEFAULT);

	m_refresh_timer.Bind(wxEVT_TIMER, &VideoWindow::OnTimerRefresh, this);
	m_refresh_timer.Start(m_refresh_rate_ms);
	

}

void VideoWindow::GetNextFrame() {
	cv::Mat frame;
	m_video_source >> frame;
	m_image = Image(frame);
	ShowImage();
}

void VideoWindow::ShowImage() {
	m_image_container->Freeze();
	m_image_container->DestroyChildren();
	wxStaticBitmap* image = new wxStaticBitmap(m_image_container, wxID_ANY, m_image.GetWxBitmap());

	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddStretchSpacer(1);
	sizer->Add(image, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 50);
	sizer->AddStretchSpacer(1);
	m_image_container->SetSizer(sizer);
	m_image_container->FitInside();
	m_image_container->Thaw();
}

void VideoWindow::OnTimerRefresh(wxEvent & evt) {
	static long i_refresh = 0;
	GetNextFrame();
	wxLogInfo("Refresh %d!", i_refresh++);
}