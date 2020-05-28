#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "gui.h"

#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/stattext.h>
#include <wx/file.h>
#include <wx/msgdlg.h>

#include "parser.h"
#include <fstream>

// Внимание: санитайзеры говорят, что есть утечки. Valgrind тоже говорит, что утечки есть.
// Но по этой ссылке говорят, что это бывает даже если запускать минимальный пример кода.
// https://wiki.wxwidgets.org/Valgrind_Suppression_File_Howto
// Насколько я знаю, объекты wxWidgets удаляют за собой детей. Тогда надо в конструкторе сразу
// после создания wxBoxSizer на куче, вызывать SetSizer, чтобы при исключении память не утекла.
// Где-то прочитал, что главные окна (создаваемые без родителей, "top level windows")
// тоже сами удаляются. В примерах кода тоже их не удаляют.
// Пришлось пока отключить санитайзеры. При этом отключил и для тестов, потому что не могу отключить
// только для этой программы. В будущем можно будет не использовать санитайзеры для графического
// интерфейса, а запускать его с valgrind и файлом с заглушаемыми ошибками.

using namespace diags::gui;

void diags::gui::warn(const char* text, wxWindow* parent) {
	wxMessageBox(
		wxString::FromUTF8(text),
		wxString::FromUTF8("Предупреждение"),
		wxICON_EXCLAMATION,
		parent
	);
}

void diags::gui::notifyAboutError(const char* text, wxWindow* parent) {
	wxMessageBox(
		wxString::FromUTF8(text),
		wxString::FromUTF8("Ошибка"),
		wxICON_ERROR,
		parent
	);
}


DetailedViewPage::DetailedViewPage(wxWindow* parent)
	: wxPanel(parent) {
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	contents = new wxTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY
	);
	sizer->Add(contents, 1, wxEXPAND | wxALL, 0);
}

void DetailedViewPage::setText(const std::string& text) {
	contents->SetValue(wxString::FromUTF8(text.c_str()));
}

void DetailedViewPage::clear() {
	contents->SetValue(wxEmptyString);
}


MainWindow::MainWindow()
	: wxFrame(nullptr, wxID_ANY, "CppDiagHelper") {
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	diagList = new wxListCtrl(this, wxID_ANY);
	sizer->Add(diagList, 4, wxEXPAND | wxALL, 0);
	diagList->SetWindowStyleFlag(wxLC_SINGLE_SEL | wxLC_REPORT);
	diagList->InsertColumn(0, wxString::FromUTF8("Строка"));
	diagList->InsertColumn(1, wxString::FromUTF8("Тип"));
	diagList->InsertColumn(2, wxString::FromUTF8("Описание"));
	diagList->Bind(wxEVT_LIST_ITEM_SELECTED, &MainWindow::onDiagSelected, this);

	detailedViewNotebook = new wxNotebook(this, wxID_ANY);
	sizer->Add(detailedViewNotebook, 3, wxEXPAND | wxALL, 0);

	detailsPage = new DetailedViewPage(detailedViewNotebook);
	detailedViewNotebook->AddPage(detailsPage, wxString::FromUTF8("Подробно"));

	notesPage = new DetailedViewPage(detailedViewNotebook);
	detailedViewNotebook->AddPage(notesPage, wxString::FromUTF8("Замечания"));

	hintsPage = new DetailedViewPage(detailedViewNotebook);
	detailedViewNotebook->AddPage(hintsPage, wxString::FromUTF8("Подсказки"));

	SetDropTarget(new FileDropTarget([this](const wxArrayString& fileNames) {
		if (fileNames.GetCount() != 1) {
			return false;
		}
		this->onFileDropped(fileNames[0]);
		return true;
	}));
}

void MainWindow::onFileDropped(const wxString& fileName) {
	std::ifstream inputStream(fileName.ToUTF8());
	if (inputStream.fail()) {
		warn("Не удалось открыть файл!", this);
		return;
	}

	if (!diagList->DeleteAllItems()) {
		notifyAboutError("Не удалось очистить список диагностик!", this);
		wxAbort();
	}
	detailsPage->clear();
	notesPage->clear();
	hintsPage->clear();
	detailedViewNotebook->SetSelection(0);

	try {
		diagnostics = diags::parsing::Parser(inputStream).parse();
	} catch (std::exception& exc) {
		warn(exc.what(), this);
		return;
	}
	// Количество элементов должно помещаться в int!
	for (const diags::parsing::Diagnostic& diagnostic: diagnostics) {
		long index = diagList->InsertItem(
			diagList->GetItemCount(),
			wxString::FromAscii(std::to_string(diagnostic.lineNum).c_str())
		); // Может ли вернуть что-то, что меньше нуля? Есть перегрузка, возвращающая -1 при ошибке.
		if (diagnostic.diagType == diags::parsing::DiagnosticType::ERROR) {
			diagList->SetItem(index, 1, wxString::FromUTF8("Ошибка"));
		} else if (diagnostic.diagType == diags::parsing::DiagnosticType::WARNING) {
			diagList->SetItem(index, 1, wxString::FromUTF8("Предупреждение"));
		} else throw;
		diagList->SetItem(index, 2, wxString::FromUTF8(diagnostic.description.c_str()));
	}
}

void MainWindow::onDiagSelected(const wxCommandEvent& event) {
	((void) event);
	long index = diagList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index == -1) {
		notifyAboutError("Не удалось найти выделенный элемент!", this);
		wxAbort();
	}
	detailsPage->setText(diagnostics[index].detailedDescription);
	notesPage->setText(diagnostics[index].notes);
	// hintsPage->setText(wxString::FromUTF8(diagnostics[index].hints.c_str());
}


wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainWindow* mainWindow = new MainWindow();
	mainWindow->Show(true);
	return true;
}



