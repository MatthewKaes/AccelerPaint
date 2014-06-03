#ifndef ACCEL_IMAGE_PANEL
#define ACCEL_IMAGE_PANEL

#include "wx\wx.h"
#include <vector>

struct Layer
{
  wxImage* Image;
  bool Enabled;
};

class Accel_ImagePanel : public wxFrame {
public:
  Accel_ImagePanel(wxWindow* parent, wxScrollBar* hscroll, wxScrollBar* vscroll);
  virtual ~Accel_ImagePanel() {};

  void Create(wxWindow* parent, int x, int y, int width, int height);
  void Update(wxPaintEvent& event);
  void Remove();
  void Refresh();
  void LoadFile(const wxString& name, bool new_layer = false);
  void CheckVisability(int index, bool state);
  wxBitmap Render();
  unsigned LayerCount();
  std::vector<Layer>& GetLayers();

private:
  long draw_id;
  int img_width, img_height;
  wxWindow* parent_;
  wxScrollBar* hscroll_;
  wxScrollBar* vscroll_;
  int id;
  wxImage Background;
  std::vector<Layer> Layers;
  wxPanel* ImagePanel;
};

#endif