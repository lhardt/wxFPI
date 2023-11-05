#include <iostream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MainApp : public wxApp { };


int main() {
	std::cout << "Hello, world!\n";
	return 0;
}


wxIMPLEMENT_APP(MainApp);
