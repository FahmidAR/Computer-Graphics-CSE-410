#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <iostream>

#include <GL/glut.h>
#include<vector>

using namespace std ;


#define pi (2*acos(0.0))
#define COLOR_COMBO 0

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double fix_width;
double change_width;
double width;


class point {

	public:

    double x,y,z;

	point() {
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

    point operator+(point vector_point);
    point operator-(point vector_point);
    point operator*(double scalar);
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

point point::operator&(point vector_point) {
    return point(y*vector_point.z-z*vector_point.y, z*vector_point.x-x*vector_point.z, x*vector_point.y-y*vector_point.x);
}

class camera {
	point pos;
	point u,r,l;

	double per_change_distance ;
	double per_change_angle ;

	public:
	camera() {
    }	

    camera(point pos,point u,point r,point l) {
        this->pos = pos;
        this->u = u;
        this->r = r;
		this->l = l;

		this->per_change_distance = 2;
		this->per_change_angle = 2;
    }	

	void setPcDis(double per_change_distance)
	{
		this->per_change_distance = per_change_distance;
	}

	void setPcAngle(double per_change_angle)
	{
		this->per_change_angle = per_change_angle;
	}

	point getCamPos() {
		return pos;
	}

	point getCamL() {
		return l;
	}

	point getCamR() {
		return r;
	}

	point getCamU() {
		return u;
	}

	void moveUp();
	void moveDown();

	void moveForward();
	void moveBackward();

	void moveLeft();
	void moveRight();

	void rotateUp();
	void rotateDown();

	void rotateLeft();
	void rotateRight();

	void tiltClockwise();
	void tiltCounterClockwise();
	
};

void camera::moveUp() {
	pos = pos + u * per_change_distance ;
}

void camera::moveDown() {
  	pos = pos - u * per_change_distance ;
}

void camera::moveForward() {
	pos = pos + l * per_change_distance ;
}

void camera::moveBackward() {
  	pos = pos - l * per_change_distance ;
}

void camera::moveLeft() {
	pos = pos - r * per_change_distance ;
}

void camera::moveRight() {
  	pos = pos + r * per_change_distance ;
}

void camera::rotateUp() {
  	u = u * cos(per_change_angle * pi / 180) + (r&u) * sin(per_change_angle * pi / 180);
	l = l * cos(per_change_angle * pi / 180) + (r&l) * sin(per_change_angle * pi / 180);
}

void camera::rotateDown() {
  	u = u * cos(-per_change_angle * pi / 180) + (r&u) * sin(-per_change_angle * pi / 180);
	l = l * cos(-per_change_angle * pi / 180) + (r&l) * sin(-per_change_angle * pi / 180);
}

void camera::rotateLeft() {
  	l = l * cos(per_change_angle * pi / 180) + (u&l) * sin(per_change_angle * pi / 180);
	r = r * cos(per_change_angle * pi / 180) + (u&r) * sin(per_change_angle * pi / 180);
}

void camera::rotateRight() {
	l = l * cos(-per_change_angle * pi / 180) + (u&l) * sin(-per_change_angle * pi / 180);
	r = r * cos(-per_change_angle * pi / 180) + (u&r) * sin(-per_change_angle * pi / 180);
}

void camera::tiltClockwise() {
  	r = r * cos(per_change_angle * pi / 180) + (l&r) * sin(per_change_angle * pi / 180);
	u = u * cos(per_change_angle * pi / 180) + (l&u) * sin(per_change_angle * pi / 180);
}

void camera::tiltCounterClockwise() {
  	r = r * cos(-per_change_angle * pi / 180) - (l&r) * sin(-per_change_angle * pi / 180);
	u = u * cos(-per_change_angle * pi / 180) + (l&u) * sin(-per_change_angle * pi / 180);
}

point pos ;
point u ;
point r ;
point l ;
camera c;

void drawAxes()
{
	if(drawaxes==1)
	{
		glBegin(GL_LINES);{

			glColor3f(1.0, 1.0, 0.0); //Yellow - x
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glColor3f(1.0, 0.0, 1.0); //Violet - y
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glColor3f(0.0, 1.0, 1.0); // Sky - z
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
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
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
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


void draw1by8Sphere(int slices,int stacks)
{
	//glColor3f(1,0,0); //Red
	glTranslatef(width,width,width);

	point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=(fix_width - width)*sin(((double)i/(double)stacks)*(pi/2));
		r=(fix_width - width)*cos(((double)i/(double)stacks)*(pi/2));

		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(pi/2));
			points[i][j].y=r*sin(((double)j/(double)slices)*(pi/2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
           
			}glEnd();
		}
	}
}

void draw8Sphere(int slices,int stacks)
{

	glColor3f(1,0,0); //Red

	glPushMatrix();
    {
        //glTranslatef(fix_width,0,0);
		//glRotatef(90,0,1,0);
		draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(90,1,0,0);
		draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,fix_width,0);
        glRotatef(180,1,0,0);
        draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,-fix_width,0);
        glRotatef(-90,1,0,0);
        draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,0,fix_width);
        glRotatef(-90,0,1,0);
        draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(-180,0,1,0);
		draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,fix_width,0);
        glRotatef(180,0,0,1);
        draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,-fix_width,0);
		glRotatef(90,1,0,0);
        glRotatef(180,0,0,1);;
        draw1by8Sphere(slices,stacks);
    }
    glPopMatrix();

}

