#include "AccelerPaint.h"
#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/scrolbar.h"
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/string.h>
 #include <sstream>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

const long AccelerPaint::ID_OpenItem = wxNewId();
const long AccelerPaint::ID_OpenLItem = wxNewId();
const long AccelerPaint::ID_SaveItem = wxNewId();
const long AccelerPaint::ID_Layer = wxNewId();
const long AccelerPaint::ID_HScroll = wxNewId();
const long AccelerPaint::ID_VScroll = wxNewId();

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
  SetSize(minimum_size);

  //Construct File Menu
  menustrip = new wxMenuBar();
  filemenu = new wxMenu();
  menu_items = new wxMenuItem(filemenu, ID_OpenItem, _("Open"), wxEmptyString, wxITEM_NORMAL);
  filemenu->Append(menu_items);
  menu_items = new wxMenuItem(filemenu, ID_OpenLItem, _("Open Layer"), wxEmptyString, wxITEM_NORMAL);
  filemenu->Append(menu_items);
  menu_items = new wxMenuItem(filemenu, ID_SaveItem, _("Save"), wxEmptyString, wxITEM_NORMAL);
  filemenu->Append(menu_items);
  menustrip->Append(filemenu, _("File"));

  //Construct SideFrame
  toolspanel = new wxPanel(this, wxNewId(), wxPoint(0,0), wxSize(TOOLFRAME_WIDTH,TOOLFRAME_HEIGHT), wxRAISED_BORDER);

  //Construct Layers
  layerframe = new wxPanel(this, wxNewId(), wxPoint(0,0), wxSize(LAYERFRAME_WIDTH + 7,LAYERFRAME_HEIGHT + 7), wxRAISED_BORDER);
  layersinfo = new wxCheckListBox(layerframe, ID_Layer);
  layersinfo->SetSize(LAYERFRAME_WIDTH,LAYERFRAME_HEIGHT);
  
  //Set the Menu bar
  SetMenuBar(menustrip);

  //Create The actual work area
  imagepanel = new wxPanel(this, wxNewId(), wxPoint(toolspanel->GetSize().GetWidth(),0), 
                           wxSize(LAYERFRAME_WIDTH + 7,LAYERFRAME_HEIGHT + 7));
  imagepanel->SetSize(this->GetSize().GetWidth() - LAYERFRAME_WIDTH - 7 - TOOLFRAME_WIDTH, 
               this->GetSize().GetHeight() - LAYERFRAME_HEIGHT - 7 - TOOLFRAME_HEIGHT);
  imagepanel->SetBackgroundStyle(wxBG_STYLE_PAINT);

  imagesizeh = new wxScrollBar(imagepanel, ID_HScroll, wxPoint(0, 0), wxSize(imagepanel->GetSize().GetWidth(), 18));
  imagesizeh->SetScrollbar(250, 10, 500, 0);
  imagesizeh->Enable(false);

  imagesizev = new wxScrollBar(imagepanel, ID_VScroll, wxPoint(0, 0), wxSize(18, imagepanel->GetSize().GetHeight()), wxVERTICAL);
  imagesizev->SetScrollbar(250, 10, 500, 0);
  imagesizev->Enable(false);

  img = new Accel_ImagePanel(imagepanel, imagesizeh, imagesizev);


  //Connect Events for the program
  Connect(ID_OpenItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenFile);
  Connect(ID_OpenLItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenLayer);
  Connect(ID_SaveItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::SaveRender);
  Connect(ID_Layer, wxEVT_CHECKLISTBOX,(wxObjectEventFunction)&AccelerPaint::LayerChecked);

  //Connect part events  
  imagepanel->Connect(wxEVT_PAINT,(wxObjectEventFunction)&AccelerPaint::ImageBackground);
  Connect(ID_HScroll, wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&AccelerPaint::ImageScroll);
  Connect(ID_VScroll, wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&AccelerPaint::ImageScroll);

  //Connect Window Events
  Connect(wxEVT_SIZE, (wxObjectEventFunction)&AccelerPaint::ResizeWindow);
}
void AccelerPaint::ResizeWindow(wxSizeEvent& event)
{
  //Reset Layer Frame position
  layerframe->SetPosition(wxPoint(GetSize().GetWidth() - LAYERFRAME_WIDTH - 21, GetSize().GetHeight() - LAYERFRAME_HEIGHT - 64));
  imagepanel->SetSize(this->GetSize().GetWidth() - LAYERFRAME_WIDTH - 21 - TOOLFRAME_WIDTH, this->GetSize().GetHeight() - 58);
  
  imagesizev->SetPosition(wxPoint(imagepanel->GetSize().GetWidth() - 18, 0));
  imagesizev->SetSize(18 , imagepanel->GetSize().GetHeight() - 18);

  imagesizeh->SetPosition(wxPoint(0, imagepanel->GetSize().GetHeight() - 18));
  imagesizeh->SetSize(imagepanel->GetSize().GetWidth() - 18, 18);
}
void AccelerPaint::OpenFile(wxCommandEvent& event)
{
  wxFileDialog  dlg( this, _T("Select a supported Image file"), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
    img->LoadFile(dlg.GetPath());
    img->Refresh();

    layersinfo->Clear();
    layersinfo->Insert(dlg.GetFilename(), 0);
    layersinfo->Check(0);
  }
}
void AccelerPaint::OpenLayer(wxCommandEvent& event)
{
  //Return if an image isn't loaded yet
  if(img->LayerCount() == 0)
  {
    return;
  }
  wxFileDialog  dlg( this, _T("Select a supported Image file"), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
    img->LoadFile(dlg.GetPath(), true);
    img->Refresh();
    
    layersinfo->Insert(dlg.GetFilename(), layersinfo->GetCount());
    layersinfo->Check(layersinfo->GetCount() - 1);
  }
}
void AccelerPaint::SaveRender(wxCommandEvent& event)
{
  //Return if an image isn't loaded yet
  if(img->LayerCount() == 0)
  {
    return;
  }
  wxFileDialog  dlg( this, _T("Save As..."), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp"), wxFD_SAVE);

	if( dlg.ShowModal() == wxID_OK )
	{
    wxImage render(img->Render().ConvertToImage());
    render.SaveFile(dlg.GetPath());
  }
}
void AccelerPaint::LayerChecked(wxCommandEvent& event)
{
  img->CheckVisability(event.GetInt(), layersinfo->IsChecked(event.GetInt()));
  img->Refresh();
}
void AccelerPaint::ImageBackground(wxPaintEvent& event)
{
  wxBufferedPaintDC dc(this);

  // draw a rectangle
  dc.SetBrush(wxBrush(wxColour(100,100,100))); // blue filling
  dc.DrawRectangle( -1, -1, this->GetSize().GetWidth() + 2, this->GetSize().GetHeight() + 2);
}
void AccelerPaint::ImageScroll(wxScrollEvent& event)
{
  img->Refresh();
}