#include "Accel_ImagePanel.h"
#include "wx\window.h"
#include "wx\dcbuffer.h"

Accel_ImagePanel::Accel_ImagePanel(wxFrame* parent)
{
  ImagePanel = NULL;

  Create(parent, 0, 0,  parent->GetSize().GetX(), parent->GetSize().GetY());
  //SetTitle("Image1");

  //Prevent Draw Flickering
#ifdef _WINDOWS
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  SetDoubleBuffered(true);
#endif
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
  for(unsigned layer = 0; layer < Layers.size(); layer++)
  {
    Layers[layer].Image->Destroy();
    delete Layers[layer].Image;
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
    ImagePanel->SetSize(img_width, img_height);
    ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - img_width) / 2, (parent_->GetSize().GetHeight() - img_height - 64) / 2));
	  
    wxBufferedPaintDC dc(ImagePanel);
    for(int y = 0; y < ImagePanel->GetSize().GetHeight(); y += 128)
      for(int x = 0; x < ImagePanel->GetSize().GetWidth(); x += 128)
        dc.DrawBitmap(Background,x,y, true);
    for(unsigned layer = 0; layer < Layers.size(); layer++)
      if(Layers[layer].Enabled)
        dc.DrawBitmap(*Layers[layer].Image, 0, 0, true);
  }
}
void Accel_ImagePanel::LoadFile(const wxString& name, bool new_layer)
{  
  //Update Fix:
  //quites iCCP warning for PNG files. Temporary fix that quites ALL warnings.
  wxLogNull Nolog;

  if(Layers.size() > 0 && !new_layer)
  {
    for(unsigned layer = 1; layer < Layers.size(); layer++)
    {
      Layers[layer].Image->Destroy();
      delete Layers[layer].Image;
    }
    Layers.resize(1);
  }
  else
  {
    Layer new_lay;
    new_lay.Image = new wxImage();
    new_lay.Enabled = true;
    Layers.push_back(new_lay);
  }
  Layers[Layers.size() - 1].Enabled = true;
  Layers[Layers.size() - 1].Image->LoadFile(name);

  if(!new_layer)
  {
    wxImage* ImagePtr = Layers[0].Image;

    img_width = ImagePtr->GetWidth() + BORDER_SIZE;
    img_height = ImagePtr->GetHeight() + BORDER_SIZE;

    ImagePanel->SetSize(img_width, img_height);
    ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - img_width) / 2, 
                            (parent_->GetSize().GetHeight() - img_height - 64) / 2));
  }
}