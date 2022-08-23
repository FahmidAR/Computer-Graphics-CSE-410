#include "bitmap_image.hpp"

#include <stack>
#include <iomanip>
#include <algorithm>
using namespace std;

#define PI (2*acos(0.0))
#define DEBUG 0

ifstream inputFile,inputFileConfig,inputFileStage3;
ofstream outputFileStage1,outputFileStage2,outputFileStage3;
ofstream outputFileZbuffer;

double weight = 1;
int triangleCount = 0;
string folder = "4";

class point {

	public:

    double x,y,z;

	point()
	{
        x = y = z = 0.0;
    }

    point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getX() {
        return x;
    }

    double getY() {
        return y;
    }

    double getZ() {
        return z;
    }

	void setX(double x) {
        this->x = x;
    }

    void setY(double y) {
        this->y = y;
    }

    void setZ(double z) {
        this->z = z;
    }

    void normalize();
    void scale(double w);

    point operator+(point vector_point);
    point operator-(point vector_point);
    point operator*(double scalar);
    double operator*(point vector_point);
    point operator&(point vector_point);

};

point point::operator+(point vector_point) {
    return point(x+vector_point.x, y+vector_point.y, z+vector_point.z);
}

point point::operator-(point vector_point) {
    return point(x-vector_point.x, y-vector_point.y, z-vector_point.z);
}

point point::operator*(double scalar) {
    return point(x*scalar, y*scalar, z*scalar);
}

double point::operator*(point vector_point) {
    return (x*vector_point.x)+(y*vector_point.y)+(z*vector_point.z);
}

point point::operator&(point vector_point) {
    return point(y*vector_point.z-z*vector_point.y, z*vector_point.x-x*vector_point.z, x*vector_point.y-y*vector_point.x);
}

void point::normalize() {
    double magnitudeValue = sqrt(x*x+y*y+z*z);
    x /= magnitudeValue;
    y /= magnitudeValue;
    z /= magnitudeValue;
}

point rodriguesFuntion(point axix, point a, double angle)
{
    return axix*cos(angle*PI/180.0)+a*(a*axix)*(1-cos(angle*PI/180.0))+(a&axix)*sin(angle*PI/180.0);
}

void point::scale(double w) {
    x /= w;
    y /= w;
    z /= w;
    w /= w;
}

class color {

    public:

    int colorR,colorG,colorB;

    color()
	{
        colorR = rand()%256;
        colorG = rand()%256;
        colorB = rand()%256;
    }

    color(int r,int g,int b)
    {
        colorR = r;
        colorG = g;
        colorB = b;
    }

    int getR() {
        return colorR;
    }

    int getG() {
        return colorG;
    }

    int getB() {
        return colorB;
    }

	void setR(int x) {
        this->colorR = x;
    }

    void setG(int y) {
        this->colorG = y;
    }

    void setB(int z) {
        this->colorB = z;
    }

};

class triangle {

    public:

    point p1,p2,p3;
    color colorTrainagle;

	triangle()
	{
        point p1 = point();
        point p2 = point();
        point p3 = point();
        colorTrainagle = color();
    }

    triangle(point p1,point p2,point p3) {
        this->p1 = point(p1.getX(),p1.getY(),p1.getZ());
        this->p2 = point(p2.getX(),p2.getY(),p2.getZ());
        this->p3 = point(p3.getX(),p3.getY(),p3.getZ());
        colorTrainagle = color();
    }

    point getP1() {
        return p1;
    }

    point getP2() {
        return p2;
    }

    point getP3() {
        return p3;
    }

    color getColor() {
        return colorTrainagle;
    }

	void setP1(point p) {
        this->p1 = p;
    }

    void setP2(point p) {
        this->p2 = p;
    }

    void setP3(point p) {
        this->p3 = p;
    }

};

class transformation {

	public:

    double homogenousMatrix[4][4];

    transformation()
    {
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<4; j++)
            {
                if(i==j)
                {
                    homogenousMatrix[i][j] = 1;
                }
                else
                {
                    homogenousMatrix[i][j] = 0;
                }
            }
        }
    }

    void genTranslationMatrix(double dx, double dy, double dz);
    void genScalingMatrix(double xs, double ys, double zs);
    void genRotationMatrix(double angle, double ax, double ay, double az);

    void genViewRotationMatrix(point l , point r, point u);

    void genProjectionMatrix(double near, double far, double t, double r);

    void printMatrix();

    transformation operator*(transformation old_matrix);
    point operator*(point old_point);

};

