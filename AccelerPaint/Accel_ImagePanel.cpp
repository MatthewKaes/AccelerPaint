#include "Accel_ImagePanel.h"
#include "wx\window.h"

Accel_ImagePanel::Accel_ImagePanel(wxFrame* parent)
{
  ImagePanel = NULL;

  Create(parent, 0, 0,  parent->GetSize().GetX(), parent->GetSize().GetY());
  
  SetTitle("Image1");
}
void Accel_ImagePanel::Create(wxFrame* parent, int x, int y, int width, int height)
{
  parent_ = parent;
  id = wxNewId();
  draw_id = wxNewId();
  Background.LoadFile("./resources/Background.png");
  SetSize(width, height);
  ImagePanel = new wxPanel(parent, id, wxPoint(x, y), wxSize(width, height), wxFULL_REPAINT_ON_RESIZE|wxRAISED_BORDER|wxTAB_TRAVERSAL, _T("ID_ImagePanel"));
  ImagePanel->Connect(wxEVT_PAINT,(wxObjectEventFunction)&Accel_ImagePanel::Update, 0, this);
}
void Accel_ImagePanel::Remove()
{
  ImagePanel->Destroy();
  Background.Destroy();
  for(int layer = 0; layer < Layers.size(); layer++)
  {
    Layers[layer]->Destroy();
    delete Layers[layer];
  }
}
void Accel_ImagePanel::Refresh()
{
  ImagePanel->Refresh();
}
void Accel_ImagePanel::Update(wxPaintEvent& event)
{
  if(ImagePanel)
  {
	  wxPaintDC dc = wxPaintDC(ImagePanel);
    for(int y = 0; y < ImagePanel->GetSize().GetHeight(); y += 128)
      for(int x = 0; x < ImagePanel->GetSize().GetWidth(); x += 128)
        dc.DrawBitmap(Background,x,y, true);
    for(int layer = 0; layer < Layers.size(); layer++)
      dc.DrawBitmap(*Layers[layer], 0, 0, true);
  }
}
void Accel_ImagePanel::LoadFile(const wxString& name)
{  
  if(Layers.size() > 0)
  {
    for(int layer = 1; layer < Layers.size(); layer++)
    {
      Layers[layer]->Destroy();
      delete Layers[layer];
    }
  }
  else
  {
    Layers.push_back(new wxImage());
  }
  Layers[0]->LoadFile(name);
  ImagePanel->SetSize(Layers[0]->GetWidth() + BORDER_SIZE, Layers[0]->GetHeight() + BORDER_SIZE);
  ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - Layers[0]->GetWidth()) / 2, (parent_->GetSize().GetHeight() - Layers[0]->GetHeight() - 64) / 2));
}