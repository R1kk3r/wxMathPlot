// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../../mathplot.h"

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/print.h"

#include <math.h>


#define TIMER_PERIOD_MS 	25

class MyFrame;
class MyApp;


// MyFrame

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnFit( wxCommandEvent &event );
    void OnTimer(wxTimerEvent& event);
    void OnPrintPreview( wxCommandEvent &event);
    void OnPrint( wxCommandEvent &event );

    mpWindow        *m_plot;
    wxTextCtrl      *m_log;
    wxTimer         *m_Timer;

private:
    int axesPos[2];
    bool ticks;
    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};

// MyApp

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

// main program

IMPLEMENT_APP(MyApp)

// MyFrame

enum {
    ID_QUIT  = 108,
    ID_ABOUT,
    ID_PRINT,
    ID_PRINT_PREVIEW,
    ID_ALIGN_X_AXIS,
    ID_ALIGN_Y_AXIS,
    ID_TOGGLE_GRID,
    TIMER_ID
};

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU(ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(ID_QUIT,  MyFrame::OnQuit)
  EVT_MENU(ID_PRINT_PREVIEW, MyFrame::OnPrintPreview)
  EVT_MENU(ID_PRINT, MyFrame::OnPrint)
  EVT_MENU(mpID_FIT, MyFrame::OnFit)
  EVT_TIMER(TIMER_ID, MyFrame::OnTimer)
 END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, wxT("wxWindows mathplot sample #3 - 'mpMovableObject' objects"),
			wxDefaultPosition, wxSize(500,500) )
{
    wxMenu *file_menu = new wxMenu();
    wxMenu *view_menu = new wxMenu();

    file_menu->Append( ID_PRINT_PREVIEW, wxT("Print Pre&view..."));
    file_menu->Append( ID_PRINT, wxT("&Print..."));
    file_menu->Append( ID_ABOUT, wxT("&About..."));
    file_menu->Append( ID_QUIT,  wxT("E&xit\tAlt-X"));

    view_menu->Append( mpID_FIT,      wxT("&Fit bounding box"), wxT("Set plot view to show all items"));
    view_menu->Append( mpID_ZOOM_IN,  wxT("Zoom in"),           wxT("Zoom in plot view."));
    view_menu->Append( mpID_ZOOM_OUT, wxT("Zoom out"),          wxT("Zoom out plot view."));

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(view_menu, wxT("&View"));

    SetMenuBar( menu_bar );
    CreateStatusBar(1);

    //mpLayer* l;

    m_plot = new mpWindow( this, -1, wxPoint(0,0), wxSize(100,100), wxSUNKEN_BORDER );

    m_plot->AddLayer(     new mpScaleX(wxT("x"), mpALIGN_CENTER, true) );
    m_plot->AddLayer(     new mpScaleY(wxT("y"), mpALIGN_CENTER, true) );

    mpBitmapLayer   *bmpLayer;
    m_plot->AddLayer(bmpLayer=new  mpBitmapLayer() );

    wxImage        bmp;
    ::wxInitAllImageHandlers();
    bmp.LoadFile(wxT("./gridmap.png"),wxBITMAP_TYPE_PNG);

    bmp.SetMaskColour(0,0,0);

    bmpLayer->SetBitmap( bmp, -40,-40,120,120 );



    m_plot->AddLayer(     new mpCovarianceEllipse( 0.4,0.4,0.2, 2, 32, wxT("Cov1") ) );
    m_plot->AddLayer(     new mpCovarianceEllipse( 0.2,0.2,-0.1, 2, 32, wxT("Cov2") ) );

    // Car shape:
    std::vector<double> car_xs(20), car_ys(20);
    int     i=0;
    car_xs[i]=-0.5;  car_ys[i++]=-0.5;
    car_xs[i]=-0.2;  car_ys[i++]=-0.5;
    car_xs[i]=-0.2;  car_ys[i++]=-0.6;
    car_xs[i]= 0;     car_ys[i++]=-0.6;
    car_xs[i]= 0;     car_ys[i++]=-0.5;
    car_xs[i]= 0.6;  car_ys[i++]=-0.5;
    car_xs[i]= 0.6;  car_ys[i++]=-0.6;
    car_xs[i]= 0.8;  car_ys[i++]=-0.6;
    car_xs[i]= 0.8;  car_ys[i++]=-0.5;
    car_xs[i]= 1.0;  car_ys[i++]=-0.5;
    car_xs[i]= 1.0;  car_ys[i++]= 0.5;
    car_xs[i]= 0.8;  car_ys[i++]= 0.5;
    car_xs[i]= 0.8;  car_ys[i++]= 0.6;
    car_xs[i]= 0.6;  car_ys[i++]= 0.6;
    car_xs[i]= 0.6;  car_ys[i++]= 0.5;
    car_xs[i]= 0;     car_ys[i++]= 0.5;
    car_xs[i]= 0;     car_ys[i++]= 0.6;
    car_xs[i]=-0.2;  car_ys[i++]= 0.6;
    car_xs[i]=-0.2;  car_ys[i++]= 0.5;
    car_xs[i]=-0.5;  car_ys[i++]= 0.5;

    mpPolygon *lCar;
    m_plot->AddLayer( lCar= new mpPolygon( wxT("Vehicle") ) );
    lCar->SetPen( wxPen(*wxBLACK, 3, wxSOLID) );
    lCar->setPoints( car_xs,car_ys, true );

    m_plot->GetLayerByName(wxT("Cov1"))->SetPen( wxPen(*wxRED, 2, wxSOLID) );
    m_plot->GetLayerByName(wxT("Cov2"))->SetPen( wxPen(*wxBLUE, 2, wxSOLID) );

    mpMovableObject *obj;
    obj = (mpMovableObject*)m_plot->GetLayerByName(wxT("Cov2"));
    obj->SetCoordinateBase(12,7,0);

    obj = (mpMovableObject*)m_plot->GetLayerByName(wxT("Cov1"));
    obj->SetCoordinateBase(-4,-4,1);

    m_log = new wxTextCtrl( this, -1, wxT("This is the log window.\n"), wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE );
    wxLog *old_log = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
    delete old_log;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    topsizer->Add( m_plot, 1, wxEXPAND );
    topsizer->Add( m_log, 0, wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
    axesPos[0] = 0;
    axesPos[1] = 0;
    ticks = true;


	m_plot->EnableDoubleBuffer(true);
    m_plot->LockAspect( true );
    m_plot->Fit();

    m_Timer = new wxTimer(this,TIMER_ID);
    m_Timer->Start( TIMER_PERIOD_MS );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( TRUE );
}

void MyFrame::OnFit( wxCommandEvent &WXUNUSED(event) )
{
    m_plot->Fit();
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    wxMessageBox( wxT("wxWidgets mathplot sample #3\n(c) 2007 David Schalig, Davide Rondini, Jose Luis Blanco"));
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    mpMovableObject *obj = (mpMovableObject*)m_plot->GetLayerByName(wxT("Vehicle"));
    if (obj)
    {
        double x,y,phi, v,w, At= m_Timer->GetInterval() * 0.001;
        obj->GetCoordinateBase(x,y,phi);

        if (x<=5 && x>=0)
        {
            v = 5;
            w = 0;
        }
        else
        {
            v = 4;
            w = 1;
        }

        x += cos(phi) * v * At;
        y += sin(phi) * v * At;
        phi+= w * At;

        obj->SetCoordinateBase(x,y,phi);
        m_plot->UpdateAll();
    }


}

void MyFrame::OnPrintPreview( wxCommandEvent &event)
{
	m_Timer->Stop();

      // Pass two printout objects: for preview, and possible printing.
      mpPrintout *plotPrint = new mpPrintout(m_plot);
      mpPrintout *plotPrintPreview = new mpPrintout(m_plot);
      wxPrintPreview *preview = new wxPrintPreview(plotPrintPreview, plotPrint);
      wxPreviewFrame *frame = new wxPreviewFrame(preview, this, wxT("Print Plot"), wxPoint(100, 100), wxSize(600, 650));
      frame->Centre(wxBOTH);
      frame->Initialize();
      frame->Show(true);


	m_Timer->Start( TIMER_PERIOD_MS );
}

void MyFrame::OnPrint( wxCommandEvent &event )
{
	m_Timer->Stop();

      wxPrinter printer;
      mpPrintout printout(m_plot, wxT("Plot print"));
      printer.Print(this, &printout, true);

	m_Timer->Start( TIMER_PERIOD_MS );
}


//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    wxFrame *frame = new MyFrame();
    frame->Show( TRUE );

    return TRUE;
}

