// Wireless Path Loss Example written in OpenGL
// Mac Clayton
// mckenzieclayton@gatech.edu
// ECE 3090, Spring 2012

#include "math.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <GLUT/glut.h>

#define BORD 10
#define SIZE 256
#define WSIZE 276

using namespace std;

// Global Variables:
char *easel;

// Function Prototypes:
void setpixel(char *buf, int x, int y, int r, int g, int b);
void myglinit();
void drawfunc();
void keyfunc(unsigned char k, int x, int y);

//Create a point class of two ints:
class Point
{
public:
    Point();
    Point(int, int);
    int x;
    int y;
};

//Create a wall class made up of two points or 4 ints
class Wall
{
public:
    Wall();
    Wall(int, int, int, int);
    Wall(Point, Point);
    void Draw(char*);
    int Intersect(Point);
    Point p1;
    Point p2;
};

//The heart of this program is this function
void PathLoss(char *data, vector<Wall> Walls)
{
    // Various Constants
    //
    // Power recieved at device:
    float Pr;
    // Transmit powe at wireless base station:
    float Pt = 20.0;
    // Antenna gain factor at transmitter:
    float Gt = 0.0;
    // Antenna gain factor at reciever:
    float Gr = 0.0;
    // Carrier Frequency 2.4GHz:
    float f = 2.4e9;
    // Speed of light 3x10^8 m/s:
    float C = 3e8;
    // Number of walls in the building:
    int k = 0;
    // Signal loss through each wall (dBm):
    int X = 5;
    // The Signal disappation in freespace:
    float fSpace = 10.0 * log10(4 * M_PI * f / C);

    // Create the inital point P:
    Point P;

    // Create R = distance; Green and Red colors:
    float R, Green, Red;

    // Initial number of intersections:
    int intersections = 0;

    // Iterate over very pixel:
    for(unsigned x = 0; x < SIZE; x++)
    {
        for(unsigned y = 0; y < SIZE; y++)
        {
            //Create an initial point:
            P = Point(x,y);
            
            //Set number of intersections to zero:
            intersections = 0;

            //The distance from the base station located at (1,1)
            R = sqrt((x - 1) * (x - 1) + (y - 1) * (y - 1));

            //Iterate through the walls and see if any of them intersect our path:
            for (unsigned i = 0; i < Walls.size(); i++)
            {
                intersections += Walls[i].Intersect(P);
            }

            //Calculate the power at the receiver:
            Pr = Pt - fSpace - 20*log10(R) - 5 * intersections;

            //Set boundaries for color:
            if(Pr > -20.0) Pr = -20.0;
            if(Pr < -80.0) Pr = -80.0;

            //Set the Red and Green values based on Pr:
            Pr += 20.0;
            if( Pr > -30.0 )
            {
                Green = 255;
                Red = -Pr * 255.0 / 30.0;
            }
            else 
            {
                Green = 255 + (Pr + 30) * 255.0 / 30.0;
                Red = 255;
            }
            //Write the pixel's color to the data array:
            setpixel(data, (x + BORD), (y + BORD), Red, Green, 0);
        }
    }//End Loops    
}//End path loss

      
int main(int argc, char** argv)
{    
    // Create a Vector of walls:
    vector<Wall> Walls;

    // Create pixel buffer and make it all white
    easel = new char[WSIZE * WSIZE * 3];
    memset(easel, 0xff, WSIZE * WSIZE * 3);

    // Init GLUT stuff:
    glutInit(&argc, argv);                          // Init GLUT
    glutInitWindowSize(WSIZE, WSIZE);               // Set Window Size
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);    // Set Display Mode to RGB 
    glutCreateWindow("Pathloss OpenGL Example");    // Create Window
    
    //Read in the walls txt file:
    ifstream w("walls.txt");
    if (!w)
    {
        cout << "No walls.txt file found" << endl;
        return(1);
    }
    // Read the walls file
    while(w)
    {
        int x0 = -1;
        int y0 = -1;
        int x1 = -1;
        int y1 = -1;
        w >> x0 >> y0 >> x1 >> y1;
        if (x0 < 0) break; // done
        
        //Make sure that pt2 is greater than pt1:
        if(y0 > y1)
        {
            int temp = y0;
            y0 = y1;
            y1 = temp;
        }
        if(x0 > x1)
        {
            int temp = x0;
            x0 = x1;
            x1 = temp;
        }
        //Add the wall to the vector:
        Walls.push_back(Wall(x0, y0, x1, y1));
    }
    //Close the file w:
    w.close();
    cout << "Done reading Walls" << endl;
    cout << "There are " << Walls.size() << " walls" << endl;

    PathLoss(easel, Walls);
    
    // Draw the Walls:
    for(int  i = 0; i < Walls.size(); i++)
    {
        Walls[i].Draw(easel);
    }

    glutDisplayFunc(drawfunc);
    glutKeyboardFunc(keyfunc);
    glutMainLoop();
    
    //Finishing touches:
    cout << "Done" << endl;
}//End Main


