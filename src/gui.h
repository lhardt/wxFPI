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

class EditorWindow;

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

class EditorWindow : public wxFrame {
private:
	Application* m_app;
	KernelDialog* m_kernel_dialog;
	int m_kernel_dialog_id = 0;
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
	void InitKernelDialog();
	void ShowImage();
public:
	EditorWindow(Application * app);

	/* Basic Info */
	void OnOpenFileClicked(wxEvent& evt);
	void OnSaveImageClicked(wxEvent& evt);
	void OnNewFileClicked(wxEvent& evt);
	void OnAboutClicked(wxEvent& evt);
	void OnResetButtonClicked(wxEvent& evt);

	/* Image Transformations */
	void OnVerticalButtonClicked(wxEvent& evt);
	void OnHorizontalButtonClicked(wxEvent& evt);
	void OnRotateLeft(wxEvent& evt);
	void OnRotateRight(wxEvent& evt);

	/* Color Transformations */
	void OnInvertButtonClicked(wxEvent& evt);
	void OnQuantizedButtonClicked(wxEvent& evt);
	void OnGreyButtonClicked(wxEvent& evt);

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

class HistogramWindow : public wxFrame {
private:
	Application* m_app;
	EditorWindow* m_parent;

public:
	HistogramWindow(EditorWindow* parent);

	void UpdateHistogram();
	void SetVisibility(bool visible);
};

#endif /* WXFPI_GUI_H */