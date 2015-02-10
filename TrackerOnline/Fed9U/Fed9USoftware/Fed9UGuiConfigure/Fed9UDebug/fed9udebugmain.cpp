#include <qapplication.h>
#include "fed9udebug.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Fed9UDebug w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