void draw1by4Cylinder(int slices,int stacks)
{
	//glColor3f(0,1,0); // Green
	glTranslatef(width,width,0);

	point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=width*sin(((double)i/(double)stacks)*(pi/2));
		r=(fix_width - width);

		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(pi/2));
			points[i][j].y=r*sin(((double)j/(double)slices)*(pi/2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void draw4Cylinder(int slices,int stacks)
{
	if(COLOR_COMBO==1)
	{
		glColor3f(0,0,0); // White
	}
	else
	{
		glColor3f(0,1,0); // Green
	}

	glPushMatrix();
    {
        //glTranslatef(fix_width,0,0);
		//glRotatef(90,0,1,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(90,1,0,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(-90,0,0,1);
		glRotatef(90,1,0,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(-180,0,0,1);
		glRotatef(90,1,0,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(-90,0,0,1);
		glRotatef(-90,1,0,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(-180,0,0,1);
		glRotatef(-90,1,0,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,fix_width,0);
        glRotatef(180,1,0,0);
        draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,-fix_width,0);
        glRotatef(-90,1,0,0);
        draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(-fix_width,0,0);
		glRotatef(90,0,1,0);
		draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,fix_width,0);
        glRotatef(180,0,1,0);
        draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,-fix_width,0);
        glRotatef(-90,0,1,0);
        draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

	glPushMatrix();
    {
        //glTranslatef(0,-fix_width,0);
        glRotatef(-180,0,0,1);
        draw1by4Cylinder(slices,stacks);
    }
    glPopMatrix();

}

void drawCube()
{
	if(COLOR_COMBO==1)
	{
		glColor3f(1,1,0); // White
	}
	else
	{
		glColor3f(1,1,1); // Yellow
	}
	

	glPushMatrix();
    {
        glTranslatef(fix_width,0,0);
		glRotatef(90,0,1,0);
		drawSquare(width);
    }
    glPopMatrix();

	

	glPushMatrix();
    {
        glTranslatef(-fix_width,0,0);
		glRotatef(90,0,1,0);
		drawSquare(width);
    }
    glPopMatrix();

	glPushMatrix();
    {
        glTranslatef(0,fix_width,0);
        glRotatef(90,1,0,0);
        drawSquare(width);
    }
    glPopMatrix();

	glPushMatrix();
    {
        glTranslatef(0,-fix_width,0);
        glRotatef(90,1,0,0);
        drawSquare(width);
    }
    glPopMatrix();

	glPushMatrix();
    {
        glTranslatef(0,0,fix_width);
        //glRotatef(0,0,0,1);
        drawSquare(width);
    }
    glPopMatrix();

	glPushMatrix();
    {
        glTranslatef(0,0,-fix_width);
        //glRotatef(0,0,0,1);
        drawSquare(width);
    }
    glPopMatrix();

}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			c.rotateLeft();
			break;

		case '2': 
			c.rotateRight();
			break;

		case '3': 
			c.rotateUp();
			break;
		case '4': 
			c.rotateDown();
			break;

		case '5':
			c.tiltClockwise();
			break;
		case '6':
			c.tiltCounterClockwise();
			break;

		case '7':
			drawgrid=1-drawgrid;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:     
			c.moveBackward();
			break;
		case GLUT_KEY_UP:        
			c.moveForward();
			break;

		case GLUT_KEY_RIGHT:
			c.moveRight();
			break;
		case GLUT_KEY_LEFT:
			c.moveLeft();
			break;

		case GLUT_KEY_PAGE_UP:
			c.moveUp();
			break;
		case GLUT_KEY_PAGE_DOWN:
			c.moveDown();
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

	gluLookAt(c.getCamPos().getX(),c.getCamPos().getY(),c.getCamPos().getZ(),
			c.getCamPos().getX()+c.getCamL().getX() ,c.getCamPos().getY()+c.getCamL().getY(),c.getCamPos().getZ()+c.getCamL().getZ(),
			c.getCamU().getX(),c.getCamU().getY(),c.getCamU().getZ());

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

	drawCube();

	//double change_radius = sqrt(width*width+fix_width*fix_width);

	draw8Sphere(24,20);

	draw4Cylinder(24,20);


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	pos = point(100.0, 100.0, 0.0);
	u = point(0.0, 0.0, 1.0);
	r = point(-1.0/sqrt(2.0), 1.0/sqrt(2.0), 0.0);
	l = point(-1.0/sqrt(2.0), -1.0/sqrt(2.0), 0.0);

	c = camera(pos,u,r,l);

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
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){

	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("1705087 OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
