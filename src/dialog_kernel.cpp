#include "gui.h"

KernelDialog::KernelDialog(EditorWindow* parent) : m_parent(parent),
			wxDialog(parent, wxID_ANY, wxT("Kernel Customization"), wxDefaultPosition, wxDefaultSize){
	InitControls();
}

void KernelDialog::InitControls() {
	m_bias_input = new wxSpinCtrl(this);
	m_convert_to_grey_input = new wxCheckBox(this, wxID_ANY, wxT("Convert to Greyscale"));
	wxButton* apply_button = new wxButton(this, wxID_ANY, wxT("Apply Convolution"));
	wxStaticText* bias_label = new wxStaticText(this, wxID_ANY, wxT("Bias"));

	apply_button->SetMinSize(wxSize(50, 20));

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);

	wxSizer* grid_sizer_container = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Kernel Weights"));
	wxSizer* grid_sizer = new wxGridSizer(3, 3, wxSize(10,10));

	wxSizer* other_container = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Kernel Weights"));
	wxSizer* other_options_sz = new wxGridSizer(2, 2, wxSize(10,0));

	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			m_weight_inputs[r][c] = new wxSpinCtrlDouble(this);
			m_weight_inputs[r][c]->SetRange(-10, 10);
			m_weight_inputs[r][c]->SetDigits(3);

			grid_sizer->Add(m_weight_inputs[r][c]);
		}
	}

	other_container->Add(other_options_sz, 0, wxEXPAND);
	other_options_sz->Add(bias_label, 0, wxLEFT, 10);
	other_options_sz->AddStretchSpacer();
	other_options_sz->Add(m_bias_input, 0, wxBOTTOM | wxLEFT | wxEXPAND, 5);
	other_options_sz->Add(m_convert_to_grey_input, 0, wxEXPAND);
	grid_sizer_container->Add(grid_sizer, 0, wxEXPAND);

	sizer->Add(other_container, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	sizer->Add(grid_sizer_container, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	sizer->Add(apply_button, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

	SetSizerAndFit(sizer);

	m_bias_input->SetMin(0);
	m_bias_input->SetMax(255);
	apply_button->Bind(wxEVT_BUTTON, &KernelDialog::OnApplyKernelClicked, this);
}

void KernelDialog::SetKernel(const Kernel& kernel) {
	m_bias_input->SetValue(kernel.shift);
	m_convert_to_grey_input->SetValue(false);
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			m_weight_inputs[r][c]->SetValue(kernel.weights[r][c]);
		}
	}
}

Kernel KernelDialog::GetKernel() {
	Kernel kernel;

	kernel.shift = m_bias_input->GetValue();
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			kernel.weights[r][c] = m_weight_inputs[r][c]->GetValue();
		}
	}

	return kernel;
}

void KernelDialog::OnApplyKernelClicked(wxEvent & evt) {
	if (m_convert_to_grey_input->GetValue()) {
		m_parent->OnGreyButtonClicked(evt);
	}
	m_parent->OnApplyConv(GetKernel());
}
