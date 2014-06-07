#include "Accel_ImagePanel.h"
#include "wx\window.h"
#include "wx\dcbuffer.h"

Accel_ImagePanel::Accel_ImagePanel(wxWindow* parent, wxScrollBar* hscroll, wxScrollBar* vscroll)
{
  ImagePanel = NULL;

  hscroll_ = hscroll;
  vscroll_ = vscroll;
  scroll_size = hscroll->GetSize().GetHeight();

  Create(parent, 0, 0,  parent->GetSize().GetX() - scroll_size, parent->GetSize().GetY() - scroll_size);
}
void Accel_ImagePanel::Create(wxWindow* parent, int x, int y, int width, int height)
{
  parent_ = parent;
  id = wxNewId();
  draw_id = wxNewId();
  Background.LoadFile("./resources/Background.png");
  SetSize(width, height);
  ImagePanel = new wxPanel(parent, id, wxPoint(x, y), wxSize(width, height), wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL, _T("ID_ImagePanel"));
  ImagePanel->Connect(wxEVT_PAINT,(wxObjectEventFunction)&Accel_ImagePanel::Update, 0, this);
  ImagePanel->SetDoubleBuffered(true);
  ImagePanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
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
    int rend_width, rend_height;
    int x_off, y_off;

    //Update scroll width
    if(img_width < parent_->GetSize().GetWidth() - scroll_size)
    {
      rend_width = img_width;
      hscroll_->Enable(false);
      x_off = 0;
    }
    else
    {
      rend_width = parent_->GetSize().GetWidth() - scroll_size;
      if(!hscroll_->IsEnabled())
      {
        hscroll_->SetThumbPosition(0);
        hscroll_->Enable(true);
      }
      if(hscroll_->GetRange() != img_width - rend_width)
        hscroll_->SetScrollbar(hscroll_->GetThumbPosition(), hscroll_->GetThumbSize(), (img_width - rend_width), 10);
      x_off = hscroll_->GetThumbPosition();
    }

    //Update scroll height
    if(img_height < parent_->GetSize().GetHeight() - scroll_size)
    {
      rend_height = img_height;
      vscroll_->Enable(false);
      y_off = 0;
    }
    else
    {
      rend_height = parent_->GetSize().GetHeight() - scroll_size;
      if(!vscroll_->IsEnabled())
      {
        vscroll_->SetThumbPosition(0);
        vscroll_->Enable(true);
      }
      if(vscroll_->GetRange() != img_height - rend_height)
        vscroll_->SetScrollbar(vscroll_->GetThumbPosition(), vscroll_->GetThumbSize(), img_height - rend_height, 10);
      y_off = vscroll_->GetThumbPosition();
    }

    //Make the image the right size and position
    ImagePanel->SetSize(rend_width, rend_height);
    ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - rend_width - scroll_size) / 2, (parent_->GetSize().GetHeight() - rend_height - scroll_size) / 2));
	  
    //Render the image
    wxBufferedPaintDC dc(ImagePanel);
    for(int y = 0; y < ImagePanel->GetSize().GetHeight(); y += 128)
      for(int x = 0; x < ImagePanel->GetSize().GetWidth(); x += 128)
        dc.DrawBitmap(Background,x,y, true);
    for(unsigned layer = 0; layer < Layers.size(); layer++)
      if(Layers[layer].Enabled)
        dc.DrawBitmap(*Layers[layer].Image, -x_off, -y_off, true);
  }
}
void Accel_ImagePanel::LoadFile(const wxString& name, bool new_layer)
{  
  //Update Fix:
  //quites iCCP warning for PNG files. Temporary fix that quites ALL warnings.
  wxLogNull Nolog;

  //Load the image into the layer it belongs.
  //If it's not a new layer then wipe the layers
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

  //Set alpha channels for images that don't have them.
  if(!Layers[Layers.size() - 1].Image->GetAlpha())
  {
    Layers[Layers.size() - 1].Image->InitAlpha();
  }
  
  //If it's not a new layer then update the canvas size and position.
  if(!new_layer)
  {
    wxImage* ImagePtr = Layers[0].Image;

    img_width = ImagePtr->GetWidth();
    img_height = ImagePtr->GetHeight();

    ImagePanel->SetSize(img_width, img_height);
    ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - img_width - scroll_size) / 2, 
                            (parent_->GetSize().GetHeight() - img_height - scroll_size) / 2));
  }
  //Resize to the canvas if it is a new layer
  else
  {
    if(Layers[Layers.size() - 1].Image->GetSize() != Layers[0].Image->GetSize())
    {
      Layers[Layers.size() - 1].Image->Resize(Layers[0].Image->GetSize(), wxPoint(0, 0));
    }
  }
}
void Accel_ImagePanel::CheckVisability(int index, bool state)
{
  Layers[index].Enabled = state;
}
unsigned char* Accel_ImagePanel::GetRGBChannel(unsigned layer)
{
  return Layers[layer].Image->GetData();
}
unsigned char* Accel_ImagePanel::GetAlphaChannel(unsigned layer)
{
  return Layers[layer].Image->GetAlpha();
}
bool Accel_ImagePanel::GetVisability(unsigned layer)
{
  return Layers[layer].Enabled;
}
unsigned Accel_ImagePanel::GetCanvasWidth()
{
  return img_width;
}
unsigned Accel_ImagePanel::GetCanvasHeight()
{
  return img_height;
}

unsigned Accel_ImagePanel::LayerCount()
{
  return Layers.size();
}
std::vector<Layer>* Accel_ImagePanel::GetLayers()
{
  return &Layers;
}