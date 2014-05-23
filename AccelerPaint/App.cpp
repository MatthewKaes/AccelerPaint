#include "App.h"
#include "AccelerPaint.h"

#include <wx/image.h>

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  bool wxsOK = true;
  wxInitAllImageHandlers();
  if( wxsOK )
  {
    	AccelerPaint* Frame = new AccelerPaint(0);
    	Frame->Show();
    	SetTopWindow(Frame);
  }
  return wxsOK;
}