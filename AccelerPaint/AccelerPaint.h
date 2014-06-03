#ifndef MY_FRAME_H
#define MY_FRAME_H

#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/checklst.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/checklst.h>

#include <wx/image.h>
#include "CL_Device.h"
#include "Accel_ImagePanel.h"
#include <vector>
#include <string>

#define TOOLFRAME_WIDTH 40
#define TOOLFRAME_HEIGHT 400
#define LAYERFRAME_WIDTH 220
#define LAYERFRAME_HEIGHT 330
#define SCROLL_BORDER_SIZE 7
#define SCROLL_SIZE_WIDTH 18

#define BACKGROUND_COLOR wxColour(100,100,100)

class AccelerPaint : public wxFrame
{
  public:
    AccelerPaint(wxWindow* parent,wxWindowID id = -1);
    virtual ~AccelerPaint() {if(opencl_img){opencl_img->Remove(); delete opencl_img;}};

    static const unsigned Program_Min_Width = 800;
    static const unsigned Program_Min_Height = 650;

  private:
    void Build_Opencl();
    void Create_GUI(wxWindow* parent, wxWindowID id);
    void Create_GUI_MenuStrip(wxWindow* parent, wxWindowID id);
    void Create_GUI_Tools(wxWindow* parent, wxWindowID id);
    void Create_GUI_Layers(wxWindow* parent, wxWindowID id);
    void Create_GUI_ImagePanel(wxWindow* parent, wxWindowID id);
    void ResizeWindow(wxSizeEvent& event);
    void OpenFile(wxCommandEvent& event);
    void OpenLayer(wxCommandEvent& event);
    void SaveRender(wxCommandEvent& event);
    void LayerChecked(wxCommandEvent& event);
    void ImageBackground(wxPaintEvent& event);
    void ImageScroll(wxScrollEvent& event);
    
    wxPanel* layerframe;
    wxPanel* toolspanel;
    wxPanel* imagepanel;
    wxScrollBar* imagesizeh;
    wxScrollBar* imagesizev;
    wxMenuBar* menustrip;
    wxMenu* filemenu;
    wxCheckListBox* layersinfo;

    Accel_ImagePanel* opencl_img;
    OpenCL_Dev device;

    
    static const long ID_OpenItem;
    static const long ID_OpenLItem;
    static const long ID_SaveItem;
    static const long ID_Layer;
    static const long ID_HScroll;
    static const long ID_VScroll;

    

    DECLARE_EVENT_TABLE()
};

#endif