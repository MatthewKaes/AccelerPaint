#ifndef MY_FRAME_H
#define MY_FRAME_H

#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/checklst.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>

#include <wx/image.h>
#include "CL_Device.h"
#include "Accel_ImagePanel.h"
#include <vector>
#include <string>

class AccelerPaint : public wxFrame
{
  public:
    AccelerPaint(wxWindow* parent,wxWindowID id = -1);
    virtual ~AccelerPaint() {};

  private:
    void Create_GUI(wxWindow* parent, wxWindowID id);
    void OpenFile(wxCommandEvent& event);

    wxMenuBar* menustrip;
    wxMenu* filemenu;
    wxMenuItem* openitem;

    Accel_ImagePanel* img;
    OpenCL_Dev device;

    
    static const long ID_OpenItem;

    DECLARE_EVENT_TABLE()
};

#endif