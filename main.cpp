#include "TS.h"
static Car *cars;
 /**The last update time*/
static long lastUpdateTime;
static bool isTrafficLightRed=false;
static int test=10;
void myDisplay(void)
{
    int i;
    float x[N],y[N];
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    //Draw car s
    glColor3f(1.0,1.0,1.0);

    for(i=0;i<config.n;i++)
    {
        if(cars[i].x>=0 && cars[i].x-config.carLength<=config.windowWidth)
        {
            glRectf(-config.windowWidth/2+cars[i].x,-config.carWidth/2,
                 -config.windowWidth/2+cars[i].x-config.carLength,config.carWidth/2);
        }
    }
    //draw the road (by two lines)
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex2f( -config.windowWidth/2, -config.roadWidth/2);
    glVertex2f( config.windowWidth/2, -config.roadWidth/2);
    glVertex2f( -config.windowWidth/2, config.roadWidth/2);
    glVertex2f( config.windowWidth/2, config.roadWidth/2);
    glEnd();
    //draw the two traffic lights
    if(isTrafficLightRed==true)
    {
        glColor3f(1,0,0);
    }
    else
    {
        glColor3f(0,1,0);
    }
    glBegin(GL_LINE_LOOP);
    drawCircle(config.trafficLightPosition-config.windowWidth/2,config.roadWidth,10,true);
    glEnd();

    glPopMatrix();
    glutSwapBuffers();
}

/** Here update the position of cars*/
void refresh(void)
{
    int i;
    float deltaT,deltaV,sStar,deltaS,tmp,a1,a2;
    long currentTime=clock();

    if(cars[config.n-1].x-config.carLength>config.windowWidth)
    {
        glutIdleFunc(NULL);
        return;
    }

    deltaT=(currentTime-lastUpdateTime)/1000.0;

    /**Update positions and velocities
    (based on the acceleration of the last time)*/
    for(i=config.n-1;i>=0;i--)
    {
        tmp=(cars[i].v+cars[i].a*deltaT/2)*deltaT;
        if(tmp>0)
            cars[i].x+=tmp;
        cars[i].v+= cars[i].a*deltaT;
        if(cars[i].v<0)
            cars[i].v=0;
    }

/**Update acceleration*/
    for(i=config.n-1;i>0;i--)
    {
        deltaS=cars[i-1].x-cars[i].x-config.carLength;
        deltaV=cars[i].v-cars[i-1].v;
        sStar=config.s0+cars[i].v*config.T+(cars[i].v*deltaV/(2*sqrt(config.a*config.b)));
        a1= config.a*(1-pow(cars[i].v/config.v0,4)-pow(sStar/deltaS,2));
        /**Juge whether the traffic light is valide for current car.
         The driver take the red light as the car in front.*/
         if(isTrafficLightRed==true && cars[i].x<config.trafficLightPosition)
        {
            deltaS=config.trafficLightPosition-cars[i].x;
            deltaV=cars[i].v;
            sStar=config.s0+cars[i].v*config.T+(cars[i].v*deltaV/(2*sqrt(config.a*config.b)));
            a2= config.a*(1-pow(cars[i].v/config.v0,4)-pow(sStar/deltaS,2));

            if(a1>a2)
                cars[i].a=a2;
            else
                cars[i].a=a1;
        }
        else
        {
            cars[i].a=a1;
        }

    }
    //i==0
    a1= config.a*(1-pow(cars[0].v/config.v0,4));
    if(isTrafficLightRed==true && cars[0].x<config.trafficLightPosition)
    {
        deltaS=config.trafficLightPosition-cars[0].x;
        deltaV=cars[0].v;
        sStar=config.s0+cars[0].v*config.T+(cars[0].v*deltaV/(2*sqrt(config.a*config.b)));
        a2= config.a*(1-pow(cars[0].v/config.v0,4)-pow(sStar/deltaS,2));
        if(a1>a2)
            cars[0].a=a2;
        else
            cars[0].a=a1;
    }
    else
    {
        cars[0].a=a1;
    }

    lastUpdateTime=currentTime;
    glutPostRedisplay();
}

void myReshape(GLsizei w, GLsizei h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500,500,-250,250,-1,1);


//    if(w<=h)
//        gluOrtho2D(-1.0,1.5,-1.5,1.5*(GLfloat)h/(GLfloat)w);
//    else
//        gluOrtho2D(-1.0,1.5*(GLfloat)w/(GLfloat)h,-1.5,1.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//    gluOrtho2D(-1.5,0.0,-1.5,1.5);
}

void mouse(int button, int state, int x, int y)
{
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(state==GLUT_UP)
            {
                //Sleep(1000);
                glutIdleFunc(refresh);
            }

            break;
        case GLUT_MIDDLE_BUTTON:
             if(state==GLUT_UP)
            {
                printf("x=%f;a=%f;v=%f;\n",cars[0].x,cars[0].a,cars[0].v);
            }

            break;
        case GLUT_RIGHT_BUTTON:
           if(state==GLUT_UP)
            {
                if(isTrafficLightRed==false)
                    isTrafficLightRed=true;
                else
                    isTrafficLightRed=false;
                glutPostRedisplay();
            }
        default:
            break;
    }
}
void timer_func(int value)
{
    if(value>0)
    {
        value --;
        printf("I'm a timer !");
    }
}
int main(int argc, char** argv)
{

    initConfiguration(&config);
    cars=(Car*)malloc(sizeof(Car)*config.n);
    initCars(cars,config);

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB);
    glutInitWindowSize( config.windowWidth,config.windowHeight);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Traffic Simulation");

    glClearColor(25.0/255,134.0/255,19.0/255,0.3);
    glShadeModel(GL_SMOOTH);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(myDisplay);
    glutMouseFunc(mouse);

    glutTimerFunc(3000,timer_func,5);
    glutTimerFunc(10000,timer_func,5);

    lastUpdateTime=clock();
    glutMainLoop();

}