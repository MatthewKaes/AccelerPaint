#include "AccelerPaint.h"
#include "wx/wx.h"
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/string.h>
 #include <sstream>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


BEGIN_EVENT_TABLE(AccelerPaint,wxFrame)
    //(*EventTable(IFXFrame)
    //*)
END_EVENT_TABLE()


AccelerPaint::AccelerPaint(wxWindow* parent,wxWindowID id)
{
  //Build the GUI
  Create_GUI(parent,id);
  SetTitle("AccelerPaint");
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
}