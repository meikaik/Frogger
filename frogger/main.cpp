#include <iostream>
#include <list>
#include <cstdlib>
#include <vector>

//#include <string>
//#include <sstream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/time.h>
#include <unistd.h>

using namespace std;

// get microseconds
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int FPS = 30;
unsigned long lastRepaint = 0;

const int Border = 5;
const int BufferSize = 10;

struct XInfo {
    Display*  display;
    Window   window;
    GC       gc;
};


// An abstract class representing displayable things.
class Displayable {
public:
    virtual void paint(XInfo& xinfo) = 0;
};

// A rectangle displayable
class Rectangle : public Displayable {
public:
    virtual void paint(XInfo& xinfo) {
        XFillRectangle(xinfo.display, xinfo.window, xinfo.gc, this->x, this->y, this->width, this->height);
    }

    Rectangle(int x, int y, int width, int height): x(x), y(y), width(width), height(height) {}

private:
    int x;
    int y;
    int width;
    int height;
};

class Text : public Displayable {
public:
    virtual void paint(XInfo& xinfo) {
        XDrawImageString( xinfo.display, xinfo.window, xinfo.gc,
                          this->x, this->y, this->s.c_str(), this->s.length() );
    }

    // constructor
    Text(int x, int y, string s): x(x), y(y), s(s)  {}

private:
    int x;
    int y;
    string s; // string to show
};

// Function to put out a message on error exits.
void error( string str ) {
    cerr << str << endl;
    exit(0);
}

// The loop responding to events from the user.
void eventloop(XInfo& xinfo, XWindowAttributes& w, Pixmap& buffer) {
    int frogX = 400;
    int frogY = 200;

    XEvent event;
    KeySym key;
    char text[BufferSize];
    list<Displayable*> dList;
    dList.push_back(new Text(50, 50, "Level: 1"));
    dList.push_back(new Rectangle(frogX, frogY, 50, 50));

    while ( true ) {
        unsigned long end = now();

        if (end - lastRepaint > 1000000 / FPS) {

            Pixmap pixmap;

            pixmap = buffer;

            // draw into the buffer
            XSetForeground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, DefaultScreen(xinfo.display)));
            XFillRectangle(xinfo.display, pixmap, xinfo.gc, 0, 0, w.width, w.height);

            XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
            XDrawImageString(xinfo.display, pixmap, xinfo.gc, w.width - 80, 25, "Level: 1", 8);
            XFillRectangle(xinfo.display, pixmap, xinfo.gc, frogX, frogY, 50, 50);

            XCopyArea(xinfo.display, pixmap, xinfo.window, xinfo.gc,
                      0, 0, w.width, w.height,  // region of pixmap to copy
                      0, 0); // position to put top left corner of pixmap in window

            XFlush( xinfo.display );

            lastRepaint = now(); // remember when the paint happened
        }

        // IMPORTANT: sleep for a bit to let other processes work
        if (XPending(xinfo.display) == 0) {
            usleep(1000000 / FPS - (end - lastRepaint));
        }

        if (XPending(xinfo.display) > 0) {
            XNextEvent( xinfo.display, &event );
            switch ( event.type ) {

                case KeyPress:
                    int i = XLookupString(
                            (XKeyEvent*)&event, text, BufferSize, &key, 0 );
                    cout << "KeySym " << key
                         << " text='" << text << "'"
                         << " at " << event.xkey.time
                         << endl;
                    if ( i == 1 && text[0] == 'q' ) {
                        cout << "Terminated normally." << endl;
                        XCloseDisplay(xinfo.display);
                        return;
                    }

                    switch(key){
                        case XK_Up:
                            cout << "Up" << endl;
                            dList.pop_back();
                            if( frogY - 50 >= 0 ) {
                                frogY -= 50;
                            }
                            else {
                                cout << "Out of Bounds (Y Axis)" << endl;
                            }
                            dList.push_back(new Rectangle(frogX, frogY , 50, 50));
                            break;
                        case XK_Down:
                            cout << "Down" << endl;
                            dList.pop_back();
                            if( frogY + 50 < w.height ) {
                                frogY += 50;
                            }
                            else {
                                cout << "Out of Bounds (Y Axis)" << endl;
                            }
                            dList.push_back(new Rectangle(frogX, frogY , 50, 50));
                            break;
                        case XK_Left:
                            cout << "Left" << endl;
                            dList.pop_back();
                            if( frogX - 50 >= 0 ) {
                                frogX -= 50;                            }
                            else {
                                cout << "Out of Bounds (X Axis)" << endl;
                            }
                            dList.push_back(new Rectangle(frogX, frogY , 50, 50));
                            break;
                        case XK_Right:
                            cout << "Right" << endl;
                            dList.pop_back();
                            if( frogX + 50 < w.width ) {
                                frogX += 50;
                            }
                            else {
                                cout << "Out of Bounds (Y Axis)" << endl;
                            }
                            dList.push_back(new Rectangle(frogX, frogY , 50, 50));
                            break;
                    }

                    break;
            }
        }
    }
}

//  Create the window;  initialize X.
void initX(int argc, char* argv[], XInfo& xinfo) {

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
    XSetStandardProperties( xinfo.display, xinfo.window, "eventloop", "eventloop", None,
                            argv, argc, &hints );


    xinfo.gc = XCreateGC (xinfo.display, xinfo.window, 0, 0 );
    XSetBackground( xinfo.display, xinfo.gc, background );
    XSetForeground( xinfo.display, xinfo.gc, foreground );

    // Tell the window manager what input events you want.
    XSelectInput( xinfo.display, xinfo.window, KeyPressMask );

    XMapRaised( xinfo.display, xinfo.window );
}



int main ( int argc, char* argv[] ) {
    if (argc > 1) {
        FPS = (int)strtol(argv[1], NULL, 10);
    }

    XInfo xinfo;
    initX(argc, argv, xinfo);

    XWindowAttributes w;
    XGetWindowAttributes(xinfo.display, xinfo.window, &w);

    // DOUBLE BUFFER
    // create bitmap (pixmap) to us a other buffer
    int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
    Pixmap buffer = XCreatePixmap(xinfo.display, xinfo.window, w.width, w.height, depth);

    eventloop(xinfo, w, buffer);
}