void transformation::genTranslationMatrix(double dx, double dy, double dz) {
    homogenousMatrix[0][3] = dx;
    homogenousMatrix[1][3] = dy;
    homogenousMatrix[2][3] = dz;
}

void transformation::genScalingMatrix(double xs, double ys, double zs) {
    homogenousMatrix[0][0] = xs;
    homogenousMatrix[1][1] = ys;
    homogenousMatrix[2][2] = zs;
}

void transformation::genRotationMatrix(double angle, double ax, double ay, double az) {

    point i = point(1, 0, 0);
    point j = point(0, 1, 0);
    point k = point(0, 0, 1);

    point a= point(ax, ay, az);
    a.normalize();

    point c1 = rodriguesFuntion(i, a, angle);
    point c2 = rodriguesFuntion(j, a, angle);
    point c3 = rodriguesFuntion(k, a, angle);

    homogenousMatrix[0][0] = c1.getX();
    homogenousMatrix[0][1] = c2.getX();
    homogenousMatrix[0][2] = c3.getX();

    homogenousMatrix[1][0] = c1.getY();
    homogenousMatrix[1][1] = c2.getY();
    homogenousMatrix[1][2] = c3.getY();

    homogenousMatrix[2][0] = c1.getZ();
    homogenousMatrix[2][1] = c2.getZ();
    homogenousMatrix[2][2] = c3.getZ();

}

void transformation::genViewRotationMatrix(point l , point r, point u) {

    homogenousMatrix[0][0] = r.getX();
    homogenousMatrix[0][1] = r.getY();
    homogenousMatrix[0][2] = r.getZ();

    homogenousMatrix[1][0] = u.getX();
    homogenousMatrix[1][1] = u.getY();
    homogenousMatrix[1][2] = u.getZ();

    homogenousMatrix[2][0] = -l.getX();
    homogenousMatrix[2][1] = -l.getY();
    homogenousMatrix[2][2] = -l.getZ();

}

void transformation::genProjectionMatrix(double near, double far, double t, double r) {

    homogenousMatrix[0][0] = near/r;
    homogenousMatrix[1][1] = near/t;

    homogenousMatrix[2][2] = -(far+near)/(far-near);
    homogenousMatrix[2][3] = -(2*far*near)/(far-near);
    homogenousMatrix[3][2] = -1;

    homogenousMatrix[3][3] = 0;
}

void transformation::printMatrix() {

    for(int i = 0; i < 4; i++)
    {
       for(int j = 0; j < 4; j++)
       {
           cout<<homogenousMatrix[i][j]<<" ";
       }
       cout<<endl;
    }
    cout<<endl;
}

transformation transformation:: operator*(transformation old_matrix) {

    transformation temp = transformation();

    for(int i = 0; i < 4; i++)
    {
       for(int j = 0; j < 4; j++)
       {
            temp.homogenousMatrix[i][j] = 0;

            for(int k = 0; k < 4; k++)
            {
                temp.homogenousMatrix[i][j] += homogenousMatrix[i][k] * old_matrix.homogenousMatrix[k][j];
            }
       }
    }

    return temp;
}

point transformation:: operator*(point old_point) {

    double newPoint[4] = { 0 };
    double oldPoint[4] = { 0 };

    oldPoint[0] = old_point.getX();
    oldPoint[1] = old_point.getY();
    oldPoint[2] = old_point.getZ();
    oldPoint[3] = weight;

    for(int i = 0; i < 4; i++)
    {
        for(int k = 0; k < 4; k++)
        {
            newPoint[i] += homogenousMatrix[i][k]*oldPoint[k];
        }
    }

    point temp = point(newPoint[0],newPoint[1],newPoint[2]);
    temp.scale(newPoint[3]);

    return temp;
}

