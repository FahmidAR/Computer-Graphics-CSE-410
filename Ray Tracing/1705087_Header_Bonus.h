#define PI (2*acos(0.0))
#define COLOR_COMBO 0

#define SLICES 20
#define STACKS 20

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define GRID_LENGTH 1000
#define FOVY 80

#define DEBUG 0
#define DEBUG_CUR 1

int objectsCount;
int lightsCount;
int pointlightsCount;
int spotlightsCount;

class pointVector {

	public:

    double x,y,z;

	pointVector() {
        x = y = z = 0.0;
    }

    pointVector(double x, double y, double z) {
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

    pointVector operator+(pointVector vector_point);
    pointVector operator-(pointVector vector_point);
    pointVector operator*(double scalar);
    double      operator/(pointVector vector_point);
    pointVector operator&(pointVector vector_point);

    void normalize();
    double distanceBetween(pointVector vector_point);

};

void pointVector::normalize() {
    double length = sqrt(x*x + y*y + z*z);
    x /= length;
    y /= length;
    z /= length;
}

double pointVector::distanceBetween(pointVector vector_point){
    return sqrt(pow(x - vector_point.getX(), 2) + pow(y - vector_point.getY(), 2) + pow(z - vector_point.getZ(), 2));
}


pointVector pointVector::operator+(pointVector vector_point) {
    return pointVector(x+vector_point.x, y+vector_point.y, z+vector_point.z);
}

pointVector pointVector::operator-(pointVector vector_point) {
    return pointVector(x-vector_point.x, y-vector_point.y, z-vector_point.z);
}

pointVector pointVector::operator*(double scalar) {
    return pointVector(x*scalar, y*scalar, z*scalar);
}

double pointVector::operator/(pointVector vector_point) {
    return x*vector_point.x+y*vector_point.y+z*vector_point.z;
}

pointVector pointVector::operator&(pointVector vector_point) {
    return pointVector(y*vector_point.z-z*vector_point.y, z*vector_point.x-x*vector_point.z, x*vector_point.y-y*vector_point.x);
}

class camera {
	pointVector pos;
	pointVector u,r,l;

	double per_change_distance ;
	double per_change_angle ;

	public:
	camera() {
    }	

