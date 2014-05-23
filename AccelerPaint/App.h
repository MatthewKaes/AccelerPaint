#ifndef MYAPP_FRAMEWORK_H
#define MYAPP_FRAMEWORK_H
#include <wx/app.h>

class MyApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif