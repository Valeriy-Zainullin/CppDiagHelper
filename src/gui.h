#ifndef GUI_H
#define GUI_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <wx/notebook.h>

namespace diags::gui {
	class MainWindow : public wxFrame {
	public:
		MainWindow();
	private:
		wxListCtrl* diagList;
		wxNotebook* detailedViewNotebook;
	};

	class App : public wxApp {
	public:
		virtual bool OnInit() override;
	};

}

#endif
