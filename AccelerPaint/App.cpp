#include "App.h"

#include <wx/image.h>

IMPLEMENT_APP(App);

bool App::OnInit()
{
  bool wxsOK = true;
  wxInitAllImageHandlers();
  if( wxsOK )
  {
    Frame = new AccelerPaint(0);
    Frame->Show();
    SetTopWindow(Frame);
  }
  return wxsOK;
}