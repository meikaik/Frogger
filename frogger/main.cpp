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
    virtual void translate(int x, int y) = 0;
    virtual void changeLevel(string newString) = 0;
    virtual void move(string direction, XInfo& xinfo, int level_number) = 0;
    virtual int getX() = 0;
    virtual int getY() = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

};

// A rectangle displayable
class Rectangle : public Displayable {
public:
    virtual void paint(XInfo& xinfo) {
        if (x > 850 - width) {
            XFillRectangle(xinfo.display, d, xinfo.gc, -(850- x), y, width, height);
        }
        else if (x < 0) {
            XFillRectangle(xinfo.display, d, xinfo.gc, 850 + x, y, width, height);
        }
        XFillRectangle(xinfo.display, d, xinfo.gc, x, y, width, height);
    }

    Rectangle(int x, int y, int width, int height, Drawable d): x(x), y(y), width(width), height(height), d(d) {}

    virtual void translate(int newX, int newY) {
        x = newX;
        y = newY;
    }
    virtual void move(string direction, XInfo& xinfo, int level_number) {
        if (direction == "right") {
            if (x < 850) {
                x += level_number;
            }
            else x = 0;
        }
        else if (direction == "left") {
            if (x > 0 - width) {
                x -= level_number;
            }
            else x = 850 - width;
        }
    }

    virtual void changeLevel(string newString) {}

    virtual int getX() {
        return x;
    }

    virtual int getY() {
        return y;
    }

    virtual int getWidth() {
        return width;
    }

    virtual int getHeight() {
        return height;
    }

private:
    int x;
    int y;
    int width;
    int height;
    Drawable d;
};

class Text : public Displayable {
public:
    virtual void paint(XInfo& xinfo) {
        XDrawImageString( xinfo.display, d, xinfo.gc, x, y, s.c_str(), s.length() );
    }

    // constructor
    Text(int x, int y, string s, Drawable d): x(x), y(y), s(s), d(d)  {}

    virtual void translate(int newX, int newY) {
        x = newX;
        y = newY;
    }

    virtual void move(string direction, XInfo& xinfo, int level_number) { }

    void changeLevel(string newString) {
        s = newString;
    }

    virtual int getX() {
        return x;
    }

    virtual int getY() {
        return y;
    }

    virtual int getWidth() {
    }

    virtual int getHeight() {
    }

private:
    int x;
    int y;
    string s; // string to show
    Drawable d;
};

// Function to put out a message on error exits.
void error( string str ) {
    cerr << str << endl;
    exit(0);
}

struct Point {
    int x, y;
};

bool checkCollision(Point left1, Point left2, Point right1, Point right2)
{
    if (left1.x >= right2.x || right1.x >= left2.x || left2.y <= right1.y || right2.y <= left1.y) {
        return false;
    }
    return true;
}

