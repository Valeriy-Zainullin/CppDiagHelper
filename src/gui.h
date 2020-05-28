#ifndef GUI_H
#define GUI_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/dnd.h>

#include "parser.h"
#include <vector>

namespace diags::gui {
	void warn(const char* text, wxWindow* parent);
	void notifyAboutError(const char* text, wxWindow* parent);

	template<typename F>
	class FileDropTarget : public wxFileDropTarget {
	public:
		FileDropTarget(F&& callableParam)
			: callable(std::move(callableParam)) {}
		virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& fileNames) override {
			((void) x);
			((void) y);
			return callable(fileNames);
		}
	private:
		F callable;
	};

	class DetailedViewPage : public wxPanel {
	public:
		DetailedViewPage(wxWindow* parent);
		void setText(const std::string& text);
		void clear();
	private:
		wxTextCtrl* contents;
	};

	class MainWindow : public wxFrame {
	public:
		MainWindow();
	private:
		void onFileDropped(const wxString& fileName);
		void onDiagSelected(const wxCommandEvent& event);
	private:
		wxListCtrl* diagList;
		wxNotebook* detailedViewNotebook;
		DetailedViewPage* detailsPage;
		DetailedViewPage* notesPage;
		DetailedViewPage* hintsPage;

		std::vector<diags::parsing::Diagnostic> diagnostics;
	};

	class App : public wxApp {
	public:
		virtual bool OnInit() override;
	};

}

#endif
