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
const long AccelerPaint::ID_OpenLItem = wxNewId();

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
  
  //Help prevent flickering on windows.
#ifdef _WINDOWS
  SetDoubleBuffered(true);
#endif
}

void AccelerPaint::Create_GUI(wxWindow* parent, wxWindowID id)
{
  //Components
  wxMenuItem* menu_items;

  //Set up the Main application window
  wxPoint Selection_Point(15,0);
  unsigned Selection_Width = 210;
  unsigned Selection_Height = 26;
  unsigned Spacing = Selection_Height + 8;

  Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxNO_BORDER, _T("wxID_ANY"));
  
  wxSize minimum_size(370 * 2 + 18 + Selection_Width + Selection_Point.x * 2, 370 * 2 + 70);
  SetMinSize(minimum_size);
  this->SetSize(minimum_size);

  //Construct File Menu
  menustrip = new wxMenuBar();
  filemenu = new wxMenu();
  menu_items = new wxMenuItem(filemenu, ID_OpenItem, _("Open"), wxEmptyString, wxITEM_NORMAL);
  filemenu->Append(menu_items);
  menu_items = new wxMenuItem(filemenu, ID_OpenLItem, _("Open Layer"), wxEmptyString, wxITEM_NORMAL);
  filemenu->Append(menu_items);
  menustrip->Append(filemenu, _("File"));
  
  //Set the Menu bar
  SetMenuBar(menustrip);

  //Connect Events for the program
  Connect(ID_OpenItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenFile);
  Connect(ID_OpenLItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenLayer);
}

void AccelerPaint::OpenFile(wxCommandEvent& event)
{
  wxFileDialog  dlg( this, _T("Select a supported Image file"), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
    if(img == NULL)
    {
      img = new Accel_ImagePanel(this);
    }
    img->LoadFile(dlg.GetPath());
    img->Refresh();
  }
}
void AccelerPaint::OpenLayer(wxCommandEvent& event)
{
  wxFileDialog  dlg( this, _T("Select a supported Image file"), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
    //Cannot open a layer if you don't have a file open yet.
    if(img == NULL)
    {
      return;
    }
    img->LoadFile(dlg.GetPath(), true);
    img->Refresh();
  }
}