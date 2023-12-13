/**
 * gui.h - Main window for Editor application.
 *
 * Project wxFPI: Image Editing Tools
 * (C) 2023 Léo Hardt <leom.hardt@gmail.com>.
 */
#ifndef WXFPI_GUI_H
#define WXFPI_GUI_H

#include "wx.h"
#include "const.h"

#include "image.h"

class EditorWindow;
class VideoWindow;

class Application : public wxApp {
private:
	void InitLogging();

	cv::VideoCapture m_video_capture;
public:
	void CreateEditorWindow();
	void CreateVideoWindow();

	virtual bool OnInit();
	virtual bool OnExceptionInMainLoop() override;
};

class KernelDialog : public wxDialog {
public:
	KernelDialog(EditorWindow* parent);

	void SetKernel(const Kernel& kernel);
	Kernel GetKernel();

	void OnApplyKernelClicked(wxEvent& evt);
private:
	void InitControls();

	EditorWindow* m_parent;
	wxSpinCtrl* m_bias_input;
	wxCheckBox* m_convert_to_grey_input;
	wxSpinCtrlDouble* m_weight_inputs[3][3];
};	


class HistogramDialog : public wxDialog {
public:
	HistogramDialog(EditorWindow* parent);

	void UpdateHistogram(Image & image);
	void SetVisibility(bool visible);
private:
	void InitControls();
	Application* m_app;
	EditorWindow* m_parent;

	wxStaticBitmap* m_view;
	wxBitmap m_bitmap;
};

/** 
 * EditorWindow
 * 
 * Window control to load, add effects and save a static image.
 */
class EditorWindow : public wxFrame {
private:
	Application* m_app;

	KernelDialog* m_kernel_dialog;
	int m_kernel_dialog_id = 0;
	HistogramDialog* m_histogram_dialog;
	int m_histogram_dialog_id = 0;

	wxPanel* m_panel;
	wxScrolledWindow * m_image_container;
	wxStatusBar* m_status_bar;

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
	void InitStatusBar();
	void InitDialogs();
	void ShowImage();
public:
	EditorWindow(Application * app);

	/* Basic Info */
	void OnOpenFileClicked(wxEvent& evt);
	void OnSaveImageClicked(wxEvent& evt);
	void OnNewFileClicked(wxEvent& evt);
	void OnExitClicked(wxEvent& evt);
	void OnAboutClicked(wxEvent& evt);
	void OnResetButtonClicked(wxEvent& evt);

	/* Image Transformations */
	void OnVerticalButtonClicked(wxEvent& evt);
	void OnHorizontalButtonClicked(wxEvent& evt);
	void OnRotateLeft(wxEvent& evt);
	void OnRotateRight(wxEvent& evt);
	void OnZoomIn(wxEvent& evt);

	/* Histogram Window */
	void OnShowHistogramWindow(wxEvent& evt);
	bool IsUsingHistogramWindow();
	void OpenHistogramWindow();
	void UpdateHistogram(); 

	/* Color Transformations */
	void OnInvertButtonClicked(wxEvent& evt);
	void OnQuantizedButtonClicked(wxEvent& evt);
	void OnGreyButtonClicked(wxEvent& evt);
	void OnBrightnessEnhClicked(wxEvent& evt);
	void OnContrastEnhClicked(wxEvent& evt);
	void OnEqualizeClicked(wxEvent& evt);
	void onHistogramMatchClicked(wxEvent& evt);

	/* Convolutions */
	void OnShowConvWindow(wxEvent& evt);
	bool IsUsingConvWindow();
	void OpenConvWindow();
	void OnApplyConv(const Kernel& kernel);
	void OnConvMenuItem(const Kernel& kernel);
	
	void OnConvGauss(wxEvent& evt);
	void OnConvLaplace(wxEvent& evt);
	void OnConvHigh(wxEvent& evt);
	void OnConvPrewHx(wxEvent& evt);
	void OnConvPrewHy(wxEvent& evt);
	void OnConvSobelHx(wxEvent& evt);
	void OnConvSobelHy(wxEvent& evt);
};

/**
 * VideoWindow
 *
 * Window control to load, add effects and save a static image.
 */
class VideoWindow : public wxFrame {
private:
	Application* m_app;

	int m_refresh_rate_ms;
	cv::VideoCapture * m_video_source;
	Image m_image;
	bool m_opened;

	wxWindow* m_image_container;
	wxStatusBar* m_status_bar;
	wxTimer m_refresh_timer;

	std::vector<std::unique_ptr<Effect>> m_effect_stack;

	void InitLayout();
	void InitCamera();
	void InitMenuBar();
	void InitStatusBar();
public: 
	VideoWindow(Application* app, cv::VideoCapture * vc, wxString window_name = APP_TITLE);

	/* Utils */
	void UpdateStatusBarText();
	void AddEffect(Effect* eff);
	void OnClosing(wxEvent& evt);

	/* Main Drawing Cycle */
	void GetNextFrame();
	void ApplyEffects();
	void OnTimerRefresh(wxEvent& evt);
	void OnPaintImage(wxEvent& evt);

	/***************************
	 * User Interface Handlers *
 	 ***************************/
	void OnNewWindowClicked(wxEvent& evt);

	/* Image Transformations */
	void OnVerticalButtonClicked(wxEvent& evt);
	void OnHorizontalButtonClicked(wxEvent& evt);
	void OnRotateLeft(wxEvent& evt);
	void OnRotateRight(wxEvent& evt);

	/* General Effects */
	void OnClearEffectStack(wxEvent& evt);
};




#endif /* WXFPI_GUI_H */