int main(){

    srand(time(0));

    cout << std::fixed;
    cout << std::setprecision(7);

    cout<<"Raster Based Pipeline"<<endl;
    cout<<"Student ID : 1705087\n"<<endl;

    string fileName = folder+"/scene.txt";
    inputFile.open(fileName);
    if(!inputFile.is_open()) { exit(EXIT_FAILURE); }

    fileName = "stage1.txt";
    outputFileStage1.open(fileName);
    if(!outputFileStage1.is_open()) { exit(EXIT_FAILURE); }

    fileName = "stage2.txt";
    outputFileStage2.open(fileName);
    if(!outputFileStage2.is_open()) { exit(EXIT_FAILURE); }

    fileName = "stage3.txt";
    outputFileStage3.open(fileName);
    if(!outputFileStage3.is_open()) { exit(EXIT_FAILURE); }

    outputFileStage1 << std::fixed;
    outputFileStage1 << std::setprecision(7);

    outputFileStage2 << std::fixed;
    outputFileStage2 << std::setprecision(7);

    outputFileStage3 << std::fixed;
    outputFileStage3 << std::setprecision(7);


    if(!inputFile.is_open()) { exit(EXIT_FAILURE); }

    transformation identityMatrix = transformation();
    stack<transformation> transformationStack;
    transformationStack.push(identityMatrix);

    stack<stack<transformation>> transformationState;

    string command;

    double eyeX, eyeY, eyeZ;
    double lookX, lookY, lookZ;
    double upX, upY, upZ;
    double fovX, fovY, aspectRatio, near, far;
    double t, r ;

    inputFile >> eyeX >> eyeY >> eyeZ;
    point eyePoint = point(eyeX , eyeY , eyeZ);

    inputFile >> lookX >> lookY >> lookZ;
    point lookPoint = point(lookX , lookY , lookZ);

    inputFile >> upX >> upY >> upZ;
    point upPoint = point(upX , upY , upZ);

    inputFile >> fovY >> aspectRatio >> near >> far;

    if(DEBUG==2) cout << fovY <<" "<< aspectRatio <<" "<< near <<" "<< far<<endl;

    fovX = fovY * aspectRatio;
    t = near * tan(fovY/2*PI/180.0);
    r = near * tan(fovX/2*PI/180.0);

    if(DEBUG==2) cout << fovX <<" "<< t <<" "<< r <<endl;
    if(DEBUG==2) cout <<endl;

    transformation projectionMatrix = transformation();
    projectionMatrix.genProjectionMatrix(near,far,t,r);
    if(DEBUG==2) projectionMatrix.printMatrix();

    point lPoint = lookPoint - eyePoint ;
    lPoint.normalize();

    point rPoint = lPoint&upPoint ;
    rPoint.normalize();

    point uPoint = rPoint&lPoint ;

    transformation viewRotationMatrix = transformation();
    transformation viewTranslateMatrix = transformation();
    transformation viewMatrix = transformation();

    viewRotationMatrix.genViewRotationMatrix(lPoint,rPoint,uPoint);
    viewTranslateMatrix.genTranslationMatrix(-eyePoint.getX(),-eyePoint.getY(),-eyePoint.getZ());
    viewMatrix = viewRotationMatrix*viewTranslateMatrix;

    while (true)
    {
        inputFile >> command;

        if(command.compare("triangle") == 0)
        {
            double a , b , c ;

            triangleCount++;

            if(DEBUG==1) cout << "main triangle ="<<endl;

            for(int i=0; i<3; i++)
            {
                inputFile >> a >> b >> c;
                point p = point(a,b,c);

                point p_1 = transformationStack.top()*p;
                outputFileStage1 << p_1.x <<" "<< p_1.y <<" "<< p_1.z <<endl;

                point p_2 = viewMatrix*p_1;
                outputFileStage2 << p_2.x <<" "<< p_2.y <<" "<< p_2.z <<endl;

                point p_3 = projectionMatrix*p_2;
                outputFileStage3 << p_3.x <<" "<< p_3.y <<" "<< p_3.z <<endl;

                if(DEBUG==1) cout << p.x <<" "<< p.y <<" "<< p.z <<endl;
            }
            if(DEBUG==1) cout <<endl;
            outputFileStage1 <<endl;
            outputFileStage2 <<endl;
            outputFileStage3 <<endl;

        }
        else if(command.compare("translate") == 0)
        {

            transformation translateMatrix = transformation();

            double a , b , c ;

            inputFile >> a >> b >> c;
            translateMatrix.genTranslationMatrix(a,b,c);

            transformationStack.push(transformationStack.top()*translateMatrix);
        }
        else if(command.compare("scale") == 0)
        {
            transformation scaleMatrix = transformation();

            double a , b , c ;

            inputFile >> a >> b >> c;
            scaleMatrix.genScalingMatrix(a,b,c);

            transformationStack.push(transformationStack.top()*scaleMatrix);
        }
        else if(command.compare("rotate") == 0)
        {
            transformation rotationMatrix = transformation();

            double angle , a , b , c ;

            inputFile >> angle >> a >> b >> c;
            rotationMatrix.genRotationMatrix(angle,a,b,c);

            transformationStack.push(transformationStack.top()*rotationMatrix);
        }
        else if(command.compare("push") == 0)
        {
            transformationState.push(transformationStack);
        }
        else if(command.compare("pop") == 0)
        {
            transformationStack = transformationState.top();
            transformationState.pop();
        }
        else if(command.compare("end") == 0)
        {
            break;
        }
    }

    inputFile.close();
    outputFileStage1.close();
    outputFileStage2.close();
    outputFileStage3.close();

    fileName = folder+"/config.txt";
    inputFileConfig.open(fileName);
    if(!inputFileConfig.is_open()) { exit(EXIT_FAILURE); }

    fileName = "stage3.txt";
    inputFileStage3.open(fileName);
    if(!inputFileStage3.is_open()) { exit(EXIT_FAILURE); }

    fileName = "z_buffer.txt";
    outputFileZbuffer.open(fileName);
    if(!outputFileZbuffer.is_open()) { exit(EXIT_FAILURE); }

    outputFileZbuffer << std::fixed;
    outputFileZbuffer << std::setprecision(7);

    int screenWidth, screenHeight;
    double leftLimitX, rightLimitX, bottomLimitY, topLimitY;
    double frontLimitZ, rearLimitZ;

    double dx, dy ;
    double topY, bottomY, leftX, rightX;

    inputFileConfig >> screenWidth >> screenHeight;
    inputFileConfig >> leftLimitX;
    inputFileConfig >> bottomLimitY;
    inputFileConfig >> frontLimitZ >> rearLimitZ;

    rightLimitX = -leftLimitX;
    topLimitY = -bottomLimitY;

    triangle triangleStage3[triangleCount];

    int temp = triangleCount;

    while(temp--)
    {
        point p1,p2,p3;
        double a ,b, c;

        if(DEBUG==3) cout <<"Traingle no = "<< triangleCount-temp <<endl;

        inputFileStage3>>a>>b>>c;
        p1 = point(a,b,c);
        if(DEBUG==3) cout << a <<" "<< b <<" "<< c <<endl;

        inputFileStage3>>a>>b>>c;
        p2 = point(a,b,c);
        if(DEBUG==3) cout << a <<" "<< b <<" "<< c <<endl;

        inputFileStage3>>a>>b>>c;
        p3 = point(a,b,c);
        if(DEBUG==3) cout << a <<" "<< b <<" "<< c <<endl;
        if(DEBUG==3) cout <<endl;

        triangleStage3[triangleCount-temp-1] = triangle(p1,p2,p3);
    }

    dx = (rightLimitX - leftLimitX)/screenWidth;
    dy = (topLimitY - bottomLimitY)/screenHeight;

    topY = topLimitY - dy/2.0;
    leftX = leftLimitX + dx/2.0;

    bottomY = bottomLimitY + dy/2.0;
    rightX = rightLimitX - dx/2.0;

    //double z_buffer[screenHeight][screenWidth] = { rearLimitZ };

    double** z_buffer = new double*[screenHeight];
    for(int i=0; i<screenHeight; i++) {
        z_buffer[i] = new double[screenWidth];
    }

    //color frame_buffer[screenHeight][screenWidth] ;

    color** frame_buffer = new color*[screenHeight];
    for(int i=0; i<screenHeight; i++) {
        frame_buffer[i] = new color[screenWidth];
    }


    //bitmap_image bitMapImage(screenHeight,screenWidth);
    bitmap_image bitMapImage(screenWidth,screenHeight);

    for(int i = 0; i < screenHeight; i++)
    {
       for(int j = 0; j < screenWidth; j++)
       {
           frame_buffer[i][j] = color(0,0,0);
           z_buffer[i][j] = rearLimitZ;
       }
    }

    if(DEBUG==3) cout <<"BLACK color DONE "<<endl;

    temp = triangleCount;

    double max_y , min_y ;
    int current_scan_line , top_scan_line , bottom_scan_line ;
    double y_value;
    point p[3];
    point p1 , p2 , p3 ;
    point pp1 , pp2 , pp3 ;
    double x_left , x_right ,temp_x;
    double z_a , z_b , z_p , constant;
    int current_column , left_intersecting_column , right_intersecting_column ;

    while(temp--)
    {
        //if(DEBUG==3) cout <<"kanekiiiiii"<<endl;
        triangleStage3[triangleCount-temp-1];

        //scan-line without clipping (max , min)

        max_y = max({ triangleStage3[triangleCount-temp-1].getP1().getY(),
                             triangleStage3[triangleCount-temp-1].getP2().getY(),
                             triangleStage3[triangleCount-temp-1].getP3().getY() });

        if(DEBUG==3) cout <<"Traingle no = "<< triangleCount-temp <<" max_y = "<< max_y <<endl;

        min_y = min({ triangleStage3[triangleCount-temp-1].getP1().getY(),
                             triangleStage3[triangleCount-temp-1].getP2().getY(),
                             triangleStage3[triangleCount-temp-1].getP3().getY() });

        if(DEBUG==3) cout <<"Traingle no = "<< triangleCount-temp <<" min_y = "<< min_y <<endl;
        if(DEBUG==3) cout <<endl;

        //scan-line without clipping and Screen map

        if(max_y>=topLimitY) top_scan_line = 0;
        else top_scan_line = (int) round((topY - max_y)/dy);

        if(min_y<=bottomLimitY) bottom_scan_line = screenHeight - 1;
        else bottom_scan_line = (screenHeight - 1 )- (int) round((-bottomY + min_y)/dy);

        if(DEBUG==3) cout <<"top_scan_line = "<< top_scan_line <<" bottom_scan_line = "<< bottom_scan_line <<endl;

        p[0] = triangleStage3[triangleCount-temp-1].getP1();
        p[1] = triangleStage3[triangleCount-temp-1].getP2();
        p[2] = triangleStage3[triangleCount-temp-1].getP3();


        //current_scan_line == row no
        for(int current_scan_line = top_scan_line ; current_scan_line <= bottom_scan_line; current_scan_line++)
        {
            //if(DEBUG==3) cout <<"kanekiiiiii"<<endl;
            y_value = topY - current_scan_line*dy;

            //x_left = INFINITY;
            //x_right = -1*INFINITY;

            x_left = rightLimitX;
            x_right = leftLimitX;

            for(int i=0;i<3;i++)
            {
                int j = (i+1)%3;

                //Not Contains
                if(p[i].getY() < y_value && p[j].getY() < y_value) continue;
                if(p[i].getY() > y_value && p[j].getY() > y_value) continue;
                //Horizontal
                //if(fabs(p[i].getY() - p[j].getY())<1e-6) continue ;
                if(p[i].getY() == p[j].getY()) continue ;

                temp_x = p[i].getX() + (y_value - p[i].getY())*(p[i].getX() - p[j].getX())/(p[i].getY() - p[j].getY());

                if(temp_x>x_right)
                {
                    x_right = temp_x ;
                    z_a = p[i].getZ() + (y_value - p[i].getY())*(p[j].getZ() - p[i].getZ())/(p[j].getY() - p[i].getY());
                }

                if(temp_x<x_left)
                {
                    x_left = temp_x ;
                    z_b = p[i].getZ() + (y_value - p[i].getY())*(p[j].getZ() - p[i].getZ())/(p[j].getY() - p[i].getY());
                }

            }

            if (x_left > x_right) continue;



            //Select 2 intersect point among 3 - same pattern
           /* if(p1.getY()==y_value && p2.getY()==y_value && p3.getY()==y_value)
            {
                x_left = min({p1.getX() , p2.getX() , p3.getX()});
                x_right = max({p1.getX() , p2.getX() , p3.getX()});
                goto skip ;
            }
            else if(p1.getY()==y_value && p2.getY()==y_value )
            {
                x_left = min({p1.getX() , p2.getX() });
                x_right = max({p1.getX() , p2.getX() });
                goto skip ;
            }
            else if(p1.getY()==y_value && p3.getY()==y_value)
            {
                x_left = min({p1.getX() , p3.getX()});
                x_right = max({p1.getX() , p3.getX()});
                goto skip ;
            }
            else if( p2.getY()==y_value && p3.getY()==y_value)
            {
                x_left = min({ p2.getX() , p3.getX()});
                x_right = max({ p2.getX() , p3.getX()});
                goto skip ;
            }

            else if((p1.getY()>y_value && p2.getY()>y_value)||(p1.getY()<y_value && p2.getY()<y_value))
            {
                pp1 = point(p3.getX(),p3.getY(),p3.getZ());

                if(p1.getX()<p2.getX())
                {
                    pp2 = point(p1.getX(),p1.getY(),p1.getZ());
                    pp3 = point(p2.getX(),p2.getY(),p2.getZ());
                }
                else
                {
                    pp2 = point(p2.getX(),p2.getY(),p2.getZ());
                    pp3 = point(p1.getX(),p1.getY(),p1.getZ());
                }
            }
            else if((p1.getY()>y_value && p3.getY()>y_value)||(p1.getY()<y_value && p3.getY()<y_value))
            {
                pp1 = point(p2.getX(),p2.getY(),p2.getZ());

                if(p1.getX()<p3.getX())
                {
                    pp2 = point(p1.getX(),p1.getY(),p1.getZ());
                    pp3 = point(p3.getX(),p3.getY(),p3.getZ());
                }
                else
                {
                    pp2 = point(p3.getX(),p3.getY(),p3.getZ());
                    pp3 = point(p1.getX(),p1.getY(),p1.getZ());
                }
            }
            else if((p3.getY()>y_value && p2.getY()>y_value)||(p3.getY()<y_value && p2.getY()<y_value))
            {
                pp1 = point(p1.getX(),p1.getY(),p1.getZ());

                if(p3.getX()<p2.getX())
                {
                    pp2 = point(p3.getX(),p3.getY(),p3.getZ());
                    pp3 = point(p2.getX(),p2.getY(),p2.getZ());
                }
                else
                {
                    pp2 = point(p2.getX(),p2.getY(),p2.getZ());
                    pp3 = point(p3.getX(),p3.getY(),p3.getZ());
                }
            }

            //finding X range
            x_left = pp1.getX() + (y_value - pp1.getY())*(pp1.getX() - pp2.getX())/(pp1.getY() - pp2.getY());
            x_right = pp1.getX() + (y_value - pp1.getY())*(pp1.getX() - pp3.getX())/(pp1.getY() - pp3.getY());

            skip: */

            //scan-line without clipping and Screen map
            if(x_left <= leftLimitX)
            {
                left_intersecting_column = 0;
                z_a = z_a + (z_a - z_b)*(x_left-leftLimitX)/(leftLimitX-x_right);
            }
            else
            {
                left_intersecting_column = (int) round((-leftX + x_left)/dx);
            }

            if(x_right >= rightLimitX)
            {
                right_intersecting_column = screenWidth-1;
                z_b = z_b + (-z_a + z_b)*(x_right-rightLimitX)/(rightLimitX-x_left);
            }
            else
            {
                right_intersecting_column = (screenWidth-1)-(int) round((rightX - x_right)/dx);
            }
            //z_a = p1.getZ() + (y_value - p1.getY())*(p2.getZ() - p1.getZ())/(p2.getY() - p1.getY());
            //z_b = p1.getZ() + (y_value - p1.getY())*(p2.getZ() - p1.getZ())/(p2.getY() - p1.getY());

            constant = dx*(z_a - z_b)/(x_left-x_right);

            if(DEBUG==3) cout <<"left_intersecting_column = "<< left_intersecting_column <<" right_intersecting_column = "<< right_intersecting_column <<endl;


            for(int current_column = left_intersecting_column ; current_column <= right_intersecting_column; current_column++)
            {

                z_p = z_a + current_column * constant;

                if(z_p<z_buffer[current_scan_line][current_column]&&z_p>frontLimitZ)
                {
                    z_buffer[current_scan_line][current_column] = z_p;
                    frame_buffer[current_scan_line][current_column] = triangleStage3[triangleCount-temp-1].getColor();
                }
            }

        }

        //if(DEBUG==3) cout <<"kanekiiiiii"<<endl;
    }







    for(int i = 0; i < screenHeight; i++)
    {
       for(int j = 0; j < screenWidth; j++)
       {
           if(z_buffer[i][j] < rearLimitZ)
           //if(fabs(z_buffer[i][j] - rearLimitZ)<1e-6)
            {
                outputFileZbuffer << z_buffer[i][j] << '\t';
            }
       }
       outputFileZbuffer << endl;
    }


    for(int row = 0; row < screenHeight; row++)
    {
       for(int col = 0; col < screenWidth; col++)
       {
           bitMapImage.set_pixel(col,row,frame_buffer[row][col].getR(),frame_buffer[row][col].getG(),frame_buffer[row][col].getB());
       }
    }

    bitMapImage.save_image("out.bmp");
    delete[] frame_buffer;
    delete[] z_buffer;

    inputFileConfig.close();
    inputFileStage3.close();
    outputFileZbuffer.close();

    return 0;
}
