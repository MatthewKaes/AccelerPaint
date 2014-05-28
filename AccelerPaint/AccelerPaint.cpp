#include "AccelerPaint.h"
#include "wx/wx.h"
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/string.h>
 #include <sstream>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

const long AccelerPaint::ID_OpenItem = wxNewId();

BEGIN_EVENT_TABLE(AccelerPaint,wxFrame)
    //(*EventTable(IFXFrame)
    //*)
END_EVENT_TABLE()


AccelerPaint::AccelerPaint(wxWindow* parent,wxWindowID id)
{
  //Build the GUI
  img = NULL;
  Create_GUI(parent,id);
  SetTitle("AccelerPaint");
  
#ifdef _WINDOWS
  SetDoubleBuffered(true);
#endif
}

void AccelerPaint::Create_GUI(wxWindow* parent, wxWindowID id)
{
  wxPoint Selection_Point(15,0);
  unsigned Selection_Width = 210;
  unsigned Selection_Height = 26;
  unsigned Spacing = Selection_Height + 8;

  Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxNO_BORDER, _T("wxID_ANY"));
  
  wxSize minimum_size(370 * 2 + 18 + Selection_Width + Selection_Point.x * 2, 370 * 2 + 70);
  SetMinSize(minimum_size);
  this->SetSize(minimum_size);

  menustrip = new wxMenuBar();
  filemenu = new wxMenu();
  openitem = new wxMenuItem(filemenu, ID_OpenItem, _("Open"), wxEmptyString, wxITEM_NORMAL);
  filemenu->Append(openitem);
  menustrip->Append(filemenu, _("File"));
  
  SetMenuBar(menustrip);

  //img.Create(this, 0,0,200,200);

  Connect(ID_OpenItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenFile);
}

void AccelerPaint::OpenFile(wxCommandEvent& event)
{
  wxFileDialog  dlg( this, _T("Select a PNG file"), wxEmptyString, wxEmptyString, _T("*.png"));

	if( dlg.ShowModal() == wxID_OK )
	{
    if(img != NULL)
    {
      img->Remove();
      delete img;
    }
    img = new Accel_ImagePanel(this);
    img->LoadFile(dlg.GetPath());
    img->Refresh();
  }
}