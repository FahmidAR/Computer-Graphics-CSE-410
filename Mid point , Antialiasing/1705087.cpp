#include "bitmap_image.hpp"

#include <bits/stdc++.h>
using namespace std;

#define PI (2*acos(0.0))
#define SUBPIXEL 20
#define DEBUG 0

double height , width ;

ifstream inputFile;

class point{
public:
    double x,y;
    point(){}
    point(double _x, double _y){
        x = _x;
        y = _y;
    }
};

class color{
public:
    double r,g,b;
    color(){}

    color(double _r, double _g, double _b){
        r = _r;
        g = _g;
        b = _b;
    }

    void intensityColor(double intensity)
    {
        r *= intensity;
        g *= intensity;
        b *= intensity;
    }

    void set(double _r, double _g, double _b){
        r = _r;
        g = _g;
        b = _b;
    }

    color intensityColorReturn(double intensity)
    {
        color temp;
        temp.r = r * intensity;
        temp.g = g * intensity;
        temp.b = b * intensity;
        return temp;
    }

    void clip()
    {
        if(r>255) r = 255;
        if(g>255) g = 255;
        if(b>255) b = 255;

        if(r<0) r = 0;
        if(g<0) g = 0;
        if(b<0) b = 0;
    }
};

class line{
public:
    point p1,p2;
    color c;
    line(){}
    line(point _p1, point _p2, color _c){
        p1 = _p1;
        p2 = _p2;
        c = _c;
    }
};

void swap(point &p1, point &p2){
    point temp = p1;
    p1 = p2;
    p2 = temp;
}

double coveredAreaRatio(int x, int y, int x0, int y0,int dx, int dy)
{
    double x1 = x0 + dx;
    double y1 = y0 + dy;

    double y_upper = y1 + 0.5;
    double y_lower = y1 - 0.5;

    double subPixel=SUBPIXEL;

    double x_temp , y_temp; 

    x_temp = x;
    y_temp = y;

    double upper_value , lower_value; 

    double covered = 0.0;

    double subPixelCount = 1.0;
    double totalPixelCount = subPixel * subPixel;

    for (int i = 0; i < subPixel; i++)
    {
        for (int j = 0; j < subPixel; j++)
        {
            x_temp = x * 1.0 + (1.0 / subPixel) * (j + 1);
            y_temp = y * 1.0 + (1.0 / subPixel) * (i + 1);

            upper_value = 2 * (dy * x_temp - dx * y_temp + dx * y_upper - dy * x1);
            lower_value = 2 * (dy * x_temp - dx * y_temp + dx * y_lower - dy * x1);

            if (upper_value >= 0 && lower_value <= 0)
            {
                subPixelCount++;
            }
        }
    }

    covered = subPixelCount / totalPixelCount;

    if(covered>1.0) covered = 0.0;
    return 1-covered;
}

double Filter(double distance)
{
    // if(distance>1) return 0.0;
    // return 1-distance;

    if(distance > 1) return 1.0;
    return distance;
}


void drawLine_Midpoint_line_algo(bitmap_image &image, line l){

    if(l.p1.x > l.p2.x){
        swap(l.p1,l.p2);
    }

    double dx = l.p2.x - l.p1.x;
    double dy = l.p2.y - l.p1.y;

    double x = l.p1.x;
    double y = l.p1.y;

    bool slopeOne = false;

    double slope = dy/dx;

    if(DEBUG) cout << "slope: " << slope << endl;

    if( slope>0 && slope < 1.0){

        double d = 2*dy - dx;
        double dE = 2*dy;
        double dNE = 2*(dy-dx);

        while(x <= l.p2.x){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                y++;
            }
            x++;
        }

    }
    else if(slope>=1.0)
    {
        double d = 2*dx - dy;
        double dE = 2*dx;
        double dNE = 2*(dx-dy);

        while(y <= l.p2.y){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                x++;
            }
            y++;
        }
    }
    else if(slope>-1.0 && slope < 0.0)
    {
        dy= -dy;
        double d = 2*dy - dx;
        double dE = 2*dy;
        double dNE = 2*(dy-dx);

        while(x <= l.p2.x){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                y--;
            }
            x++;
        }
    }
    else if(slope<-1.0){

        dy= -dy;
        double d = 2*dx - dy;
        double dE = 2*dx;
        double dNE = 2*(dx-dy);

        while(y >= l.p2.y){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                x++;
            }
            y--;
        }
    }
    else if(slope == 0.0){
        while(x <= l.p2.x){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            x++;
        }
    }

}

