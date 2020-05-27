#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "gui.h"

#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

// Внимание: санитайзеры говорят, что есть утечки. Valgrind тоже говорит, что утечки есть.
// Но по этой ссылке говорят, что это бывает даже если запускать минимальный пример кода.
// https://wiki.wxwidgets.org/Valgrind_Suppression_File_Howto
// Насколько я знаю, объекты wxWidgets удаляют за собой детей. Тогда надо в конструкторе сразу
// после создания wxBoxSizer на куче, вызывать SetSizer, чтобы при исключении память не утекла.
// Пришлось пока отключить санитайзеры. При этом отключил и для тестов, потому что не могу отключить
// только для этой программы. В будущем можно будет не использовать санитайзеры для графического
// интерфейса, а запускать его с valgrind и файлом с заглушаемыми ошибками.

using namespace diags::gui;

MainWindow::MainWindow()
	: wxFrame(nullptr, wxID_ANY, "CppDiagHelper") {
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);

	diagList = new wxListCtrl(this, wxID_ANY);
	diagList->SetWindowStyleFlag(wxLC_SINGLE_SEL | wxLC_REPORT);
	diagList->InsertColumn(0, wxString::FromUTF8("Текст"));
	diagList->InsertItem(0, wxString::FromUTF8("Элемент"));
	sizer->Add(diagList, 4, wxEXPAND | wxALL, 0);

	// detailedViewNotebook = new wxNotebook(this, wxID_ANY);
	// sizer->Add(detailedViewNotebook, 3, wxEXPAND | wxALL, 0);
}


wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainWindow* mainWindow = new MainWindow();
	mainWindow->Show(true);
	return true;
}



