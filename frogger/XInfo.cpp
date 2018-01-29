//
// Created by Mei Kai Koh
//

#include "XInfo.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

const int Border = 5;

// Function to put out a message on error exits.
void error( string str ) {
    cerr << str << endl;
    exit(0);
}

//  Create the window;  initialize X.
void XInfo::initX(int argc, char* argv[], XInfo& xinfo) {

    xinfo.display = XOpenDisplay( "" );
    if ( !xinfo.display ) {
        error( "Can't open display." );
    }

    int screen = DefaultScreen( xinfo.display );
    unsigned long background = WhitePixel( xinfo.display, screen );
    unsigned long foreground = BlackPixel( xinfo.display, screen );


    XSizeHints hints;
    hints.x = 100;
    hints.y = 100;
    hints.width = 850;
    hints.height = 250;
    hints.flags = PPosition | PSize;
    xinfo.window = XCreateSimpleWindow( xinfo.display, DefaultRootWindow( xinfo.display ),
                                        hints.x, hints.y, hints.width, hints.height,
                                        Border, foreground, background );
    XSetStandardProperties( xinfo.display, xinfo.window, "frogger", "frogger", None,
                            argv, argc, &hints );


    xinfo.gc = XCreateGC (xinfo.display, xinfo.window, 0, 0 );
    XSetBackground( xinfo.display, xinfo.gc, background );
    XSetForeground( xinfo.display, xinfo.gc, foreground );

    // Tell the window manager what input events you want.
    XSelectInput( xinfo.display, xinfo.window, KeyPressMask );

    XMapRaised( xinfo.display, xinfo.window );
}