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

class AccelerPaint : public wxFrame
{
  public:
    AccelerPaint(wxWindow* parent,wxWindowID id = -1);
    virtual ~AccelerPaint() {if(img){img->Remove(); delete img;}};

  private:
    void Create_GUI(wxWindow* parent, wxWindowID id);
    void ResizeWindow(wxSizeEvent& event);
    void OpenFile(wxCommandEvent& event);
    void OpenLayer(wxCommandEvent& event);
    void LayerChecked(wxCommandEvent& event);
    void ImageBackground(wxPaintEvent& event);
    
    wxPanel* layerframe;
    wxPanel* toolspanel;
    wxPanel* imagepanel;
    wxMenuBar* menustrip;
    wxMenu* filemenu;
    wxCheckListBox* layersinfo;

    Accel_ImagePanel* img;
    OpenCL_Dev device;

    
    static const long ID_OpenItem;
    static const long ID_OpenLItem;
    static const long ID_Layer;

    DECLARE_EVENT_TABLE()
};

#endif