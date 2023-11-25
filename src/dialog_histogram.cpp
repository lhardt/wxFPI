#include "gui.h"


HistogramDialog::HistogramDialog(EditorWindow* parent) : m_parent(parent),
		wxDialog(parent, wxID_ANY, wxT("Histogram Visualization"), wxDefaultPosition, wxDefaultSize) {
	InitControls();
}

void HistogramDialog::InitControls() {
	m_bitmap = wxBitmap(256, 100);
	m_view = new wxStaticBitmap(this, wxID_ANY, m_bitmap);
	Refresh();
	Fit();
}
//Application* m_app;
//EditorWindow* m_parent;

void HistogramDialog::UpdateHistogram(Image& image) {
	Histogram hist(image);

	wxMemoryDC image_dc(m_bitmap);

//	image_dc.SetBackground(wxBrush(wxColor(0, 0, 0)));
	image_dc.SetPen(wxPen(wxColor(0, 0, 0)));
	image_dc.SetBrush(wxBrush(wxColor(0, 0, 0)));
	image_dc.DrawRectangle(0, 0, 256, 100);

	image_dc.SetPen(wxPen(wxColor(255, 255, 255)));
	image_dc.SetBrush(wxBrush(wxColor(255, 255, 255)));
//	image_dc.SetBrush(wxBrush(wxColor(255, 255, 255)));

	for (int i = 0; i < 256; ++i) {
		int height = hist.GetLumHist()[i] * 100 / hist.GetMaxValue();
		image_dc.DrawLine(i, 99-height, i, 99);
	}
	m_bitmap = image_dc.GetAsBitmap();
	DestroyChildren();
	m_view = new wxStaticBitmap(this, wxID_ANY, m_bitmap);
	Refresh();
	Fit();
}

void HistogramDialog::SetVisibility(bool visible) {

}
