#include <qapplication.h>
#include "fed9uguiconfig.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Fed9UGUIConfig w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
