#ifndef ACCEL_IMAGE_PANEL
#define ACCEL_IMAGE_PANEL

#pragma warning(disable : 4996)
#include "CL_Device.h"
#include "wx\wx.h"

#include <vector>

#define FILL_TOLERANCE 18

struct Layer
{
  wxImage* Image;
  bool Enabled;
  float Opacity;
  channel Channels;
};
struct Pixel
{
  Pixel(int x_, int y_) : x(x_), y(y_){}
  int x;
  int y;
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
  void LoadFile(int width, int height, unsigned char* data, unsigned char* alpha, channel channels, bool new_layer = false);
  void Delete(unsigned layer);

  //Filters
  void Invert(unsigned layer);
  void Blur(unsigned layer);
  void Sobel(unsigned layer);
  void Threshold(unsigned layer);
  
  //Operations
  void BucketFill(int layer, unsigned x, unsigned y, unsigned r, unsigned g, unsigned b);

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

  wxPanel* ImagePanel;

private:
  enum Repaint_States { NO_REPAINT, FULL_REPAINT };
  
  image ImageData(unsigned layer);

  int id;
  long draw_id;
  int img_width, img_height;
  int x_off, y_off;
  int scroll_size;
  wxImage Render;
  wxWindow* parent_;
  wxScrollBar* hscroll_;
  wxScrollBar* vscroll_;
  wxImage Background;
  std::vector<Layer> Layers;
  OpenCL_Dev device;
  Repaint_States need_paint;
};

#endif