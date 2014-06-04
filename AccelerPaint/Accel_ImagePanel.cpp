#include "Accel_ImagePanel.h"
#include "wx\window.h"
#include "wx\dcbuffer.h"

Accel_ImagePanel::Accel_ImagePanel(wxWindow* parent, wxScrollBar* hscroll, wxScrollBar* vscroll)
{
  ImagePanel = NULL;

  hscroll_ = hscroll;
  vscroll_ = vscroll;

  Create(parent, 0, 0,  parent->GetSize().GetX() - 18, parent->GetSize().GetY() - 18);
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
    if(img_width < parent_->GetSize().GetWidth() - 18)
    {
      rend_width = img_width;
      hscroll_->Enable(false);
      x_off = 0;
    }
    else
    {
      rend_width = parent_->GetSize().GetWidth() - 18;
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
    if(img_height < parent_->GetSize().GetHeight() - 18)
    {
      rend_height = img_height;
      vscroll_->Enable(false);
      y_off = 0;
    }
    else
    {
      rend_height = parent_->GetSize().GetHeight() - 18;
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
    ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - rend_width - 18) / 2, (parent_->GetSize().GetHeight() - rend_height - 18) / 2));
	  
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

    img_width = ImagePtr->GetWidth();// + BORDER_SIZE;
    img_height = ImagePtr->GetHeight();// + BORDER_SIZE;

    ImagePanel->SetSize(img_width, img_height);
    ImagePanel->SetPosition(wxPoint((parent_->GetSize().GetWidth() - img_width - 18) / 2, 
                            (parent_->GetSize().GetHeight() - img_height - 18) / 2));
  }
}
void Accel_ImagePanel::CheckVisability(int index, bool state)
{
  Layers[index].Enabled = state;
}
wxBitmap Accel_ImagePanel::Render()
{
  wxMemoryDC dc;
  wxBitmap output(img_width, img_height, dc);
  dc.SelectObject(output);

  for(unsigned layer = 0; layer < Layers.size(); layer++)
    if(Layers[layer].Enabled)
      dc.DrawBitmap(*Layers[layer].Image, 0, 0, true);

  return output;
}

unsigned Accel_ImagePanel::LayerCount()
{
  return Layers.size();
}
std::vector<Layer>& Accel_ImagePanel::GetLayers()
{
  return Layers;
}