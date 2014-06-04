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

#ifdef _DEBUG
#define OPENCL_TEST
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
  opencl_img = NULL;
  Create_GUI(parent,id);
  SetTitle("AccelerPaint");
  
  //Build opencl
  Build_Opencl();

  //Help prevent flickering on windows.
#ifdef _WINDOWS
  SetDoubleBuffered(true);
#endif
}

void AccelerPaint::Build_Opencl()
{
  device.Build_Kernel("Fill_Shader");
}

void AccelerPaint::Create_GUI(wxWindow* parent, wxWindowID id)
{
  //Set up the Main application window
  wxPoint Selection_Point(15,0);

  Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxNO_BORDER, _T("wxID_ANY"));
  
  wxSize minimum_size(Program_Min_Width, Program_Min_Height);
  SetMinSize(minimum_size);
  SetSize(minimum_size);

  //Create subwindows
  Create_GUI_MenuStrip(parent, id);
  Create_GUI_Tools(parent, id);
  Create_GUI_Layers(parent, id);
  Create_GUI_ImagePanel(parent, id);

  //Connect Window Events
  Connect(wxEVT_SIZE, (wxObjectEventFunction)&AccelerPaint::ResizeWindow);
}
void AccelerPaint::Create_GUI_MenuStrip(wxWindow* parent, wxWindowID id)
{
  //Components
  wxMenuItem* menu_items;

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
  
  //Set the Menu bar
  SetMenuBar(menustrip);

  //Connect Events for Menustrip
  Connect(ID_OpenItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenFile);
  Connect(ID_OpenLItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::OpenLayer);
  Connect(ID_SaveItem, wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&AccelerPaint::SaveRender);
}
void AccelerPaint::Create_GUI_Tools(wxWindow* parent, wxWindowID id)
{
  //Construct SideFrame
  toolspanel = new wxPanel(this, wxNewId(), wxPoint(0,0), wxSize(TOOLFRAME_WIDTH,TOOLFRAME_HEIGHT), wxRAISED_BORDER);
}
void AccelerPaint::Create_GUI_Layers(wxWindow* parent, wxWindowID id)
{
  //Construct Layers
  layerframe = new wxPanel(this, wxNewId(), wxPoint(0,0), wxSize(LAYERFRAME_WIDTH + 7,LAYERFRAME_HEIGHT + 7), wxRAISED_BORDER);
  layersinfo = new wxCheckListBox(layerframe, ID_Layer);
  layersinfo->SetSize(LAYERFRAME_WIDTH,LAYERFRAME_HEIGHT);

  //Connect Events for the layer checking
  Connect(ID_Layer, wxEVT_CHECKLISTBOX,(wxObjectEventFunction)&AccelerPaint::LayerChecked);

}
void AccelerPaint::Create_GUI_ImagePanel(wxWindow* parent, wxWindowID id)
{
  //Create The actual work area
  imagepanel = new wxPanel(this, wxNewId(), wxPoint(toolspanel->GetSize().GetWidth(),0), 
                           wxSize(LAYERFRAME_WIDTH + 7,LAYERFRAME_HEIGHT + SCROLL_BORDER_SIZE));
  imagepanel->SetSize(this->GetSize().GetWidth() - LAYERFRAME_WIDTH - SCROLL_BORDER_SIZE - TOOLFRAME_WIDTH, 
               this->GetSize().GetHeight() - LAYERFRAME_HEIGHT - SCROLL_BORDER_SIZE - TOOLFRAME_HEIGHT);
  imagepanel->SetBackgroundStyle(wxBG_STYLE_PAINT);

  //Scroll bars for image
  imagesizeh = new wxScrollBar(imagepanel, ID_HScroll, wxPoint(0, 0), wxSize(imagepanel->GetSize().GetWidth(), 18));
  imagesizeh->Enable(false);

  imagesizev = new wxScrollBar(imagepanel, ID_VScroll, wxPoint(0, 0), wxSize(18, imagepanel->GetSize().GetHeight()), wxVERTICAL);
  imagesizev->Enable(false);

  //Create the new ImagePanel to handel OpenCL drawing
  //This is the component that does all the work.
  opencl_img = new Accel_ImagePanel(imagepanel, imagesizeh, imagesizev);

  //Connect part events  
  imagepanel->Connect(wxEVT_PAINT,(wxObjectEventFunction)&AccelerPaint::ImageBackground);
  Connect(ID_HScroll, wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&AccelerPaint::ImageScroll);
  Connect(ID_VScroll, wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&AccelerPaint::ImageScroll);

}
void AccelerPaint::ResizeWindow(wxSizeEvent& event)
{
  //Reset Layer Frame position
  layerframe->SetPosition(wxPoint(GetSize().GetWidth() - LAYERFRAME_WIDTH - 21, GetSize().GetHeight() - LAYERFRAME_HEIGHT - 64));
  imagepanel->SetSize(this->GetSize().GetWidth() - LAYERFRAME_WIDTH - 21 - TOOLFRAME_WIDTH, this->GetSize().GetHeight() - 58);
  
  imagesizev->SetPosition(wxPoint(imagepanel->GetSize().GetWidth() - SCROLL_SIZE_WIDTH, 0));
  imagesizev->SetSize(SCROLL_SIZE_WIDTH, imagepanel->GetSize().GetHeight() - SCROLL_SIZE_WIDTH);

  imagesizeh->SetPosition(wxPoint(0, imagepanel->GetSize().GetHeight() - SCROLL_SIZE_WIDTH));
  imagesizeh->SetSize(imagepanel->GetSize().GetWidth() - SCROLL_SIZE_WIDTH, SCROLL_SIZE_WIDTH);
}
void AccelerPaint::OpenFile(wxCommandEvent& event)
{
  //Simply call load dialog
  wxFileDialog  dlg( this, _T("Select a supported Image file"), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
    opencl_img->LoadFile(dlg.GetPath());
    opencl_img->Refresh();
    
    //test code
#ifdef OPENCL_TEST
    image img_dat;
    img_dat.rgb_data = opencl_img->GetRGBChannel(0);
    img_dat.alpha_data = opencl_img->GetAlphaChannel(0);
    img_dat.pos_data.width = opencl_img->GetCanvasWidth();
    img_dat.pos_data.height = opencl_img->GetCanvasHeight();
    color fill_c;
    fill_c.Alpha = 155;
    fill_c.Blue = 255;
    fill_c.Green = 0;
    fill_c.Red = 255;
    rect fill_r;
    fill_r.width = fill_r.height = 100;
    fill_r.x = fill_r.y = 0;
    device.Fill(img_dat, fill_r, fill_c);
#endif

    layersinfo->Clear();
    layersinfo->Insert(dlg.GetFilename(), 0);
    layersinfo->Check(0);
  }
}
void AccelerPaint::OpenLayer(wxCommandEvent& event)
{
  //Return if an image isn't loaded yet
  if(opencl_img->LayerCount() == 0)
  {
    return;
  }
  //Simply call load dialog
  wxFileDialog  dlg( this, _T("Select a supported Image file"), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( dlg.ShowModal() == wxID_OK )
	{
    opencl_img->LoadFile(dlg.GetPath(), true);
    opencl_img->Refresh();
    
    layersinfo->Insert(dlg.GetFilename(), layersinfo->GetCount());
    layersinfo->Check(layersinfo->GetCount() - 1);
  }
}
void AccelerPaint::SaveRender(wxCommandEvent& event)
{
  //Return if an image isn't loaded yet
  if(opencl_img->LayerCount() == 0)
  {
    return;
  }
  //Simply call save dialog
  wxFileDialog  dlg( this, _T("Save As..."), wxEmptyString, wxEmptyString, _T("PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|BMP files (*.bmp)|*.bmp"), wxFD_SAVE);

	if( dlg.ShowModal() == wxID_OK )
	{
    wxImage render(opencl_img->Render().ConvertToImage());
    render.SaveFile(dlg.GetPath());
  }
}
void AccelerPaint::LayerChecked(wxCommandEvent& event)
{
  opencl_img->CheckVisability(event.GetInt(), layersinfo->IsChecked(event.GetInt()));
  opencl_img->Refresh();
}
void AccelerPaint::ImageBackground(wxPaintEvent& event)
{
  wxBufferedPaintDC dc(this);

  // draw a rectangle
  dc.SetBrush(wxBrush(BACKGROUND_COLOR));

  //Increase the rectangle size by 2 and recenter
  dc.DrawRectangle( -1, -1, this->GetSize().GetWidth() + 2, this->GetSize().GetHeight() + 2);
}
void AccelerPaint::ImageScroll(wxScrollEvent& event)
{
  opencl_img->Refresh();
}