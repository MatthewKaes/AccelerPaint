#ifndef ACCEL_IMAGE_PANEL
#define ACCEL_IMAGE_PANEL

#pragma warning(disable : 4996)
#include "CL_Device.h"
#include "wx\wx.h"

#include <vector>

struct Layer
{
  wxImage* Image;
  bool Enabled;
  float Opacity;
};

class Accel_ImagePanel : public wxFrame {
public:
  Accel_ImagePanel(wxWindow* parent, wxScrollBar* hscroll, wxScrollBar* vscroll);
  virtual ~Accel_ImagePanel() {};

  void Create(wxWindow* parent, int x, int y, int width, int height);
  void Update(wxPaintEvent& event);
  void Remove();
  void Refresh();
  void RenderImage();
  void LoadFile(const wxString& name, bool new_layer = false);
  void LoadFile(int width, int height, unsigned char* data, unsigned char* alpha, bool new_layer = false);

  //Filters
  void Invert(unsigned layer);
  void Blur(unsigned layer);

  //Setters
  void CheckVisability(int index, bool state);
  void SetOpacity(int index, float opacity);

  //Getters
  unsigned LayerCount();
  unsigned char* GetRGBChannel(unsigned layer);
  unsigned char* GetAlphaChannel(unsigned layer);
  bool GetVisability(unsigned layer);
  float GetOpacity(unsigned layer);
  unsigned GetCanvasWidth();
  unsigned GetCanvasHeight();
  wxImage* GetRender();
  std::vector<Layer>* GetLayers();
  void Empty();

private:
  enum Repaint_States { NO_REPAINT, FULL_REPAINT };

  int id;
  long draw_id;
  int img_width, img_height;
  int scroll_size;
  wxImage Render;
  wxWindow* parent_;
  wxScrollBar* hscroll_;
  wxScrollBar* vscroll_;
  wxImage Background;
  std::vector<Layer> Layers;
  wxPanel* ImagePanel;
  OpenCL_Dev device;
  Repaint_States need_paint;
};

#endif