//
// Created by Mei Kai Koh on 2018-01-26.
//

#include "Displayable.h"

// A rectangle displayable
void Rectangle::paint(XInfo& xinfo) {
        if (x > 850 - width) {
            XFillRectangle(xinfo.display, d, xinfo.gc, x - 850, y, width, height);
        }
        else if (x < 0) {
            XFillRectangle(xinfo.display, d, xinfo.gc, 850 + x, y, width, height);
        }
        XFillRectangle(xinfo.display, d, xinfo.gc, x, y, width, height);
}

Rectangle::Rectangle(int x, int y, int width, int height, Drawable d): x(x), y(y), width(width), height(height), d(d) {}

void Rectangle::translate(int newX, int newY) {
    x = newX;
    y = newY;
}

void Rectangle::move(string direction, XInfo& xinfo, int level_number) {
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

void Rectangle::changeLevel(string newString) {}

int Rectangle::getX() {
    return x;
}

int Rectangle::getY() {
    return y;
}

int Rectangle::getWidth() {
    return width;
}

int Rectangle::getHeight() {
    return height;
}

void Text::paint(XInfo& xinfo) {
    XDrawImageString( xinfo.display, d, xinfo.gc, x, y, s.c_str(), s.length() );
}

Text::Text(int x, int y, string s, Drawable d): x(x), y(y), s(s), d(d)  {}

void Text::translate(int newX, int newY) {
    x = newX;
    y = newY;
}

void Text::changeLevel(string newString) {
    s = newString;
}

int Text::getX() {
    return x;
}

int Text::getY() {
    return y;
}
