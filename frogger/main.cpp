#include <iostream>

#include <vector>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/time.h>
#include <unistd.h>

#include "XInfo.h"
#include "Displayable.h"

using namespace std;

int FPS = 30;
unsigned long lastRepaint = 0;
const int BufferSize = 10;

// get microseconds
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
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

    Pixmap pixmap;
    pixmap = buffer;

    vector<Displayable*> dList;
    dList.push_back(new Text(w.width - 80, 25, "Level: 1", pixmap));
    dList.push_back(new Rectangle(frogX, frogY, 50, 50, pixmap));
    Displayable* level = dList[0];
    Displayable* frog = dList[1];

    // Add all the moving blocks
    for (int i = 0; i < 900; i += 300) {
        dList.push_back(new Rectangle(i, 50, 50, 50, pixmap));
    }
    for (int i = 0; i < 700; i += 200) {
        dList.push_back(new Rectangle(i, 100, 20, 50, pixmap));
    }
    dList.push_back(new Rectangle(0, 150, 100, 50, pixmap));
    dList.push_back(new Rectangle(400, 150, 100, 50, pixmap));


    while ( true ) {
        unsigned long end = now();

        if (end - lastRepaint > 1000000 / FPS) {

            // draw into the buffer
            XSetForeground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, DefaultScreen(xinfo.display)));
            XFillRectangle(xinfo.display, pixmap, xinfo.gc, 0, 0, w.width, w.height);

            // Iterate through all moving blocks in dList( check for collisions and move blocks )
            for (int i = 2; i <= 10; i++) {
                // check for collisions
                Point point1 = {frogX, frogY};
                Point point2 = {point1.x + 50, point1.y + 50};
                Point point3;
                if (dList[i]->getX() > (850 - dList[i]->getWidth())) {
                    point3.x = dList[i]->getX() - 850;
                    point3.y = dList[i]->getY();
                } else if (dList[i]->getX() < 0) {
                    point3.x = 850 + dList[i]->getX();
                    point3.y = dList[i]->getY();
                } else {
                    point3.x = dList[i]->getX();
                    point3.y = dList[i]->getY();
                }
                Point point4 = {point3.x + dList[i]->getWidth(), point3.y + dList[i]->getHeight()};
                if (checkCollision(point1, point2, point3, point4)){
                    frogX = 400;
                    frogY = 200;
                    level_number = 1;
                    level->changeLevel("Level: 1");
                    frog->translate(frogX, frogY);
                }
                // move blocks
                if ((i >= 5) && (i <= 8)) {
                    dList[i]->move("left", xinfo, level_number);
                }
                else {
                    dList[i]->move("right", xinfo, level_number);
                }
            }

            XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
            // repaint onto pixmap
            for (auto begin = dList.begin(); begin != dList.end(); begin++) {
                Displayable* d = *begin;
                d->paint(xinfo);
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

int main ( int argc, char* argv[] ) {
    if (argc > 1) {
        FPS = (int)strtol(argv[1], NULL, 10);
    }

    XInfo xinfo;
    XInfo::initX(argc, argv, xinfo);

    XWindowAttributes w;
    XGetWindowAttributes(xinfo.display, xinfo.window, &w);

    // DOUBLE BUFFER
    // create bitmap (pixmap) to use as other buffer
    int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
    Pixmap buffer = XCreatePixmap(xinfo.display, xinfo.window, w.width, w.height, depth);

    eventloop(xinfo, w, buffer);
}
