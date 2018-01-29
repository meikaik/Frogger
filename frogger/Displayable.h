//
// Created by Mei Kai Koh
//

#ifndef FROGGER_DISPLAYABLE_H
#define FROGGER_DISPLAYABLE_H

#include "XInfo.h"
#include <string>

using namespace std;

class Displayable {
public:
    virtual ~Displayable();
    virtual void paint(XInfo& xinfo) = 0;
    virtual void translate(int x, int y) = 0;
    virtual void changeLevel(string newString) = 0;
    virtual void move(string direction, XInfo& xinfo, int level_number) {}
    virtual int getX() = 0;
    virtual int getY() = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

};

// A rectangle displayable
class Rectangle : public Displayable {
public:
    virtual void paint(XInfo& xinfo);

    Rectangle(int x, int y, int width, int height, Drawable d);
    ~Rectangle();

    virtual void translate(int newX, int newY);
    virtual void move(string direction, XInfo& xinfo, int level_number);
    virtual void changeLevel(string newString);
    virtual int getX();
    virtual int getY();
    virtual int getWidth();
    virtual int getHeight();
private:
    int x;
    int y;
    int width;
    int height;
    Drawable d;

};

class Text : public Displayable {
public:
    virtual void paint(XInfo& xinfo);

    Text(int x, int y, string s, Drawable d);
    ~Text();

    virtual void translate(int newX, int newY);
    void changeLevel(string newString);
    virtual int getX();
    virtual int getY();
    virtual int getWidth();
    virtual int getHeight();
private:
    int x;
    int y;
    string s; // string to show
    Drawable d;

};

#endif //FROGGER_DISPLAYABLE_H