// The loop responding to events from the user.
void eventloop(XInfo& xinfo, XWindowAttributes& w, Pixmap& buffer) {
    int frogX = 400;
    int frogY = 200;
    int level_number = 1;

    XEvent event;
    KeySym key;
    char text[BufferSize];
    vector<Displayable*> dList;
    dList.push_back(new Text(w.width - 80, 25, "Level: 1", buffer));
    dList.push_back(new Rectangle(frogX, frogY, 50, 50, buffer));
    Displayable* level = dList[0];
    Displayable* frog = dList[1];

    // Add all the moving blocks
    dList.push_back(new Rectangle(0, 50, 50, 50, buffer));
    dList.push_back(new Rectangle(200, 50, 50, 50, buffer));
    dList.push_back(new Rectangle(400, 50, 50, 50, buffer));

    dList.push_back(new Rectangle(0, 100, 20, 50, buffer));
    dList.push_back(new Rectangle(200, 100, 20, 50, buffer));
    dList.push_back(new Rectangle(400, 100, 20, 50, buffer));
    dList.push_back(new Rectangle(600, 100, 20, 50, buffer));

    dList.push_back(new Rectangle(0, 150, 100, 50, buffer));
    dList.push_back(new Rectangle(400, 150, 100, 50, buffer));


    while ( true ) {
        unsigned long end = now();

        if (end - lastRepaint > 1000000 / FPS) {

            Pixmap pixmap;

            pixmap = buffer;

            // draw into the buffer
            XSetForeground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, DefaultScreen(xinfo.display)));
            XFillRectangle(xinfo.display, pixmap, xinfo.gc, 0, 0, w.width, w.height);

            for (int i = 2; i <= 10; i++) {
                Point point1 = {frogX, frogY};
                Point point2 = {frogX + 50, frogY + 50};
                Point point3 = {dList[i]->getX(), dList[i]->getY()};
                Point point4 = {point3.x + dList[i]->getWidth(), point3.y + dList[i]->getHeight()};
                if (checkCollision(point1, point2, point3, point4)) {
                    frogX = 400;
                    frogY = 200;
                    level_number = 1;
                    level->changeLevel("Level: " + to_string(level_number));
                    frog->translate(frogX, frogY);
                }
                if (dList[i]->getX() > (850 - dList[i]->getWidth())) {
                    point3 = {-(850 - dList[i]->getX()), dList[i]->getY()};
                    point4 = {point3.x + dList[i]->getWidth(), point3.y + dList[i]->getHeight()};
                    if (checkCollision(point1, point2, point3, point4)){
                        frogX = 400;
                        frogY = 200;
                        level_number = 1;
                        level->changeLevel("Level: " + to_string(level_number));
                        frog->translate(frogX, frogY);
                    }

                }
                if (dList[i]->getX() < 0) {
                    point3 = {850 + dList[i]->getX(), dList[i]->getY()};
                    point4 = {point3.x + dList[i]->getWidth(), point3.y + dList[i]->getHeight()};
                    if (checkCollision(point1, point2, point3, point4)){
                        frogX = 400;
                        frogY = 200;
                        level_number = 1;
                        level->changeLevel("Level: " + to_string(level_number));
                        frog->translate(frogX, frogY);
                    }
                }
                if ((i >= 5) && (i <= 8)) {
                    dList[i]->move("left", xinfo, level_number);
                }
                else {
                    dList[i]->move("right", xinfo, level_number);
                }
            }

            XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
            auto begin = dList.begin();
            auto end = dList.end();
            while (  begin != end ) {
                Displayable* d = *begin;
                d->paint(xinfo);
                begin++;
            }

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

                    if (frogY == 0 && i == 1 && text[0] == 'n') {
                        frogX = 400;
                        frogY = 200;
                        level_number++;
                        level->changeLevel("Level: " + to_string(level_number));
                        frog->translate(frogX, frogY);
                    }

                    switch(key){
                        case XK_Up:
                            cout << "Up" << endl;
                            if( frogY - 50 >= 0 ) {
                                frogY -= 50;
                            }
                            else {
                                cout << "Out of Bounds (Y Axis)" << endl;
                            }
                            frog->translate(frogX, frogY);
                            cout << "Current Position:" << frogX << ", " << frogY << endl;
                            break;
                        case XK_Down:
                            cout << "Down" << endl;
                            if( frogY != 0 && frogY + 50 < w.height ) {
                                frogY += 50;
                            }
                            else {
                                cout << "Out of Bounds (Y Axis)" << endl;
                            }
                            frog->translate(frogX, frogY);
                            cout << "Current Position:" << frogX << ", " << frogY << endl;
                            break;
                        case XK_Left:
                            cout << "Left" << endl;

                            if( frogX - 50 >= 0 ) {
                                frogX -= 50;                            }
                            else {
                                cout << "Out of Bounds (X Axis)" << endl;
                            }
                            frog->translate(frogX, frogY);
                            cout << "Current Position:" << frogX << ", " << frogY << endl;
                            break;
                        case XK_Right:
                            cout << "Right" << endl;
                            if( frogX + 50 < w.width ) {
                                frogX += 50;
                            }
                            else {
                                cout << "Out of Bounds (Y Axis)" << endl;
                            }
                            frog->translate(frogX, frogY);
                            cout << "Current Position:" << frogX << ", " << frogY << endl;
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
