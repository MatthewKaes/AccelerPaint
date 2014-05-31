#ifndef MYAPP_FRAMEWORK_H
#define MYAPP_FRAMEWORK_H
#include <wx/app.h>
#include "AccelerPaint.h"

class App : public wxApp
{
  public:
    virtual bool OnInit();

  private:
    AccelerPaint* Frame;
};

#endif