void drawLine_Unweighted_Area_Sampling_AntiAliasing(bitmap_image &image, line l){

    if(l.p1.x > l.p2.x){
        swap(l.p1,l.p2);
    }

    double dx = l.p2.x - l.p1.x;
    double dy = l.p2.y - l.p1.y;

    double x = l.p1.x;
    double y = l.p1.y;

    bool slopeOne = false;

    double slope = dy/dx;

    if(DEBUG) cout << "slope: " << slope << endl;

    if( slope>0 && slope < 1.0){

        double d = 2*dy - dx;
        double dE = 2*dy;
        double dNE = 2*(dy-dx);

        while(x <= l.p2.x){


            double intensity = coveredAreaRatio(x,y,l.p1.x,l.p1.y,dx,dy);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y+1),l.p1.x,l.p1.y,dx,dy);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y-1),l.p1.x,l.p1.y,dx,dy);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                y++;
            }
            x++;
        }

    }
    else if(slope>=1.0)
    {
        double d = 2*dx - dy;
        double dE = 2*dx;
        double dNE = 2*(dx-dy);

        while(y <= l.p2.y){
            double intensity = coveredAreaRatio(x,y,l.p1.x,l.p1.y,dx,dy);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            tempColor.clip();
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y+1),l.p1.x,l.p1.y,dx,dy);;
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y-1),l.p1.x,l.p1.y,dx,dy);;
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                x++;
            }
            y++;
        }
    }
    else if(slope>-1.0 && slope < 0.0)
    {
        dy= -dy;
        double d = 2*dy - dx;
        double dE = 2*dy;
        double dNE = 2*(dy-dx);

        while(x <= l.p2.x){
            double intensity = coveredAreaRatio(x,y,l.p1.x,l.p1.y,dx,-dy);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y+1),l.p1.x,l.p1.y,dx,-dy);;
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y-1),l.p1.x,l.p1.y,dx,-dy);;
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                y--;
            }
            x++;
        }
    }
    else if(slope<-1.0){

        dy= -dy;
        double d = 2*dx - dy;
        double dE = 2*dx;
        double dNE = 2*(dx-dy);

        while(y >= l.p2.y){
            double intensity = coveredAreaRatio(x,y,l.p1.x,l.p1.y,dx,-dy);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y+1),l.p1.x,l.p1.y,dx,-dy);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = coveredAreaRatio(x,(y-1),l.p1.x,l.p1.y,dx,-dy);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                x++;
            }
            y--;
        }
    }
    else if(slope == 0.0){
        while(x <= l.p2.x){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            x++;
        }
    }
}

