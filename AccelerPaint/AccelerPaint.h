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
#include <wx/colordlg.h>
#include <wx/spinctrl.h>

#include <wx/image.h>
#include "Accel_ImagePanel.h"
#include <vector>
#include <string>

#define COLOR_DEPTH 3
#define TOOLFRAME_WIDTH 30
#define TOOLFRAME_HEIGHT 400
#define TOOLFRAME_BUFFER 4
#define TOOLFRAME_SEPERATOR 5
#define LAYERFRAME_WIDTH 220
#define LAYERFRAME_HEIGHT 330
#define SCROLL_BORDER_SIZE 7
#define SCROLL_SIZE_WIDTH 18
#define ACCELER_SIGNITURE 0xAC00E521
#define ACCELER_VERSION 1

#define BACKGROUND_COLOR wxColour(100,100,100)

class AccelerPaint : public wxFrame
{
  public:
    AccelerPaint(wxWindow* parent,wxWindowID id = -1);
    virtual ~AccelerPaint() {if(opencl_img){opencl_img->Remove(); delete opencl_img;}};

    static const unsigned Program_Min_Width = 800;
    static const unsigned Program_Min_Height = 650;

  private:
    typedef struct Acceler_Data
    {
	    unsigned signature;
      unsigned version;
      unsigned i_width;
	    unsigned i_height;
	    unsigned layer_count;
    } Acceler_Data;

    typedef struct Layer_Data
    {
	    bool visible;
	    float opacity;
    } Layer_Data;

    void Create_GUI(wxWindow* parent, wxWindowID id);
    void Create_GUI_MenuStrip(wxWindow* parent, wxWindowID id);
    void Create_GUI_Layers(wxWindow* parent, wxWindowID id);
    void Create_GUI_ImagePanel(wxWindow* parent, wxWindowID id);
    void Create_GUI_Tools(wxWindow* parent, wxWindowID id);
    void Create_GUI_Tools_Color(wxWindow* parent, unsigned toolindex);
    void Create_GUI_Tool_Generic(wxWindow* parent, unsigned toolindex, const char* img_path, const char* tooltip);
    void Create_GUI_Tool_Sperator(wxWindow* parent, unsigned toolindex);

    // Events
    void ResizeWindow(wxSizeEvent& event);
    void OpenFile(wxCommandEvent& event);
    void OpenLayer(wxCommandEvent& event);
    void SaveRender(wxCommandEvent& event);
    void LayerChecked(wxCommandEvent& event);
    void LayerChanged(wxCommandEvent& event);
    void ImageBackground(wxPaintEvent& event);
    void ImageScroll(wxScrollEvent& event);
    void ColorPicker(wxCommandEvent& event);
    void ColorSelected(wxCommandEvent& event);
    void ToolSelected(wxCommandEvent& event);
    void OpacityChanged(wxSpinEvent& event);
    void ClickEvent(wxMouseEvent& event);

    void DuplicateLayer(wxCommandEvent& event);
    void RedChannel(wxCommandEvent& event);
    void GreenChannel(wxCommandEvent& event);
    void BlueChannel(wxCommandEvent& event);

    void InvertLayer(wxCommandEvent& event);
    void BlurLayer(wxCommandEvent& event);
    void SobelLayer(wxCommandEvent& event);
    void ThresholdLayer(wxCommandEvent& event);


    void Toolsupdate(int tool);
    int LayerSelected();

    
    void BucketFill(wxMouseEvent& event);

    wxPanel* layerframe;
    wxPanel* toolspanel;
    wxPanel* imagepanel;
    wxScrollBar* imagesizeh;
    wxScrollBar* imagesizev;
    wxSpinCtrl* opacityctrl;
    wxMenuBar* menustrip;
    wxMenu* filemenu;
    wxCheckListBox* layersinfo;
    wxButton *ColorButton;
    
    //Channels
    wxMenuItem* redchannel;
    wxMenuItem* greenchannel;
    wxMenuItem* bluechannel;

    std::vector<wxBitmapButton*> toolbuttons;

    wxColour pickedcolor;
    Accel_ImagePanel* opencl_img;
    
    unsigned seperators;
    unsigned selected_tool;
    static unsigned FILL_ID;
    static unsigned EYEDROP_ID;

    //Event ids
    static const long ID_ColorUpdate;
    static const long ID_OpenItem;
    static const long ID_OpenLItem;
    static const long ID_SaveItem;
    static const long ID_Layer;
    static const long ID_HScroll;
    static const long ID_VScroll;

    DECLARE_EVENT_TABLE()
};

#endif