    camera(pointVector pos,pointVector u,pointVector r,pointVector l) {
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

	pointVector getCamPos() {
		return pos;
	}

	pointVector getCamL() {
		return l;
	}

	pointVector getCamR() {
		return r;
	}

	pointVector getCamU() {
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
  	u = u * cos(per_change_angle * PI / 180) + (r&u) * sin(per_change_angle * PI / 180);
	l = l * cos(per_change_angle * PI / 180) + (r&l) * sin(per_change_angle * PI / 180);
}

void camera::rotateDown() {
  	u = u * cos(-per_change_angle * PI / 180) + (r&u) * sin(-per_change_angle * PI / 180);
	l = l * cos(-per_change_angle * PI / 180) + (r&l) * sin(-per_change_angle * PI / 180);
}

void camera::rotateLeft() {
  	l = l * cos(per_change_angle * PI / 180) + (u&l) * sin(per_change_angle * PI / 180);
	r = r * cos(per_change_angle * PI / 180) + (u&r) * sin(per_change_angle * PI / 180);
}

void camera::rotateRight() {
	l = l * cos(-per_change_angle * PI / 180) + (u&l) * sin(-per_change_angle * PI / 180);
	r = r * cos(-per_change_angle * PI / 180) + (u&r) * sin(-per_change_angle * PI / 180);
}

void camera::tiltClockwise() {
  	r = r * cos(per_change_angle * PI / 180) + (l&r) * sin(per_change_angle * PI / 180);
	u = u * cos(per_change_angle * PI / 180) + (l&u) * sin(per_change_angle * PI / 180);
}

void camera::tiltCounterClockwise() {
  	r = r * cos(-per_change_angle * PI / 180) - (l&r) * sin(-per_change_angle * PI / 180);
	u = u * cos(-per_change_angle * PI / 180) + (l&u) * sin(-per_change_angle * PI / 180);
}

camera cam;

class Ray
{
    
    pointVector origin;
    pointVector direction;

    public:

    Ray() {
    }
    Ray(pointVector origin, pointVector direction) {
        this->origin = origin;
        this->direction = direction;
    }

    pointVector getOrigin() {
        return origin;
    }
    pointVector getDirection() {
        return direction;
    }

    void setOrigin(pointVector origin) {
        this->origin = origin;
    }

    void setDirection(pointVector direction) {
        this->direction = direction;
    }

    pointVector getPoint(double t) {
        return origin + direction * t;
    }

    pointVector getPoint(double t, pointVector light_pos) {
        return origin + direction * t + (light_pos - origin) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal, pointVector light_refraction_reflection) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal, pointVector light_refraction_reflection, pointVector light_refraction_reflection_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal, pointVector light_refraction_reflection, pointVector light_refraction_reflection_normal, pointVector light_refraction_reflection_normal_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal, pointVector light_refraction_reflection, pointVector light_refraction_reflection_normal, pointVector light_refraction_reflection_normal_normal, pointVector light_refraction_reflection_normal_normal_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal, pointVector light_refraction_reflection, pointVector light_refraction_reflection_normal, pointVector light_refraction_reflection_normal_normal, pointVector light_refraction_reflection_normal_normal_normal, pointVector light_refraction_reflection_normal_normal_normal_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal_normal_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

    pointVector getPoint(double t, pointVector light_pos, pointVector light_dir, pointVector light_normal, pointVector light_reflection, pointVector light_refraction, pointVector light_refraction_normal, pointVector light_refraction_reflection, pointVector light_refraction_reflection_normal, pointVector light_refraction_reflection_normal_normal, pointVector light_refraction_reflection_normal_normal_normal, pointVector light_refraction_reflection_normal_normal_normal_normal, pointVector light_refraction_reflection_normal_normal_normal_normal_normal) {
        return origin + direction * t + (light_pos - origin) * (1/t) + (light_dir - direction) * (1/t) + (light_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal_normal_normal - (light_pos - origin) * (1/t)) * (1/t) + (light_refraction_reflection_normal_normal_normal_normal_normal - (light_pos - origin) * (1/t)) * (1/t);
    }

};

// Color class 

class Color {

    public:

    double red;
    double green;
    double blue;

    Color() {
        red = 0;
        green = 0;
        blue = 0;
    }

    Color(double r, double g, double b) {
        red = r;
        green = g;
        blue = b;
    }

    Color operator+(Color c) {
        return Color(red + c.red, green + c.green, blue + c.blue);
    }

    Color operator-(Color c) {
        return Color(red - c.red, green - c.green, blue - c.blue);
    }

    Color operator*(double d) {
        return Color(red * d, green * d, blue * d);
    }

    Color operator/(double d) {
        return Color(red / d, green / d, blue / d);
    }

    Color operator*(Color c) {
        return Color(red * c.red, green * c.green, blue * c.blue);
    }

    Color operator/(Color c) {
        return Color(red / c.red, green / c.green, blue / c.blue);
    }

    void clip() {
        if (red > 1) red = 1;
        if (red < 0) red = 0;
        if (green > 1) green = 1;
        if (green < 0) green = 0;
        if (blue > 1) blue = 1;
        if (blue < 0) blue = 0;
    }

    void setColor(double r, double g, double b) {
        red = r;
        green = g;
        blue = b;
    }

    void setColor(Color c) {
        red = c.red;
        green = c.green;
        blue = c.blue;
    }

    void setRed(double r) {
        red = r;
    }

    void setGreen(double g) {
        green = g;
    }

    void setBlue(double b) {
        blue = b;
    }

    double getRed() {
        return red;
    }

    double getGreen() {
        return green;
    }

    double getBlue() {
        return blue;
    }

};

class Light
{
    public:

    pointVector position;
    Color color;

    Light()
    {
        position = pointVector(0, 0, 0);
        color = Color(0, 0, 0);
    }

    Light(pointVector p, Color c) {
        position = p;
        color = c;
    }

    pointVector getPosition() {
        return position;
    }

    Color getColor() {
        return color;
    }

    void setPosition(pointVector p) {
        position = p;
    }

    void setColor(Color c) {
        color = c;
    }

    void setPosition(double x, double y, double z) {
        position.setX(x);
        position.setY(y);
        position.setZ(z);
    }

    void setColor(double r, double g, double b) {
        color.setRed(r);
        color.setGreen(g);
        color.setBlue(b);
    }

    virtual void draw() = 0;

};

class PointLight: public Light {

    public:

    PointLight() {
        position = pointVector(0, 0, 0);
        color = Color(1, 1, 1);
    }
    PointLight(pointVector p, Color c) : Light(p, c) {
    }

    void draw();

};

void PointLight::draw() {
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(color.getRed(), color.getGreen(), color.getBlue());
    glVertex3f(position.getX(), position.getY(), position.getZ());
    glEnd();
    glPointSize(1);
}

class SpotLight: public Light {

    public:

    pointVector direction;
    double angle;

    SpotLight()
    {
        direction = pointVector(0, 0, 0);
        angle = 0;
    }
    SpotLight(pointVector p, Color c, pointVector d, double a) : Light(p, c) {
        direction = d;
        angle = a;
    }

    pointVector getDirection() {
        return direction;
    }

    double getAngle() {
        return angle;
    }

    void draw();

};

void SpotLight::draw() {
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(color.getRed(), color.getGreen(), color.getBlue());
    glVertex3f(position.getX(), position.getY(), position.getZ());
    glEnd();
    glPointSize(1);
}




class Object{

    public:

    Color color;

    pointVector reference_point;

    double height;
    double width;
    double length;

    double coEfficients[4];

    int shine;

    Object()
    {
        shine=0;
    }

    Object(pointVector p, double h, double w, double l, Color c, double* co, int s) {
        reference_point = p;
        height = h;
        width = w;
        length = l;
        color = c;
        for(int i = 0; i < 4; i++) {
            coEfficients[i] = co[i];
        }
        shine = s;
    }

    pointVector getReferencePoint() {
        return reference_point;
    }

    Color getColor() {
        return color;
    }

    double getHeight() {
        return height;
    }

    double getWidth() {
        return width;
    }

    double getLength() {
        return length;
    }

    double* getCoEfficients() {
        return coEfficients;
    }

    int getShine() {
        return shine;
    }

    void setReferencePoint(pointVector p) {
        reference_point = p;
    }

    void setColor(Color c) {
        color = c;
    }

    void setHeightWidthLength(double h, double w, double l) {
        height = h;
        width = w;
        length = l;
    }

    void setHeight(double h) {
        height = h;
    }

    void setWidth(double w) {
        width = w;
    }

    void setLength(double l) {
        length = l;
    }

    void setShine(int s) {
        shine = s;
    }

    void setColor(double r, double g, double b) {
        color.setRed(r);
        color.setGreen(g);
        color.setBlue(b);
    }

    void setCoEfficients(double a, double b, double c, double d) {
        coEfficients[0] = a;
        coEfficients[1] = b;
        coEfficients[2] = c;
        coEfficients[3] = d;
    }

    virtual void draw() = 0;
    virtual double intersect(Ray ray, Color& color, int level) = 0;

};

pointVector position;
int recursionLevel = 0;
vector<Object*> objects;
vector<PointLight*> pointlights;
vector<SpotLight*> spotlights;

//draw share class
class Sphere: public Object {

    double radius;
    pointVector center;

    public:

    Sphere() {
        radius = 0;
    }

    Sphere(double r, pointVector c) {
        radius = r;
        center = c;
    }

    void draw();
    double intersect(Ray ray, Color& color, int level); 
};

void Sphere::draw() {
    glPushMatrix();
    glTranslatef(center.getX(), center.getY(), center.getZ());
    glColor3f(color.getRed(), color.getGreen(), color.getBlue());
    glutSolidSphere(radius, SLICES , STACKS);
    glPopMatrix();
}

double Sphere::intersect(Ray ray, Color& color, int level) {
    //ray.getDirection().normalize();
    pointVector ray_origin = (ray.getOrigin() - center);
    //ray_origin.normalize();
    double a = ray.getDirection()/ray.getDirection();
    double b = (ray.getDirection()/ray_origin )* 2.0 ;
    double c = ray_origin/ray_origin - radius * radius;

    double discriminant = b * b - 4 * a * c;
    
    if(discriminant < 0) {
        return -1;
    }

    double t1 = (-b - sqrt(discriminant)) / (2 * a);
    double t2 = (-b + sqrt(discriminant)) / (2 * a);

    if(t1 < 0 && t2 < 0) {
        return -1;
    }
    else if(t1 < 0) {
        return t2;
    }
    else if(t2 < 0) {
        return t1;
    }

    if(level == 0) {
       return min(t1, t2);
    }

    double t = min(t1, t2);

    //ambient lighting

    color.setColor(this->color);
    color = color*coEfficients[0];

    //other lighting

    pointVector intersectionPoint = ray.getOrigin()+ray.getDirection()*t;

    pointVector normal ;

    if(cam.getCamPos().distanceBetween(center) > radius) {
        normal = intersectionPoint-center;
    }
    else {
        normal = center-intersectionPoint;
    }

    normal.normalize();

    if(DEBUG) {
        cout << "ambient: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
    }


    for(int i = 0; i < pointlights.size(); i++) {

        pointVector light_direction = intersectionPoint-pointlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(pointlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = pointlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << pointlights[i]->getPosition().getX() << " " << pointlights[i]->getPosition().getY() << " " << pointlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(pointlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(pointlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = pointlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);
            reflected_ray.normalize();
            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);

            color = color +this->color*pointlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color +this->color*pointlights[i]->color*coEfficients[2]*phongValue;
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }

    }

    for(int i = 0; i < spotlights.size(); i++) {

        pointVector light_direction = intersectionPoint-spotlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(spotlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = spotlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << spotlights[i]->getPosition().getX() << " " << spotlights[i]->getPosition().getY() << " " << spotlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(spotlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(spotlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normspotal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;

        double alpha = spotlights[i]->getAngle();
        double beta = acos(light_direction/spotlights[i]->getDirection())*(180/PI);

        if(beta > alpha) {
            inShadow = true;
        }
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = spotlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);
            reflected_ray.normalize();
            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);

            color = color + this->color*spotlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color + this->color*spotlights[i]->color*coEfficients[2]*phongValue;
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }
    }

    if(level < recursionLevel) {
        
    double epsilon = 0.0000001;

    pointVector intersect_reflection_dir = ray.getDirection()-normal*((ray.getDirection()/normal)*2.0);
    intersect_reflection_dir.normalize();
    
    //Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir, intersect_reflection_dir);
    Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir*epsilon, intersect_reflection_dir);

    int nearest = -1;
    double t, tMinimum=INFINITY;

    for(int i=0; i<objects.size(); i++) {
        Color colorTemp;  
        t = objects[i]->intersect(intersect_reflected_ray, colorTemp, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    Color colorReflected;

    if(nearest != -1) {
        tMinimum = objects[nearest]->intersect(intersect_reflected_ray, colorReflected, level+1);
    }

    color = color + colorReflected*coEfficients[3];

    }

    color.clip();

    return t;
}

class Triangle: public Object {

    pointVector p1;
    pointVector p2;
    pointVector p3;

    public:

    Triangle() {
        p1 = pointVector(0, 0, 0);
        p2 = pointVector(0, 0, 0);
        p3 = pointVector(0, 0, 0);
    }

    Triangle(pointVector p1, pointVector p2, pointVector p3, Color c) {
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
        color = c;
    }

    void draw();
    double intersect(Ray ray, Color& color, int level);
};

void Triangle::draw() {
    glBegin(GL_TRIANGLES);
    glColor3f(color.getRed(), color.getGreen(), color.getBlue());
    glVertex3f(p1.getX(), p1.getY(), p1.getZ());
    glVertex3f(p2.getX(), p2.getY(), p2.getZ());
    glVertex3f(p3.getX(), p3.getY(), p3.getZ());
    glEnd();
}

//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

double Triangle::intersect(Ray ray, Color& color, int level) {

    double tMin = INFINITY;

    double epsilon = 0.0000001;

    pointVector edge1 = p2 - p1;
    pointVector edge2 = p3 - p1;

    pointVector normal = edge1&edge2;
    normal.normalize();

    if(ray.getDirection()/normal < 0) {
        normal = normal*(-1);
    }

    pointVector h = ray.getDirection()&edge2;
    double a = edge1/h;

    if(a > -epsilon && a < epsilon) {
        return -1;
    }

    double f = 1.0/a;
    pointVector s = ray.getOrigin() - p1;

    double u = f*(s/h);
    if(u < 0.0 || u > 1.0) {
        return -1;
    }
    
    pointVector q = s&edge1;

    double v = f*(ray.getDirection()/q);
    if(v < 0.0 || u+v > 1.0) {
        return -1;
    }

    double t = f*(edge2/q);

    if(t > epsilon) {
        
        tMin = t;
    }
    else {
        return -1;
    }

    if(level==0)
    {
        return tMin;
    }

    //ambient lighting

    color.setColor(this->color);
    color = color*coEfficients[0];

    //other lighting

    pointVector intersectionPoint = ray.getOrigin()+ray.getDirection()*t;

    if(DEBUG) {
        cout << "ambient: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
    }


    for(int i = 0; i < pointlights.size(); i++) {

        pointVector light_direction = intersectionPoint-pointlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(pointlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = pointlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << pointlights[i]->getPosition().getX() << " " << pointlights[i]->getPosition().getY() << " " << pointlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(pointlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(pointlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = pointlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);

            double etaRed = 1.3;
            double etaBlue = 1.5;
            double etaGreen = 1.7;

            double n_dot_i = (light_direction/normal);

            double kRed = 1.0 - etaRed * etaRed * (1.0 - n_dot_i * n_dot_i);
            double kBlue = 1.0 - etaBlue * etaBlue * (1.0 - n_dot_i * n_dot_i);
            double kGreen = 1.0 - etaGreen * etaGreen * (1.0 - n_dot_i * n_dot_i);

            pointVector refracted_ray_Red ;
            pointVector refracted_ray_Blue ;
            pointVector refracted_ray_Green ;

            if (kRed < 0.0)
            {
                refracted_ray_Red = pointVector(0, 0, 0);
            }
                
            else
            {
                refracted_ray_Red = light_direction*etaRed - normal*(etaRed * n_dot_i + sqrt(kRed));
            }

            if (kGreen < 0.0)
            {
                refracted_ray_Green = pointVector(0, 0, 0);
            }
                
            else
            {
                refracted_ray_Green = light_direction*etaGreen - normal*(etaGreen * n_dot_i + sqrt(kGreen));
            }

            if (kBlue < 0.0)
            {
                refracted_ray_Blue = pointVector(0, 0, 0);
            }
                
            else
            {
                refracted_ray_Blue = light_direction*etaBlue - normal*(etaBlue * n_dot_i + sqrt(kBlue));
            }

            reflected_ray.normalize();
            refracted_ray_Red.normalize();
            refracted_ray_Green.normalize();
            refracted_ray_Blue.normalize();

            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;
            double phongValueRed = inverse_ray/refracted_ray_Red;
            double phongValueGreen = inverse_ray/refracted_ray_Green;
            double phongValueBlue = inverse_ray/refracted_ray_Blue;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);
            phongValueRed = max(phongValueRed, 0.0);
            phongValueGreen = max(phongValueGreen, 0.0);
            phongValueBlue = max(phongValueBlue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);
            phongValueRed = pow(phongValueRed , shine);
            phongValueGreen = pow(phongValueGreen , shine);
            phongValueBlue = pow(phongValueBlue , shine);

            color = color +this->color*pointlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color +this->color*pointlights[i]->color*coEfficients[2]*phongValue;
            //color.setRed(color.getRed() + this->color.getRed()*pointlights[i]->color.getRed()*coEfficients[3]*phongValueRed);
            //color.setGreen(color.getGreen() + this->color.getGreen()*pointlights[i]->color.getGreen()*coEfficients[3]*phongValueGreen);
            //color.setBlue(color.getBlue() + this->color.getBlue()*pointlights[i]->color.getBlue()*coEfficients[3]*phongValueBlue);

            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }

    }

    for(int i = 0; i < spotlights.size(); i++) {

        pointVector light_direction = intersectionPoint-spotlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(spotlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = spotlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << spotlights[i]->getPosition().getX() << " " << spotlights[i]->getPosition().getY() << " " << spotlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(spotlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(spotlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normspotal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;

        double alpha = spotlights[i]->getAngle();
        double beta = acos(light_direction/spotlights[i]->getDirection())*(180/PI);

        if(beta > alpha) {
            inShadow = true;
        }
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = spotlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);

             double etaRed = 0.3;
            double etaBlue = 2.9;
            double etaGreen = 1.7;

            double n_dot_i = (light_direction/normal);

            double kRed = 1.0 - etaRed * etaRed * (1.0 - n_dot_i * n_dot_i);
            double kBlue = 1.0 - etaBlue * etaBlue * (1.0 - n_dot_i * n_dot_i);
            double kGreen = 1.0 - etaGreen * etaGreen * (1.0 - n_dot_i * n_dot_i);

            pointVector refracted_ray_Red ;
            pointVector refracted_ray_Blue ;
            pointVector refracted_ray_Green ;

            if (kRed < 0.0)
            {
                refracted_ray_Red = pointVector(0, 0, 0);
            }
                
            else
            {
                refracted_ray_Red = light_direction*etaRed - normal*(etaRed * n_dot_i + sqrt(kRed));
            }

            if (kGreen < 0.0)
            {
                refracted_ray_Green = pointVector(0, 0, 0);
            }
                
            else
            {
                refracted_ray_Green = light_direction*etaGreen - normal*(etaGreen * n_dot_i + sqrt(kGreen));
            }

            if (kBlue < 0.0)
            {
                refracted_ray_Blue = pointVector(0, 0, 0);
            }
                
            else
            {
                refracted_ray_Blue = light_direction*etaBlue - normal*(etaBlue * n_dot_i + sqrt(kBlue));
            }

            double lambertValue = refracted_ray_Blue/normal;
            //double lambertValue = inv_light_direction/normal;

            reflected_ray.normalize();
            refracted_ray_Red.normalize();
            refracted_ray_Green.normalize();
            refracted_ray_Blue.normalize();

            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;
            double phongValueRed = inverse_ray/refracted_ray_Red;
            double phongValueGreen = inverse_ray/refracted_ray_Green;
            double phongValueBlue = inverse_ray/refracted_ray_Blue;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);
            phongValueRed = max(phongValueRed, 0.0);
            phongValueGreen = max(phongValueGreen, 0.0);
            phongValueBlue = max(phongValueBlue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);
            phongValueRed = pow(phongValueRed , shine);
            phongValueGreen = pow(phongValueGreen , shine);
            phongValueBlue = pow(phongValueBlue , shine);

            color = color + this->color*spotlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            //color = color + this->color*spotlights[i]->color*coEfficients[2]*phongValue;
            color.setRed(color.getRed() + this->color.getRed()*pointlights[i]->color.getRed()*coEfficients[3]*phongValueRed);
            color.setGreen(color.getGreen() + this->color.getGreen()*pointlights[i]->color.getGreen()*coEfficients[3]*phongValueGreen);
            color.setBlue(color.getBlue() + this->color.getBlue()*pointlights[i]->color.getBlue()*coEfficients[3]*phongValueBlue);
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }
    }

    if(level < recursionLevel) {
        
    double epsilon = 0.0000001;

    pointVector intersect_reflection_dir = ray.getDirection()-normal*((ray.getDirection()/normal)*2.0);
    intersect_reflection_dir.normalize();
    
    //Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir, intersect_reflection_dir);
    Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir*epsilon, intersect_reflection_dir);

    int nearest = -1;
    double t, tMinimum=INFINITY;

    for(int i=0; i<objects.size(); i++) {
        Color colorTemp;  
        t = objects[i]->intersect(intersect_reflected_ray, colorTemp, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    Color colorReflected;

    if(nearest != -1) {
        tMinimum = objects[nearest]->intersect(intersect_reflected_ray, colorReflected, level+1);
    }

    color = color + colorReflected*coEfficients[3];

    }

    color.clip();

    return tMin;
}

//GeneralQuadricSurfaceCoefficient class
class GeneralQuadricSurfaces: public Object{

    double a, b, c, d, e, f, g, h, i, j;

    public:

    GeneralQuadricSurfaces() {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
        e = 0;
        f = 0;
        g = 0;
        h = 0;
        i = 0;
        j = 0;
    }

    GeneralQuadricSurfaces(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
        this->g = g;
        this->h = h;
        this->i = i;
        this->j = j;

        if(DEBUG)
        {
            cout<<"a: "<<a<<" b: "<<b<<" c: "<<c<<" d: "<<d<<" e: "<<e<<" f: "<<f<<" g: "<<g<<" h: "<<h<<" i: "<<i<<" j: "<<j<<endl;
        }
    
    }

    void draw();
    double intersect(Ray ray, Color& color, int level);
    bool isNotInside(pointVector point);

};

void GeneralQuadricSurfaces::draw() {
  
}

bool GeneralQuadricSurfaces::isNotInside(pointVector point) {

    //return false;

    if(length!=0.0)
    {
        if(point.getX()<reference_point.getX()||point.getX()>(reference_point.getX()+length))
        {
            return true;
        }
    }

    if(width!=0.0)
    {
        if(point.getY()<reference_point.getY()||point.getY()>(reference_point.getY()+width))
        {
            return true;
        }
    }
    if(height!=0.0)
    {
        if(point.getZ()<reference_point.getZ()||point.getZ()>(reference_point.getZ()+height))
        {
            return true;
        }
    
    }
    return false;

}

//http://skuld.bmsc.washington.edu/people/merritt/graphics/quadrics.html

double GeneralQuadricSurfaces::intersect(Ray ray, Color& color, int level) {

    double tMax,tMin = INFINITY;

    //cout<<"a "<<a <<" b "<<b<<" c "<<c<<" d "<<d<<" e "<<e<<" f "<<f<<" g "<<g<<" h "<<h<<" i "<<i<<" j "<<j<<endl;

    pointVector rd = ray.getDirection();
    //rd.normalize();

    double epsilon = 0.0000001;

    double xd = rd.getX();
    double yd = rd.getY();
    double zd = rd.getZ();

    //ray.setOrigin(ray.getOrigin()+rd*epsilon);
    
    double xo = ray.getOrigin().getX();
    double yo = ray.getOrigin().getY();
    double zo = ray.getOrigin().getZ();

    if(DEBUG)
    {
        cout<< "xd: "<<xd<<" yd: "<<yd<<" zd: "<<zd<<" xo: "<<xo<<" yo: "<<yo<<" zo: "<<zo<<endl;
    }

    //Aq = Axd2 + Byd2 + Czd2 + Dxdyd + Exdzd + Fydzd

    double aq = a*xd*xd + b*yd*yd + c*zd*zd + d*xd*yd + e*xd*zd + f*yd*zd;

    // double aq = ray.getDirection().getX()*2*a + ray.getDirection().getY()*2*b + ray.getDirection().getZ()*2*c
    //             + ray.getDirection().getX()*ray.getDirection().getY()*d 
    //             + ray.getDirection().getX()*ray.getDirection().getZ()*e 
    //             + ray.getDirection().getY()*ray.getDirection().getZ()*f;

    //Bq = 2*Axoxd + 2*Byoyd + 2*Czozd + D(xoyd + yoxd) + E(xozd + zoxd) + F(yozd + ydzo) + Gxd + Hyd + Izd

    double bq = 2*a*xo*xd + 2*b*yo*yd + 2*c*zo*zd + d*(xo*yd + yo*xd) + e*(xo*zd + zo*xd) + f*(yo*zd + yd*zo) + g*xd + h*yd + i*zd;

    // double bq = ray.getOrigin().getX()*ray.getDirection().getX()*a*2+ ray.getOrigin().getY()*ray.getDirection().getY()*2*b + ray.getOrigin().getZ()*ray.getDirection().getZ()*2*c 
    //             + (ray.getOrigin().getX()*ray.getDirection().getY()+ray.getDirection().getY()*ray.getOrigin().getX())*d 
    //             + (ray.getOrigin().getY()*ray.getDirection().getZ()+ray.getDirection().getZ()*ray.getOrigin().getY())*e
    //             + (ray.getOrigin().getZ()*ray.getDirection().getX()+ray.getDirection().getX()*ray.getOrigin().getZ())*f;
    //             + ray.getDirection().getX()*g + ray.getDirection().getY()*h + ray.getDirection().getZ()*i;

    //Cq = Axo2 + Byo2 + Czo2 + Dxoyo + Exozo + Fyozo + Gxo + Hyo + Izo + J 

    double cq = a*xo*xo + b*yo*yo + c*zo*zo + d*xo*yo + e*xo*zo + f*yo*zo + g*xo + h*yo + i*zo + j; 

    // double cq = ray.getOrigin().getX()*ray.getOrigin().getX()*a+ray.getOrigin().getY()*ray.getOrigin().getY()*b+ray.getOrigin().getZ()*ray.getOrigin().getZ()*c
    //             + ray.getOrigin().getX()*ray.getOrigin().getY()*d + ray.getOrigin().getX()*ray.getOrigin().getZ()*e + ray.getOrigin().getY()*ray.getOrigin().getZ()*f
    //             + ray.getOrigin().getX()*g + ray.getOrigin().getY()*h + ray.getOrigin().getZ()*i + j; 

    if(DEBUG)
    {
        cout<<"aq : "<<aq<<" bq : "<<bq<<" cq : "<<cq<<endl;
    }

    if(aq == 0.0) {
        if(bq == 0.0) {
            tMax = INFINITY;
        } else {
            tMax = -cq / bq;
        }
        tMax = INFINITY;
    } else {

        if((bq*bq-4.0*aq*cq) < 0.0) {
            tMin = tMax = INFINITY;
        } else if((bq*bq-4.0*aq*cq) > 0.0) {
            tMax = (-bq+sqrt(bq*bq-4*aq*cq))/(2*aq);
            tMin = (-bq-sqrt(bq*bq-4*aq*cq))/(2*aq);
        } else {
            tMin = -bq/(2.0*aq);
            tMax = INFINITY;
        }
    }

    pointVector intersectionPointOne = ray.getOrigin()+rd*tMin;
    pointVector intersectionPointTwo = ray.getOrigin()+rd*tMax;

    if(tMin < INFINITY) {

        if(tMin > 0.0) {

                if(isNotInside(intersectionPointOne)) {
                    tMin = INFINITY;
                }
            }

        if(tMax < INFINITY) {
            
            if(tMax > 0.0) {

                if(isNotInside(intersectionPointTwo)) {
                    tMax = INFINITY;
                }
            }

            if(tMin>0.0)
            {
                tMin = min(tMin , tMax);
            }
 
        } 
    }

    if(level == 0) {
        return tMin;
    }

   // cout<<" tMin : "<<tMin<<endl;

    //ambient lighting
    color.setColor(this->color);
    color = color*coEfficients[0];

    pointVector intersectionPoint = ray.getOrigin()+rd*tMin;

    double xNormal = 2.0*a*intersectionPoint.getX()+d*intersectionPoint.getY();
                 + e*intersectionPoint.getZ()+g;

    double yNormal = 2.0*b*intersectionPoint.getY()+d*intersectionPoint.getX();
                  + f*intersectionPoint.getZ()+h;

    double zNormal = 2.0*c*intersectionPoint.getZ()+e*intersectionPoint.getX();
                  + f*intersectionPoint.getY()+i;

    pointVector normal(xNormal , yNormal , zNormal);

    if(rd/normal > 0) {
        normal = normal*(-1);
    }

    for(int i = 0; i < pointlights.size(); i++) {

        pointVector light_direction = intersectionPoint-pointlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(pointlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = pointlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << pointlights[i]->getPosition().getX() << " " << pointlights[i]->getPosition().getY() << " " << pointlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(pointlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(pointlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = pointlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);
            reflected_ray.normalize();
            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);

            color = color +this->color*pointlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color +this->color*pointlights[i]->color*coEfficients[2]*phongValue;
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }

    }

    for(int i = 0; i < spotlights.size(); i++) {

        pointVector light_direction = intersectionPoint-spotlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(spotlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = spotlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << spotlights[i]->getPosition().getX() << " " << spotlights[i]->getPosition().getY() << " " << spotlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(spotlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(spotlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normspotal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;

        double alpha = spotlights[i]->getAngle();
        double beta = acos(light_direction/spotlights[i]->getDirection())*(180/PI);

        if(beta > alpha) {
            inShadow = true;
        }
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = spotlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);
            reflected_ray.normalize();
            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);

            color = color + this->color*spotlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color + this->color*spotlights[i]->color*coEfficients[2]*phongValue;
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }
    }

    if(level < recursionLevel) {
        
    double epsilon = 0.0000001;

    pointVector intersect_reflection_dir = ray.getDirection()-normal*((ray.getDirection()/normal)*2.0);
    intersect_reflection_dir.normalize();
    
    //Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir, intersect_reflection_dir);
    Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir*epsilon, intersect_reflection_dir);

    int nearest = -1;
    double t, tMinimum=INFINITY;

    for(int i=0; i<objects.size(); i++) {
        Color colorTemp;  
        t = objects[i]->intersect(intersect_reflected_ray, colorTemp, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    Color colorReflected;

    if(nearest != -1) {
        tMinimum = objects[nearest]->intersect(intersect_reflected_ray, colorReflected, level+1);
    }

    color = color + colorReflected*coEfficients[3];

    }

    color.clip();

    return tMin;
    
}

class Floor: public Object {

    double totalWidth;
    double miniWidth;

    Color colorOne;
    Color colorTwo;

    public:
    
    Floor() {
        totalWidth = 0;
         miniWidth = 0;
    }

    Floor(double totalWidth, double miniWidth, Color c1) {
        this->totalWidth = totalWidth;
        this->miniWidth = miniWidth;
        colorOne = c1;
        Color c2 = Color(1-c1.getRed(), 1-c1.getGreen(), 1-c1.getBlue());
        colorTwo = c2;
    }

    void draw();
    double intersect(Ray ray, Color& color, int level);

}; 

void Floor::draw() {

    int miniBlockNo = round(totalWidth / miniWidth);

    double x,y,z;

    pointVector leftBottomCorner(-totalWidth/2.0, -totalWidth/2.0, 0.0);

    for (int i = 0; i <  miniBlockNo; i++)
    {
        for (int j = 0; j <  miniBlockNo; j++)
        {
            glBegin(GL_QUADS);
            {
                if ((i + j) % 2 == 0)
                    glColor3f(colorOne.getRed(), colorOne.getGreen(), colorOne.getBlue());
                else
                    glColor3f(colorTwo.getRed(), colorTwo.getGreen(), colorTwo.getBlue());

                x=leftBottomCorner.getX() + i * miniWidth;
                y=leftBottomCorner.getY() + j * miniWidth;
                z=leftBottomCorner.getZ();

                glVertex3f(x, y, z);
                glVertex3f(x+miniWidth, y,z);
                glVertex3f(x+miniWidth, y+miniWidth, z);
                glVertex3f(x, y+miniWidth, z);
            }
            glEnd();
        }
    }
}

/* https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes
/ray-plane-and-ray-disk-intersection?fbclid=IwAR3mK7zuzN5zD-fZHPQNqag1AFYoEfKh4cIQNG-XSbDlXahS4_TkYmbs4aM */

double Floor::intersect(Ray ray, Color& color, int level) {
    
    pointVector normal(0.0, 0.0, 1.0);
    normal.normalize();

    if(normal/ray.getDirection() > 0) {
        normal = normal * (-1);
    }
        
    

    if(DEBUG) {
        cout<<"normal: "<<normal.getX()<<" "<<normal.getY()<<" "<<normal.getZ()<<endl;
    }

    /* finding intersecting tMin */
    double tMin = INFINITY;
    //normal.normalize();
    pointVector floorMID = pointVector(0.0,0.0, 0.0);

    double denominator = normal/ray.getDirection();
    

    if(denominator != 0.0) {
        //tMin = (-1.0)*(normal/ray.getOrigin()  )/denominator;
        tMin = (normal/(floorMID-ray.getOrigin())  )/denominator;
    }

    pointVector intersectionPoint;

    if(tMin>0.0 && tMin<INFINITY) {


        intersectionPoint = ray.getOrigin()+ray.getDirection()*tMin;

        if(DEBUG) {
            //cout << "tMin: " << tMin << endl;
            cout << "intersectionPoint: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
        }

        //out of bound x axix
        if((intersectionPoint.getX()<-totalWidth/2.0 && intersectionPoint.getX()>totalWidth/2.0) ) {
            
            tMin = INFINITY;
        }

        //out of bound y axix

        if(intersectionPoint.getY()<totalWidth/2.0 && intersectionPoint.getY()>totalWidth/2.0) {
            tMin = INFINITY;
        }
    }

    if(level == 0) {
        return tMin;
    }
    if(DEBUG) {
        cout << "tMin: " << tMin << endl;
    }

    double t = tMin;

    //ambient lighting
    //intersectionPoint = ray.getOrigin()+ray.getDirection()*tMin;
    floorMID = pointVector(-totalWidth/2.0, -totalWidth/2.0, 0.0);
    pointVector diff = intersectionPoint - floorMID;

    if(((int)diff.getX()/(int)miniWidth)%2 ==  ((int)diff.getY()/(int)miniWidth)%2 ) {
        color = colorOne;
    }
    else {
        color = colorTwo;
    }

    Color nowColor = color;

    color = color*coEfficients[0];
    //color = color*0.1;

    //color.setColor(1,0,0);

    if(DEBUG) {
        cout << "ambient: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
    }


    for(int i = 0; i < pointlights.size(); i++) {

        pointVector light_direction = intersectionPoint-pointlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(pointlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = pointlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << pointlights[i]->getPosition().getX() << " " << pointlights[i]->getPosition().getY() << " " << pointlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(pointlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(pointlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = pointlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);
            reflected_ray.normalize();
            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);

            color = color + nowColor*pointlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color + nowColor*pointlights[i]->color*coEfficients[2]*phongValue;
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }
    }

    for(int i = 0; i < spotlights.size(); i++) {

        pointVector light_direction = intersectionPoint-spotlights[i]->getPosition();
        light_direction.normalize();

        Ray light_ray(spotlights[i]->getPosition(), light_direction);

        bool inShadow = false;

        double tMinn = INFINITY;
        double epsilon = 0.0000001;

        Color tempColor;

        for (int k = 0; k < objects.size(); k++) {
                Color color;
                double tt = objects[k]->intersect(light_ray, color, 0);

                //cout<<"1"<<endl;

                if (tt > 0 && tt < tMinn) {
                    //cout<<"2"<<endl;
                    tMinn = tt;
                }
        }

        pointVector shadowIntersectionPoint = spotlights[i]->getPosition()+light_direction*tMinn;

        if(DEBUG) {
            cout << "shadow: " << shadowIntersectionPoint.getX() << " " << shadowIntersectionPoint.getY() << " " << shadowIntersectionPoint.getZ() << endl;
            cout << "light: " << spotlights[i]->getPosition().getX() << " " << spotlights[i]->getPosition().getY() << " " << spotlights[i]->getPosition().getZ() << endl;
            cout << "intersection: " << intersectionPoint.getX() << " " << intersectionPoint.getY() << " " << intersectionPoint.getZ() << endl;
            cout << "light direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
        }

        if(intersectionPoint.distanceBetween(spotlights[i]->getPosition())-epsilon > 
            shadowIntersectionPoint.distanceBetween(spotlights[i]->getPosition())) {
            inShadow = true;

            if(DEBUG) {
                cout << "tMin: " << tMinn << endl;
            }
        }

        //cout<< "normal: " << normspotal.getX() << " " << normal.getY() << " " << normal.getZ() << endl;

        double alpha = spotlights[i]->getAngle();
        double beta = acos(light_direction/spotlights[i]->getDirection())*(180/PI);

        if(beta > alpha) {
            inShadow = true;
        }
        

        if(!inShadow) {
           // if(true) {

            //pointVector light_direction =  intersectionPoint - pointlights[i]->getPosition();
            pointVector inv_light_direction = spotlights[i]->getPosition() - intersectionPoint;
            inv_light_direction.normalize();
            double lambertValue = inv_light_direction/normal;

            //light_direction = light_direction*-1;
            pointVector reflected_ray = light_direction -  normal*((light_direction/normal)*2);
            reflected_ray.normalize();
            pointVector inverse_ray = ray.getDirection()*(-1.0);
            inverse_ray.normalize();
            double phongValue = inverse_ray/reflected_ray;

            lambertValue = max(lambertValue, 0.0);
            phongValue = max(phongValue, 0.0);

            if(DEBUG) {
                //cout << "light_direction: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "incident: " << light_direction.getX() << " " << light_direction.getY() << " " << light_direction.getZ() << endl;
                cout << "reflected: " << reflected_ray.getX() << " " << reflected_ray.getY() << " " << reflected_ray.getZ() << endl;
                cout << "lambert: " << lambertValue << endl;
                cout << "phong: " << phongValue << endl;
            }

            phongValue = pow(phongValue , shine);

            color = color + nowColor*spotlights[i]->color*coEfficients[1]*lambertValue;
            if(DEBUG) {
                cout << "d color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }
            color = color + nowColor*spotlights[i]->color*coEfficients[2]*phongValue;
            if(DEBUG) {
                cout << "s color: " << color.getRed() << " " << color.getGreen() << " " << color.getBlue() << endl;
            }

            }
    }

    if(level < recursionLevel) {
        
    double epsilon = 0.0000001;

    pointVector intersect_reflection_dir = ray.getDirection()-normal*((ray.getDirection()/normal)*2.0);
    intersect_reflection_dir.normalize();
    
    //Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir, intersect_reflection_dir);
    Ray intersect_reflected_ray(intersectionPoint+intersect_reflection_dir*epsilon, intersect_reflection_dir);

    int nearest = -1;
    double t, tMinimum=INFINITY;

    for(int i=0; i<objects.size(); i++) {
        Color colorTemp;  
        t = objects[i]->intersect(intersect_reflected_ray, colorTemp, 0);

        if(t>0.0 && t<tMinimum) {
            tMinimum = t;
            nearest = i;
        }
    }

    Color colorReflected;

    if(nearest != -1) {
        tMinimum = objects[nearest]->intersect(intersect_reflected_ray, colorReflected, level+1);
    }

    color = color + colorReflected*coEfficients[3];

    }


    color.clip();

    return t;
}