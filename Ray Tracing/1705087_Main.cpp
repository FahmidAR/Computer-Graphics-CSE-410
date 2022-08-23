#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include<cstdlib>
#include<vector>
#include<limits>
#include <climits>

#include<GL/glut.h>
#include "bitmap_image.hpp"

using namespace std;

#include "1705087_Header.h"

extern int recursionLevel;

int imagePixelDimension = 0;
int imageHeight = 0;
int imageWidth = 0;



extern int objectsCount;
extern int lightsCount;
extern int pointlightsCount;
extern int spotlightsCount;

int bmpImageCount = 0;

string objectType;

extern vector<Object*> objects;
extern vector<PointLight*> pointlights;
extern vector<SpotLight*> spotlights;

int bitmapImageCount;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double fix_width;
double change_width;
double width;

pointVector pos ;
pointVector u ;
pointVector r ;
pointVector l ;
extern camera cam;

void drawAxes()
{
	if(drawaxes==1)
	{
		glBegin(GL_LINES);{

			glColor3f(1.0, 1.0, 0.0); //Yellow - x
			glVertex3f( GRID_LENGTH,0,0);
			glVertex3f(-GRID_LENGTH,0,0);

			glColor3f(1.0, 0.0, 1.0); //Violet - y
			glVertex3f(0,-GRID_LENGTH,0);
			glVertex3f(0, GRID_LENGTH,0);

			glColor3f(0.0, 1.0, 1.0); // Sky - z
			glVertex3f(0,0, GRID_LENGTH);
			glVertex3f(0,0,-GRID_LENGTH);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct pointVector points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
    }
}

void capture() {

    cout << "3 2 1 .. Capturing ... ^_^" << endl;

    bitmap_image bitmapImage(imageWidth, imageHeight);
    
    bitmapImage.set_all_channels(0, 0, 0);
    //bitmapImage.set_all_channels(255, 255, 255);

    double du = (double) WINDOW_WIDTH / (double)imageWidth;
    double dv = (double) WINDOW_HEIGHT / (double)imageHeight;

    double planeDistance = WINDOW_HEIGHT/(2.0*tan(FOVY/2.0*PI/180.0));
    
	pointVector topLeft = cam.getCamPos() +cam.getCamL()*planeDistance-cam.getCamR()*(WINDOW_WIDTH/2.0)+cam.getCamU()*(WINDOW_HEIGHT/2.0);
    topLeft = topLeft + cam.getCamR()*(0.5*du) - cam.getCamU()*(0.5*dv);

    if(DEBUG)
    {
        cout << "topLeft: " << topLeft.x << " " << topLeft.y << " " << topLeft.z << endl;
        cout << "du: " << du << " " << "dv: " << dv << endl;
        cout << "planeDistance: " << planeDistance << endl;
    }

        
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {

            double tMin = INFINITY;
            Object *nearest = NULL;
            int nearestIndex = -1;

            //pointVector curPixel = topLeft + r*(0.5*du) - u*(0.5*dv);
            pointVector curPixel = topLeft + cam.getCamR()*(i*du) - cam.getCamU()*(j*dv);
            pointVector rayDir = curPixel-cam.getCamPos();
            rayDir.normalize();
            Ray ray(cam.getCamPos(),rayDir );

            for (int k = 0; k < objects.size() ; k++) {
                Color color;
                double t = objects[k]->intersect(ray, color, 0);

                //cout<<"1"<<endl;

                if (t > 0 && t < tMin) {
                    //cout<<"2"<<endl;
                    tMin = t;
                    nearest = objects[k];
                    nearestIndex = k;
                }
            }

            //if (nearest != NULL) {
            if(nearestIndex != -1){
                //cout<<"Meu"<<endl;
                Color color;
                tMin = nearest->intersect(ray, color,1);
                //if(tMin<INFINITY&&tMin>0){
                    //cout<<"3"<<endl;
                    
                

                color = color * 255.0;
                bitmapImage.set_pixel(i, j,(int)color.getRed(), (int)color.getGreen(), (int)color.getBlue());
                
            }
        }
    }

    bmpImageCount++;
    bitmapImage.save_image(to_string(bmpImageCount) + ".bmp");
    cout << "-- Captured ["+to_string(bmpImageCount)+"] no bmp image successfully :D" << endl;

}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

        case '0':
            capture();
            break;
		case '1':
			cam.rotateLeft();
			break;

		case '2': 
			cam.rotateRight();
			break;

		case '3': 
			cam.rotateUp();
			break;
		case '4': 
			cam.rotateDown();
			break;

		case '5':
			cam.tiltClockwise();
			break;
		case '6':
			cam.tiltCounterClockwise();
			break;

		case '7':
			drawgrid=1-drawgrid;
            drawaxes=1-drawaxes;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:     
			cam.moveBackward();
			break;
		case GLUT_KEY_UP:        
			cam.moveForward();
			break;

		case GLUT_KEY_RIGHT:
			cam.moveRight();
			break;
		case GLUT_KEY_LEFT:
			cam.moveLeft();
			break;

		case GLUT_KEY_PAGE_UP:
			cam.moveUp();
			break;
		case GLUT_KEY_PAGE_DOWN:
			cam.moveDown();
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			if(width>0) width -= change_width;		
			break;

		case GLUT_KEY_END:
			if(width<fix_width) width += change_width;
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(COLOR_COMBO==1)
	{
		glClearColor(1,1,1,0.5);	//color white
	}
	else
	{
		glClearColor(0,0,0,0.5);	//color Black
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//This give three info
	//1. where is the camera (viewer)
	//2. where is the camera looking
	//3. Which direction is the camera's UP direction

	gluLookAt(cam.getCamPos().getX(),cam.getCamPos().getY(),cam.getCamPos().getZ(),
			cam.getCamPos().getX()+cam.getCamL().getX() ,cam.getCamPos().getY()+cam.getCamL().getY(),cam.getCamPos().getZ()+cam.getCamL().getZ(),
			cam.getCamU().getX(),cam.getCamU().getY(),cam.getCamU().getZ());

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
    //glutSolidSphere(50, 20 , 20);
	//drawGrid();

	for(int i=0; i<objects.size(); i++) {
        objects[i]->draw();
	}

    for(int i=0; i<pointlights.size(); i++) {
        pointlights[i]->draw();
    }

    for(int i=0; i<spotlights.size(); i++) {
        spotlights[i]->draw();
    }


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){

    objectsCount=0;
    lightsCount=0;
    pointlightsCount=0;
    spotlightsCount=0;

	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	pos = pointVector(100.0, 100.0, 0.0);
	u = pointVector(0.0, 0.0, 1.0);
	r = pointVector(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0.0);
	l = pointVector(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0.0);

	cam = camera(pos,u,r,l);

	width = 25 ;
	fix_width = 50 ;
	change_width = 1 ;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(FOVY,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

void loadData() {

    ifstream inputFile;

    inputFile.open("scene.txt");

    if(!inputFile) {
        cout << "Error: File not found" << endl;
        exit(1);
    }
    if(!inputFile.is_open()) {
        cout << "Error: File can not Open" << endl;
        exit(1);
    }

    inputFile >> recursionLevel >> imagePixelDimension;
    //imagePixelDimension=500;
    imageHeight = imagePixelDimension;
	imageWidth = imagePixelDimension;

    inputFile >> objectsCount;

    Object* object = NULL;
    double x,y,z;
    double ambient, diffuse, specular, recursiveReflectionCoefficient;
    double shine;

    for(int i=0; i<objectsCount; i++) {
        inputFile >> objectType;

        if(objectType.compare("sphere") == 0) 
        {
            pointVector center;
            double radius;
            Color colorTemp;

            inputFile >> x >> y >> z;
            center = pointVector(x,y,z);

            inputFile >> radius;

            inputFile >> x >> y >> z;
            colorTemp = Color(x,y,z);


            inputFile >> ambient >> diffuse >> specular >> recursiveReflectionCoefficient;
            inputFile >> shine;

            object = new Sphere( radius, center );
            object->setColor(colorTemp);

            object->setCoEfficients(ambient, diffuse, specular, recursiveReflectionCoefficient);
            object->setShine(shine);

        } 
        else if(objectType.compare("triangle") == 0) {
            pointVector p1,p2,p3;
            Color colorTemp;

            inputFile >> x >> y >> z;
            p1 = pointVector(x,y,z);
            inputFile >> x >> y >> z;
            p2 = pointVector(x,y,z);
            inputFile >> x >> y >> z;
            p3 = pointVector(x,y,z);

            inputFile >> x >> y >> z;
            colorTemp = Color(x,y,z);

            inputFile >> ambient >> diffuse >> specular >> recursiveReflectionCoefficient;
            inputFile >> shine;

            object = new Triangle( p1, p2, p3 , colorTemp);

            object->setCoEfficients(ambient, diffuse, specular, recursiveReflectionCoefficient);
            object->setShine(shine);

        } 
        else if(objectType.compare("general") == 0) {
            double a, b, c, d, e, f, g, h, i, j;
            
            Color colorTemp;

            double length;
            double width;
            double height;
            pointVector cubeReferencePoint;

            inputFile >> a>>b>>c>>d>>e>>f>>g>>h>>i>>j;
            inputFile >> x >> y >> z >> length >> width >> height;
            cubeReferencePoint = pointVector(x,y,z);

            object = new GeneralQuadricSurfaces(a,b,c,d,e,f,g,h,i,j);

            object->setReferencePoint(cubeReferencePoint);
            object->setLength(length);
            object->setWidth(width);
            object->setHeight(height);

            inputFile >> x >> y >> z;
            colorTemp = Color(x,y,z);

            object->setColor(colorTemp);

            inputFile >> ambient >> diffuse >> specular >> recursiveReflectionCoefficient;
            inputFile >> shine;

            object->setCoEfficients(ambient, diffuse, specular, recursiveReflectionCoefficient);
            object->setShine(shine);
        }

        objects.push_back(object);
    }

    object = new Floor(1000.0, 20.0, Color());
    object->setCoEfficients(0.2,0.2,0.2,0.2);
    object->setShine(15);

    objects.push_back(object);

    object = NULL;

    PointLight* pointlight = NULL;
    SpotLight* spotlight = NULL;

    inputFile >> pointlightsCount;

    for(int i=0; i<pointlightsCount; i++) {
        pointVector position;
        Color color;

        inputFile >> x >> y >> z;
        position = pointVector(x,y,z);

        inputFile >> x >> y >> z;
        color = Color(x,y,z);

        pointlight = new PointLight(position, color);

        pointlights.push_back(pointlight);
    }

    inputFile >> spotlightsCount;

    for(int i=0; i<spotlightsCount; i++) {
        pointVector position;
        Color color;
        pointVector direction;
        double angle;

        inputFile >> x >> y >> z;
        position = pointVector(x,y,z);

        inputFile >> x >> y >> z;
        color = Color(x,y,z);

        inputFile >> x >> y >> z;
        direction = pointVector(x,y,z);

        inputFile >> angle;
        spotlight = new SpotLight(position, color, direction, angle);

        spotlights.push_back(spotlight);
    }

    lightsCount = pointlightsCount + spotlightsCount;

    inputFile.close();

    object = NULL;
    pointlight = NULL;
    spotlight = NULL;

}

void clearObjects() {
    for(int i=0; i<objects.size(); i++) {
        delete objects[i];
    }
    objects.clear();
}

void clearPointLights() {
    for(int i=0; i<pointlights.size(); i++) {
        delete pointlights[i];
    }
    pointlights.clear();
}

void clearSpotLights() {
    for(int i=0; i<spotlights.size(); i++) {
        delete spotlights[i];
    }
    spotlights.clear();
}

int main(int argc, char **argv){

	glutInit(&argc,argv);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("1705087 Ray Tracing with OpenGL");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

    if((atexit(clearPointLights) != 0)) {
        cout << "MEMORY MANAGEMENT failed for CLear Point Lights" << endl;
        exit(EXIT_FAILURE);
	}

    if((atexit(clearSpotLights) != 0)) {
        cout << "MEMORY MANAGEMENT failed for CLear Spot Lights" << endl;
        exit(EXIT_FAILURE);
	}

    if((atexit(clearObjects) != 0)) {
        cout << "MEMORY MANAGEMENT failed for CLear Objects" << endl;
        exit(EXIT_FAILURE);
	}

    loadData();

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}

