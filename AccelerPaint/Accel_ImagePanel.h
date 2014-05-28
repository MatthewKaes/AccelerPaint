#ifndef ACCEL_IMAGE_PANEL
#define ACCEL_IMAGE_PANEL

#include "wx\wx.h"
#include <vector>

#define BORDER_SIZE 14

class Accel_ImagePanel : public wxFrame {
public:
  Accel_ImagePanel(wxFrame* parent);
  virtual ~Accel_ImagePanel() {};

  void Create(wxFrame* parent, int x, int y, int width, int height);
  void Update(wxPaintEvent& event);
  void Remove();
  void Refresh();
  void LoadFile(const wxString& name);
private:

  long draw_id;
  wxFrame* parent_;
  int id;
  wxImage Background;
  std::vector<wxImage*> Layers;
  wxPanel* ImagePanel;
};

#endif