//Point default constructor
Point::Point()
{
    x = 0;
    y = 0;
}

//Point constructor with an X and Y value:
Point::Point(int a, int b)
{
    x = a;
    y = b;
}

//Wall default constructor
Wall::Wall()
{
    p1 = Point();
    p2 = Point();
}

//Wall Constructor for 2 points:
Wall::Wall(Point a, Point b)
{
    p1 = a;
    p2 = b;
}

//Wall constructor for 4 coordinates:
Wall::Wall(int x1, int y1, int x2, int y2)
{
    p1 = Point(x1, y1);
    p2 = Point(x2, y2);
}

//Draw the Wall in Black:
void Wall::Draw(char *data)
{
    //Create counting variables
    int x = p1.x;
    int y = p1.y;

    if (p1.x == p2.x)
    {
        while(y < p2.y)
        {
            setpixel(data, (x + BORD), (y + BORD), 0, 0, 0);
           ++y;
        }
    }

    if(x > p2.x)
    {
        int temp = x;
        x = p2.x;
        p2.x = temp;
    }

    //If the wall is vertical (y1 == y2)
    else if(p1.y == p2.y)
    {
        while(x < p2.x)
        {
            setpixel(data, (x + BORD), (y + BORD), 0, 0, 0);
           ++x;    
        }
    }
}

//Check if a Wall intersects a Point:
int Wall::Intersect(Point pt)
{
    float x,y;
    
    //If the wall is horizontal:
    if(p1.x == p2.x)
    {
        if((p1.x > pt.x) || (p1.x == 0)) return 0;
        y = (p1.x * pt.y) / pt.x;
        if((y >= p1.y) && (y < p2.y))    return 1;
        
    //If the wall is vertical:    
    }
    else if(p1.y == p2.y)
    {
        if((p1.y > pt.y) || (p1.y == 0)) return 0;
        x = (p1.y * pt.x) / pt.y;
        if((x >= p1.x) && (x < p2.x))    return 1;
    }
    //Otherwise return 0
    return 0;
}//End Intersect

void keyfunc(unsigned char k, int x, int y)
{
    switch (k)
    {
        // Escape:
        case 27:
            exit(0);
        break;
    }
}

// Set pixel function used to interact with pixelMap
void setpixel(char *data, int x, int y, int r, int g, int b)
{
    data[((WSIZE - y) * WSIZE + x) * 3 + 0] = r;
    data[((WSIZE - y) * WSIZE + x) * 3 + 1] = g;
    data[((WSIZE - y) * WSIZE + x) * 3 + 2] = b;
}

// Draw Callback function used in glutDisplayFunc
void drawfunc(void)
{
    glDrawPixels(WSIZE, WSIZE, GL_RGB, GL_UNSIGNED_BYTE, easel);
    glutSwapBuffers();
    glutPostRedisplay();
}




      
  
    