void drawLine_Weighted_Area_Sampling_AntiAliasing(bitmap_image &image, line l){

    if(l.p1.x > l.p2.x){
        swap(l.p1,l.p2);
    }

    double dx = l.p2.x - l.p1.x;
    double dy = l.p2.y - l.p1.y;

    double x = l.p1.x;
    double y = l.p1.y;

    int two_v_dx = 0;
    double invDenom = 1.0/(2.0*sqrt(dx*dx+dy*dy));
    double two_dx_invDemon = 2.0*dx*invDenom;

    bool slopeOne = false;

    double slope = dy/dx;

    if(DEBUG) cout << "slope: " << slope << endl;

    if( slope>0 && slope < 1.0){

        double d = 2*dy - dx;
        double dE = 2*dy;
        double dNE = 2*(dy-dx);


        while(x <= l.p2.x){

            if(d <= 0){
                two_v_dx = d+dx;
                d += dE;
            }
            else{
                two_v_dx = d-dx;
                d += dNE;
                y++;
            }
            x++;

            double intensity = (round(abs(two_v_dx*invDenom)));
            intensity = Filter(intensity);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon - two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon + two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
        }

    }
    else if(slope>=1.0)
    {
        double d = 2*dx - dy;
        double dE = 2*dx;
        double dNE = 2*(dx-dy);

        while(y <= l.p2.y){
            double intensity = (round(abs(two_v_dx*dx*invDenom)));
            intensity = Filter(intensity);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            tempColor.clip();
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon - two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon + two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                x++;
            }
            y++;
        }
    }
    else if(slope>-1.0 && slope < 0.0)
    {
        dy= -dy;
        double d = 2*dy - dx;
        double dE = 2*dy;
        double dNE = 2*(dy-dx);

        while(x <= l.p2.x){
            double intensity = (round(abs(two_v_dx*dx*invDenom)));
            intensity = Filter(intensity);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon - two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon + two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                y--;
            }
            x++;
        }
    }
    else if(slope<-1.0){

        dy= -dy;
        double d = 2*dx - dy;
        double dE = 2*dx;
        double dNE = 2*(dx-dy);

        while(y >= l.p2.y){
            double intensity = (round(abs(two_v_dx*dx*invDenom)));
            intensity = Filter(intensity);
            color tempColor(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-y,tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon - two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y+1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);

            intensity = (round(abs(two_dx_invDemon + two_v_dx*invDenom)));
            intensity = Filter(intensity);
            tempColor.set(l.c.r+l.c.r*intensity,l.c.g+l.c.g*intensity,l.c.b+l.c.b*intensity);
            image.set_pixel(x,height-(y-1),tempColor.r,tempColor.g,tempColor.b);
            //image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            if(d <= 0){
                d += dE;
            }
            else{
                d += dNE;
                x++;
            }
            y--;
        }
    }
    else if(slope == 0.0){
        while(x <= l.p2.x){
            image.set_pixel(x,height-y,l.c.r,l.c.g,l.c.b);
            x++;
        }
    }

}



int main(){

    cout<<"Mid-point algo and Antialiasing"<<endl;
    cout<<"Student ID : 1705087"<<endl;
    cout<<"Lines colored Handling :D\n"<<endl;


    string fileName = "scene.txt";
    inputFile.open(fileName);
    if(!inputFile.is_open()) { exit(EXIT_FAILURE); }

    int total ;
    double x1, y1, x2, y2, r, g, b;


    inputFile >> width >> height ;
    inputFile >> total ;

    if(DEBUG) cout<<"start" << endl;

    bitmap_image image(width, height);
    image.set_all_channels(255, 255, 255);

    bitmap_image image2(width, height);
    image2.set_all_channels(255, 255, 255);

    bitmap_image image3(width, height);
    image3.set_all_channels(255, 255, 255);


    if(DEBUG) cout<<"mid" << endl;

    for(int i = 0 ; i < total ; i++){

        inputFile >> x1 >> y1 >> x2 >> y2 >> r >> g >> b;
        line l(point(x1,y1),point(x2,y2),color(r,g,b));
        l.c.intensityColor(0.5);

        drawLine_Midpoint_line_algo(image,l);
        drawLine_Unweighted_Area_Sampling_AntiAliasing(image2,l);
        drawLine_Weighted_Area_Sampling_AntiAliasing(image3,l);
    }

    cout<<"Input from scene.txt taken successfully !!\n"<<endl;

    if(DEBUG) cout<<"stop" << endl;

    image.save_image("1_R.bmp");

    cout<<"For RUA one PiXEL is divided into "<<SUBPIXEL<<" SUBPIXEl "<<endl;

    if(SUBPIXEL>3)
    {
     cout << "and So realistic image ^_^" << endl;
    }

    image2.save_image("2_RUA.bmp");
    image3.save_image("3_RWA.bmp");

    inputFile.close();

    cout<<"\nBMP images 1_R.bmp,2_RUA.bmp,3_RWA.bmp generated successfully !!"<<endl;

    return 0;

}
