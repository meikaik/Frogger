//
// Created by Mei Kai Koh on 2018-01-26.
//

#ifndef FROGGER_XINFO_H
#define FROGGER_XINFO_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>


struct XInfo {
    Display*  display;
    Window   window;
    GC       gc;

    static void initX(int argc, char* argv[], XInfo& xinfo);
    };

#endif //FROGGER_XINFO_H
