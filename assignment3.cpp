// anim.cpp version 5.0 -- Template code for drawing an articulated figure.  CS 174A.
#include "../CS174a template/Utilities.h"
#include "../CS174a template/Shapes.h"

std::stack<mat4> mvstack;

int g_width = 800, g_height = 800 ;
float zoom = 1 ;

int animate = 0, recording = 0, basis_to_display = -1;
double TIME = 0;

const unsigned X = 0, Y = 1, Z = 2;

vec4 eye( 0, 40, 70, 1), ref( 0, 0, 0, 1 ), up( 0, 1, 0, 0 );	// The eye point and look-at point.

mat4	orientation, model_view;
ShapeData cubeData, sphereData, coneData, cylData, hexagonData, starData, diamondData;
// Structs that hold the Vertex Array Object index and number of vertices of each shape.

GLuint	texture_none, texture_autumn, 
         texture_grass, texture_diamond, texture_sky;
GLint   uModelView, uProjection, uView,
		uAmbient, uDiffuse, uSpecular, uLightPos, uShininess,
		uTex, uEnableTex;

void init()
{
#if defined(EMSCRIPTEN) 
    GLuint program = LoadShaders( "vshader.glsl", "fshader.glsl" );
    // Load shaders and use the resulting shader program
    TgaImage autumnImage("autumn.tga");
    TgaImage grassImage("grass.tga");
    TgaImage diamondImage("diamond.tga");
    TgaImage skyImage("sky.tga");

#else
	GLuint program = LoadShaders( "../my code/vshader.glsl", "../my code/fshader.glsl" );
    // Load shaders and use the resulting shader program
    TgaImage autumnImage ("../my code/autumn.tga");
    TgaImage grassImage("../my code/grass.tga");
    TgaImage diamondImage("../my code/diamond.tga");
    TgaImage skyImage("../my code/sky.tga");
    
#endif
    glUseProgram(program);

	generateCube(program, &cubeData);		// Generate vertex arrays for geometric shapes
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);
    generateHexagon(program, &hexagonData);
    generateStar(program, &starData);
    generateDiamond(program, &diamondData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView		= glGetUniformLocation( program, "View"       );
    uAmbient	= glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse	= glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular	= glGetUniformLocation( program, "SpecularProduct" );
    uLightPos	= glGetUniformLocation( program, "LightPosition"   );
    uShininess	= glGetUniformLocation( program, "Shininess"       );
    uTex		= glGetUniformLocation( program, "Tex"             );
    uEnableTex	= glGetUniformLocation( program, "EnableTex"       );

    glUniform4f( uAmbient,    0.2,  0.2,  0.2, 1 );
    glUniform4f( uDiffuse,    0.6,  0.6,  0.6, 1 );
    glUniform4f( uSpecular,   0.2,  0.2,  0.2, 1 );
    glUniform4f( uLightPos,  15.0, 15.0, 30.0, 0 );
    glUniform1f( uShininess, 100);

    glEnable(GL_DEPTH_TEST);
    
    glGenTextures( 1, &texture_autumn );
    glBindTexture( GL_TEXTURE_2D, texture_autumn );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, autumnImage.width, autumnImage.height, 0,
                 (autumnImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, autumnImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_grass);
    glBindTexture( GL_TEXTURE_2D, texture_grass);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, grassImage.width, grassImage.height, 0,
                 (grassImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, grassImage.data );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glBindTexture( GL_TEXTURE_2D, texture_diamond );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, diamondImage.width, diamondImage.height, 0,
                 (diamondImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, diamondImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glGenTextures( 1, &texture_sky );
    glBindTexture( GL_TEXTURE_2D, texture_sky );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, skyImage.width, skyImage.height, 0,
                 (skyImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, skyImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glUniform1i( uTex, 0);	// Set texture sampler variable to texture unit 0
	
	glEnable(GL_DEPTH_TEST);
}

struct color{ color( float r, float g, float b) : r(r), g(g), b(b) {} float r, g, b;};
std::stack<color> colors;
void set_color(float r, float g, float b)
{
	colors.push(color(r, g, b));

	float ambient  = 0.2, diffuse  = 0.6, specular = 0.2;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1 );
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1 );
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1 );
}

int mouseButton = -1, prevZoomCoord = 0 ;
vec2 anchor;
void myPassiveMotionCallBack(int x, int y) {	anchor = vec2( 2. * x / g_width - 1, -2. * y / g_height + 1 ); }

void myMouseCallBack(int button, int state, int x, int y)	// start or end mouse interaction
{
    mouseButton = button;
   
    if( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
        mouseButton = -1 ;
    if( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
        prevZoomCoord = -2. * y / g_height + 1;

    glutPostRedisplay() ;
}

void myMotionCallBack(int x, int y)
{
	vec2 arcball_coords( 2. * x / g_width - 1, -2. * y / g_height + 1 );
	 
    if( mouseButton == GLUT_LEFT_BUTTON )
    {
	   orientation = RotateX( -10 * (arcball_coords.y - anchor.y) ) * orientation;
	   orientation = RotateY(  10 * (arcball_coords.x - anchor.x) ) * orientation;
    }
	
	if( mouseButton == GLUT_RIGHT_BUTTON )
		zoom *= 1 + .1 * (arcball_coords.y - anchor.y);
    glutPostRedisplay() ;
}

void idleCallBack(void)
{
    if( !animate ) return;
	double prev_time = TIME;
    TIME = TM.GetElapsedTime() ;
    /*printf("TIME: %f\n", TIME);*/
	if( prev_time == 0 ) TM.Reset();
    glutPostRedisplay() ;
}

void drawCylinder()	//render a solid cylinder oriented along the Z axis; bases are of radius 1, placed at Z = 0, and at Z = 1.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

void drawCone()	//render a solid cone oriented along the Z axis; bases are of radius 1, placed at Z = 0, and at Z = 1.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}

void drawCube( GLuint texture_id)		// draw a cube with dimensions 1,1,1 centered around the origin.
{
	if (texture_id == texture_none)
    {
        glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
        glBindVertexArray( cubeData.vao );
        glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    }
    else
    {
        glBindTexture( GL_TEXTURE_2D, texture_id );
        glUniform1i( uEnableTex, 1 );
        glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
        glBindVertexArray( cubeData.vao );
        glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
        glUniform1i( uEnableTex, 0 );
    }
}

void drawSphere()	// draw a sphere with radius 1 centered around the origin.
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
}

void drawBackground( GLuint texture_id )
{
    glBindTexture( GL_TEXTURE_2D, texture_id );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
    
}
void drawHexagon()
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( hexagonData.vao );
    glDrawArrays( GL_TRIANGLES, 0, hexagonData.numVertices );
}

void drawStar()
{
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( starData.vao );
    glDrawArrays( GL_TRIANGLES, 0, starData.numVertices );
}

void drawDiamond()
{
    glBindTexture( GL_TEXTURE_2D, texture_diamond );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_FALSE, transpose(model_view) );
    glBindVertexArray( diamondData.vao );
    glDrawArrays( GL_TRIANGLES, 0, diamondData.numVertices );
    glUniform1i( uEnableTex, 0 );
}



int basis_id = 0;
void drawOneAxis()
{
	mat4 origin = model_view;
	model_view *= Translate	( 0, 0, 4 );
	model_view *= Scale(.25) * Scale( 1, 1, -1 );
	drawCone();
	model_view = origin;
	model_view *= Translate	( 1,  1, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube(texture_none);
	model_view = origin;
	model_view *= Translate	( 1, 0, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube(texture_none);
	model_view = origin;
	model_view *= Translate	( 0,  1, .5 );
	model_view *= Scale		( .1, .1, 1 );
	drawCube(texture_none);
	model_view = origin;
	model_view *= Translate	( 0,  0, 2 );
	model_view *= Scale(.1) * Scale(   1, 1, 20);
    drawCylinder();
	model_view = origin;
}

void drawAxes(int selected)
{
	if( basis_to_display != selected ) 
		return;
	mat4 given_basis = model_view;
	model_view *= Scale		(.25);
	drawSphere();
	model_view = given_basis;
	set_color( 0, 0, 1 );
	drawOneAxis();
	model_view *= RotateX	(-90);
	model_view *= Scale		(1, -1, 1);
	set_color( 1, 1, 1);
	drawOneAxis();
	model_view = given_basis;
	model_view *= RotateY	(90);
	model_view *= Scale		(-1, 1, 1);
	set_color( 1, 0, 0 );
	drawOneAxis();
	model_view = given_basis;
	
	colors.pop();
	colors.pop();
	colors.pop();
	set_color( colors.top().r, colors.top().g, colors.top().b );
}



void drawGround(void)
{
    mvstack.push(model_view);
    model_view *= Translate(0, -10, 0);
    model_view *= Scale(1000, 1, 1000);
    drawCube(texture_grass);
    model_view = mvstack.top(); mvstack.pop();
}

void drawArms(float angle)
{
    float theta, arm_segment = 1.5;
    theta = 30*sin(angle*DegreesToRadians*TIME);
    
    mvstack.push(model_view);
    //left arms
    model_view *= Translate(0, 0.08, -0.75);
    model_view *= RotateX(theta);
    model_view *= Translate(0, -0.08, 0.75);
    mvstack.push(model_view);
    model_view *= Scale(0.4, 0.4, arm_segment);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, arm_segment);
    model_view *= Translate(0, 0.08, -0.75);
    model_view *= RotateX(theta);
    model_view *= Translate(0, -0.08, 0.75);
    mvstack.push(model_view);
    model_view *= Scale(0.4, 0.4, arm_segment);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, 0.75);
    model_view *= Scale(0.2, 0.2, 0.8);
    set_color(0.8, 0.4, 0);
    drawSphere(); //left hand
    
    //right arms
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, -2.5);
    model_view *= Translate(0, 0.08, 0.75);
    model_view *= RotateX(-theta);
    model_view *= Translate(0, -0.08, -0.75);
    mvstack.push(model_view);
    model_view *= Scale(0.4, 0.4, arm_segment);
    set_color(0.9, 0.6, 0.6);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, -arm_segment);
    model_view *= Translate(0, 0.08, 0.75);
    model_view *= RotateX(-theta);
    model_view *= Translate(0, -0.08, -0.75);
    mvstack.push(model_view);
    model_view *= Scale(0.4, 0.4, arm_segment);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, -0.75);
    model_view *= Scale(0.2, 0.2, 0.8);
    set_color(0.8, 0.4, 0);
    drawSphere(); //right hand
}


void drawLegs(float angle)
{
    float theta, beta, leg_segment = 1.8;
    
    mvstack.push(model_view);
    
    
    theta = 50*sin(angle*DegreesToRadians*(TIME))+50;
    beta = 50*sin(angle*DegreesToRadians*(TIME)-PI/2)+50;
    //left leg
    if (TIME > 8.495 && TIME < 31.4694)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-theta);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    else if (TIME >= 31.4694 && TIME < 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-80);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    else if (TIME >= 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-55);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    
    model_view *= Translate(0.02, 0.9, 0);
    model_view *= RotateZ(55); //rotate to fit the inclination of the seasaw
    model_view *= Translate(-0.02, -0.9, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.5, leg_segment, 0.5);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -leg_segment, 0);
    
    if (TIME > 8.495 && TIME < 31.4694)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-theta);
        model_view *= Translate(-0.02, -0.9, 0);
        
    }
    else if (TIME >= 31.4694 && TIME < 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-80);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    /*else if (TIME >= 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-55);
        model_view *= Translate(-0.02, -0.9, 0);
    }*/
    
    mvstack.push(model_view);
    model_view *= Scale(0.5, leg_segment, 0.5);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0.45, -0.9, 0);
    model_view *= Scale(1, 0.2, 0.3);
    set_color(0.8, 0.4, 0);
    drawSphere(); //left feet
    
    //right leg
    set_color(0.6, 0.9, 1.8);
    model_view = mvstack.top(); mvstack.pop();
    
    model_view *= Translate(0, 0, -1);
    if (TIME > 8.495  && TIME < 31.4694)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-beta);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    else if (TIME >= 31.4694 && TIME < 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-80);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    else if (TIME >= 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-55);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    model_view *= Translate(0.02, 0.9, 0);
    model_view *= RotateZ(55);
    model_view *= Translate(-0.02, -0.9, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.5, leg_segment, 0.5);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -leg_segment, 0);
    if (TIME > 8.495  && TIME < 31.4694)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-beta);
        model_view *= Translate(-0.02, -0.9, 0);
    }
   
    else if (TIME >= 31.4694 && TIME < 45.3699)
    {
        model_view *= Translate(0.02, 0.9, 0);
        model_view *= RotateZ(-80);
        model_view *= Translate(-0.02, -0.9, 0);
    }
    mvstack.push(model_view);
    model_view *= Scale(0.5, leg_segment, 0.5);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0.45, -0.9, 0);
    model_view *= Scale(1, 0.2, 0.3);
    set_color(0.8, 0.4, 0);
    drawSphere(); //right feet
    
}

void drawStarFlower(int option, int k, double phase, int color)
{
    mvstack.push(model_view);
    double l_segment = 5, sway_speed = 100;
    for (int i = 0; i < 8; i++)
    {
        if (option == 1)
            model_view *= RotateZ(k*5*sin(sway_speed*DegreesToRadians*TIME+phase));
        else if (option == 2)
            model_view *= RotateX(k*5*sin(sway_speed*DegreesToRadians*TIME+phase));
        model_view *= Translate(0, l_segment/2, 0);
        mvstack.push(model_view);
        model_view *= Scale(1, l_segment, 1);
        set_color(0.59765625, 0.296875, 0);
        drawCube( texture_none );
        model_view = mvstack.top(); mvstack.pop();
    }
    model_view *= Translate(0, l_segment/2, 0);
    model_view *= Scale(5, 5, 5);
    if (option == 1)
        model_view *= RotateZ(200*TIME);
    else if (option == 2)
    {
        model_view *= RotateY(90);
        model_view *= RotateZ(200*TIME);
    }
    if (color == 1) //yellow
        set_color(0.9961, 0.9961, 0.39844);
    else if (color == 2) //pink
        set_color(0.59765625, 0.9961, 0.9961);
    else if (color == 3)//purple
        set_color(0.796875, 0.597656, 0.9961);
    else if (color ==4) //shallow pink
        set_color(0.9961, 0.3984, 0.3984);
    else//green 
        set_color(0.1992, 0.9961, 0.1992);
    drawStar();
    model_view = mvstack.top(); mvstack.pop();
}


std::stack<mat4> shapesInDog; //assign model_view to it; using a for loop to draw one by one

//every primitive take the same parent vec4


double previousTime = 0;
int frameNumber = 0;
double average;

void display(void)
{
	/*animate = 1;*/
    if (TIME != previousTime)
    {
        double newMeasurement = 1.0 / (TIME -previousTime - 0.000001);
        const float decay_period = 10;
        const float a = 2. / (decay_period - 1. );
        average = a*newMeasurement+ (1-a)*average;
        if (frameNumber % 20 == 0)
        {
            std::cout << "Frame Rate: " << average << std::endl;
        }
        previousTime = TIME;
        frameNumber++;
    }
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    vec4 rotatedEye;
    vec4 varyEye;
    vec4 varyRef;
    float rotationBeginTime = 1;
    float rotationSceneTime = TIME - rotationBeginTime;
    float timeToRotate = 5;
    varyRef = ref;
    if (TIME < 9)
        varyEye = eye;
    else if (TIME >= 9 && TIME < 13)
        varyEye = Translate(-10*(TIME-9), 10*(TIME-9), 8*(TIME-9)) * eye;
    else if (TIME >= 13 && TIME < 16)
        varyEye = Translate(-40-5*(TIME-13), 40-18*(TIME-13), 32-6.182*(TIME-13)) * eye;
    else if (TIME >= 16 && TIME < 24)
        varyEye = Translate(-55, -14, 13.454-6.182*(TIME-16)) * eye;
    else if (TIME >= 24 && TIME < 30)
    {
        varyEye = Translate(-55+5*(TIME-24), -14-5*(TIME-24), -36-5.833*(TIME-24)) * eye;
        varyRef = Translate(1.429*(TIME-24), 1*(TIME-24), 0) * ref;
    }
    else if (TIME >= 30 && TIME < 31)
    {
        varyEye = Translate(-25, -44, -71) * eye;
        varyRef = Translate(10, 7, 0) * ref;

    }
    else if (TIME >= 31 && TIME < 36)
    {
        varyEye = Translate(-25+TIME-31, -44+2*(TIME-31), -71+15*(TIME-31)) * eye;
        varyRef = Translate(10+2*(TIME-31), 7, 0) * ref;
    }
    else if (TIME >= 36 && TIME < 42)
    {
        varyEye = Translate(-20-3*(TIME-36), -34, 4+12*(TIME-36)) * eye;
        varyRef = Translate(20, 7, 0) * ref;
    }
    
    else if (TIME >= 42 && TIME < 43)
    {
        varyEye = Translate(-38, -34, 76) * eye;
        varyRef = Translate(20, 7, 0) * ref;
    }
    else
    {
        varyEye = Translate(-38+5*(TIME-43), -34, 76-10*(TIME-43)) * eye;
        varyRef = Translate(20+5*(TIME-43), 7-(TIME-43), 0) * ref;
    }
    
    
    
    if (rotationSceneTime > 0 && rotationSceneTime < timeToRotate) //360 scene
    {
        rotatedEye = RotateY(360 / timeToRotate * rotationSceneTime) * eye;
        model_view = LookAt(rotatedEye, ref, up);
    }

    else
        model_view = LookAt(varyEye, varyRef, up);
    
    
    
    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );
    
    model_view *= orientation;
    model_view *= Scale(zoom);
    
    mvstack.push(model_view);
    mvstack.push(model_view);
    mvstack.push(model_view);
    
    drawGround();
    mvstack.push(model_view);
    model_view *= Translate(0, 150, 0);
    model_view *= Scale(1000, 500, 1000);
    drawBackground(texture_autumn);
    model_view = mvstack.top();
    model_view *= Translate(0, 400, 0);
    model_view *= Scale(1000, 1, 1000);
    drawCube(texture_sky);
    
    
    //draw a seesaw
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -9, 0);
    mvstack.push(model_view);
    set_color(0.64, 0.16, 0.16);
    model_view *= Scale(9, 0.8, 4);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 2.4, 0);
    mvstack.push(model_view);
    mvstack.push(model_view);
    model_view *= Translate(-1, -0.8, 0);
    model_view *= RotateZ(-30);
    model_view *= Scale(0.5, 4, 4);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(1, -0.8, 0);
    model_view *= RotateZ(30);
    model_view *= Scale(0.5, 4, 4);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 1, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.5, 0.5, 2.4);
    drawCylinder();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0.5, 0);
    
    mvstack.push(model_view);
    set_color(0.64, 0.15, 0.15);
    if (TIME < 8.495)
        model_view *= RotateZ(-25);
    else if (TIME >= 8.495 && TIME < 9.495)
        model_view *= RotateZ(100*sin(30*DegreesToRadians*(TIME-8.495))-25);
    else if (TIME >= 9.495 && TIME < 31.4694)
        model_view *= RotateZ(25);
    else if (TIME >= 31.4694 && TIME < 32.4694)
        model_view *= RotateZ(25-100*sin(30*DegreesToRadians*(TIME-31.4694)));
    else if (TIME >= 32.4694 && TIME < 40.3733)
        model_view *= RotateZ(-25);
    else if (TIME >= 40.3733 && TIME < 41.3733)
        model_view *= RotateZ(-25+100*sin(30*DegreesToRadians*(TIME-40.3733)));
    else
        model_view *= RotateZ(25);
    model_view *= Scale(20, 0.7, 4);
    drawCube(texture_none);
    
    //DRAW FLYMAN
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(7, 2.2, 0);
    //add translation here (fly up) (ADAPATION HERE)
    if (TIME >= 8.495 && TIME < 12)
        model_view *= Translate(7+5*(TIME-8.495), 2.2+30*(TIME-8.495)-4.9*pow((TIME-8.495),2), 0);
    else if (TIME >= 12 && TIME < 15.07)
        model_view *= Translate(24.525+2.5*(TIME-12), 47.153-4.9*pow((TIME-12),2), 0);
    else if (TIME >= 15.07 && TIME < 19.95)
        model_view *= Translate(32.2 + 3.5*(TIME-15.07),
                                1+25*(TIME-15.07)-4.9*pow((TIME-15.07),2), 0);
    else if (TIME > 19.95 && TIME < 24.426)
        model_view *= Translate(49.28+4.5*(TIME-19.95),
                                4.9224+25*(TIME-19.95)-4.9*pow((TIME-19.95),2), 0);
    else if (TIME >= 24.426 && TIME < 26.6378)
        model_view *= Translate(69.422+5*(TIME-24.426),
                                18.653+25*(TIME-24.426)-4.9*pow((TIME-24.426),2), 0);
    else if (TIME > 26.6378 && TIME < 28.6378)
    {
        model_view *= Translate(80.481, 49.977, 0);
        model_view *= RotateY(90*(TIME-26.6378));
    }
    else if (TIME >= 28.6378 && TIME < 31.4694)
    {
        model_view *= Translate(80.481, 49.977, 0);
        model_view *= RotateY(180);
        model_view *= Translate(26*(TIME-28.6378), -4.9*pow((TIME-28.6378),2), 0);
    }
    else if (TIME >= 31.4694 && TIME < 32.4694)
    {
        model_view *= RotateY(180);
        model_view *= Translate(1.1962, 10.689-9.389*(TIME-31.4694), 0);
    }
    else if (TIME >= 32.4694 && TIME < 40.3733) 
    {
        model_view *= RotateY(180);
        model_view *= Translate(1.1962, 1.3, 0);
    }
    else if (TIME >= 40.3733 && TIME < 42.3733)
    {
        model_view *= Translate(1.1962+12*(TIME-40.3733), 1.3+30*(TIME-40.3733)-4.9*pow((TIME-40.3733),2), 0);
        model_view *= RotateY(90*(TIME-40.3733));
    }
    else if (TIME >= 42.3733 && TIME < 43.9317)
    {
        model_view *= Translate(25.1962+14*(TIME-42.3733), 41.7, 0);
    }
    else if (TIME >= 43.9317 && TIME < 45.3699)
        model_view *= Translate(47.0138+14*(TIME-43.9317),
                                41.7-10.49*pow((TIME-43.9317),2), 0);
    else if (TIME >= 45.3699)
        model_view *= Translate(67.1486, 20, 0);
    

    
    mvstack.push(model_view);
    mvstack.push(model_view);
    mvstack.push(model_view);
    //draw head
    model_view *= Scale(1.8, 1.8, 2.2);
    set_color(0.6, 0.9, 0.8);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    //draw left eye
    model_view *= Translate(0.9, 0.2, 0.8);
    mvstack.push(model_view);
    model_view *= Scale(0.2, 0.2, 0.2);
    set_color(0, 0, 0);
    drawSphere();
    //draw right eye
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, -1.6);
    mvstack.push(model_view);
    model_view *= Scale(0.2, 0.2, 0.2);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    //draw nose
    model_view *= Translate(0, -0.1, 0.8);
    mvstack.push(model_view);
    model_view *= Scale(0.2, 0.5, 0.2);
    set_color(0.8, 0.4, 0);
    drawCube(texture_none);
    //draw mouth
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -0.5, 0);
    model_view *= Scale(0.3, 0.2, 0.8);
    set_color(0.9, 0.2, 0.2);
    drawCube(texture_none);
    //draw ears
    set_color(0.8, 0.4, 0);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, 1);
    mvstack.push(model_view);
    model_view *= Scale(0.4, 0.4, 0.4);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, -2);
    model_view *= Scale(0.4, 0.4, 0.4);
    drawSphere();
    
    
    //draw body
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1.2, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.5, 0.8, 0.5);
    set_color(0.6, 0.9, 1.8);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1.5, 0);
    mvstack.push(model_view); //extra push so that rotation in drawArms don't affect later parts
    mvstack.push(model_view); //extra push for arms
    model_view *= Scale(1, 2.5, 1);
    set_color(0.9, 0.6, 0.6);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0.6, 1.2);
    drawArms(30);
    model_view = mvstack.top(); mvstack.pop(); //at the center of the body
    model_view *= Translate(0, -1.6, 0);
    mvstack.push(model_view);
    //extra push for legs
    model_view *= Scale(0.6, 0.6, 1);
    set_color(0.6, 0.9, 1.8);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1, 0.5);
    drawLegs(30);
    
    //DRAW SWING
    model_view = mvstack.top(); mvstack.pop(); //at the center of the space
    model_view *= Translate(-40, 15, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.8, 0.8, 10);
    set_color(0, 0.9, 0.9);
    drawCylinder();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -12, 8);
    mvstack.push(model_view);
    model_view *= Scale(0.8, 25, 0.8);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -12, 0);
    mvstack.push(model_view);
    model_view *= Scale(2, 1, 2);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, -16);
    mvstack.push(model_view);
    model_view *= Scale(2, 1, 2);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 12, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.8, 25, 0.8);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    
    //draw swaying sticks
    model_view *= Translate(0, 4, 3);
    mvstack.push(model_view); //for bear not to be subjected to the rotation
    //ADAPTATION HERE
    if (TIME < 6.92)
    {
        model_view *= Translate(0, 7.5, 0);
        model_view *= RotateZ(15*TIME*sin(3*TIME));
        model_view *= Translate(0, -7.5, 0);
    }
    else
    {
        model_view *= Translate(0, 7.5, 0);
        model_view *= RotateZ(30/(TIME-6.92)*sin(3*(TIME-6.92)));
        model_view *= Translate(0, -7.5, 0);
        
    }
    mvstack.push(model_view);
    model_view *= Scale(0.3, 15, 0.3);
    set_color(0.996, 0.598, 0.598);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 0, 10);
    mvstack.push(model_view);
    model_view *= Scale(0.3, 15, 0.3);
    drawCube(texture_none);
    model_view = mvstack.top(); mvstack.pop();
    model_view *=  Translate(0, -7.5, -5);
    model_view *= Scale(2.5, 0.5, 12);
    drawCube(texture_none);
    
    //DRAW BEAR (stop at 36.9897s)
    model_view = mvstack.top(); mvstack.pop(); //at the right stick center without rotation
    model_view *= Translate(0, 2, 5);
    //animation of the bear (ADAPATION HERE)
    if (TIME < 6.57)
    {
        model_view *= Translate(0, 7.5, 0);
        model_view *= RotateZ(15*TIME*sin(3*TIME));
        model_view *= Translate(0, -7.5, 0);
    }
    else if (TIME >= 6.57 && TIME < 7.57)
        model_view *= Translate(15+8*(TIME-6.57), 5-3*(TIME-6.57), 0);
    else if (TIME >= 7.57 && TIME < 8.495)
        model_view *= Translate(23+8*(TIME-7.57), 2-2.162*(TIME-7.57), 0);
    else if (TIME >= 8.495 && TIME < 9.495)
        model_view *= Translate(31, -8.5*(TIME-8.495), 0);
    /*else if (TIME >= 9.495 && TIME < 12.495)
        model_view *= Translate(31+2*(TIME -9.495), -8.5+2*(TIME-9.495), 0);*/
    else if (TIME >= 9.495 && TIME < 31.4694)
        model_view *= Translate(31, -8.5, 0);
    /*else if (TIME >= 12.495 && TIME < 31.4694)
    {
        model_view *= Translate(37, -2.5, 0);
        model_view *= Translate(0, -5, 0);
        model_view *= RotateZ(180*(TIME-12.495));
        model_view *= Translate(0, 5, 0);
    }*/
    else if (TIME >= 31.4694 && TIME < 36.9064)
        model_view *= Translate(31+16*(TIME-31.4694),
                                -8.5+30*(TIME-31.4694)-4.9*pow((TIME-31.4694),2), 0);
    else if (TIME >= 36.9064 && TIME < 39.9064)
    {
        model_view *= Translate(117.992, 9.76125, 0);
        model_view *= RotateY(60*(TIME-36.9064));
    }
    else
    {
        model_view *= Translate(117.992, 9.76125, 0);
        model_view *= RotateY(180);
    }
    mvstack.push(model_view);
    model_view *= Scale(1.5, 1.5, 1.5);
    set_color(0.996, 0.598, 0.199);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 1, 1.2);
    mvstack.push(model_view);
    model_view *= Scale(0.8, 0.8, 0.8);
    set_color(0.797, 0.797, 0);
    drawSphere();
    model_view *= Translate(0, 0, -3);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(1, -1, 0);
    mvstack.push(model_view);
    model_view *= Scale(0.3, 0.3, 0.3);
    set_color(0.398, 0, 0);
    drawSphere();
    model_view *= Translate(0, 0, -7.5);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1, -1);
    mvstack.push(model_view);
    model_view *= Scale(0.3, 0.3, 0.7);
    set_color(0.9, 0.2, 0.2);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    //draw bear body
    model_view *= Translate(-1, -3, 0);
    mvstack.push(model_view);
    model_view *= Scale(2, 2.8, 2);
    set_color(0.797, 0.797, 0);
    drawSphere();
    //draw bear arms (ADAPTATION)
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 1, 2.5);
    mvstack.push(model_view);
    model_view *= RotateX(-40);
    model_view *= Scale(0.8, 1.8, 0.8);
    set_color(0.996, 0.598, 0.199);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1.5, 1);
    mvstack.push(model_view);
    model_view *= Scale(0.6, 0.6, 0.6);
    set_color(0.797, 0.797, 0);
    drawSphere();
    
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, 1.6, -6);
    mvstack.push(model_view);
    model_view *= RotateX(40);
    model_view *= Scale(0.8, 1.8, 0.8);
    set_color(0.996, 0.598, 0.199);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1.5, -1);
    mvstack.push(model_view);
    
    model_view *= Scale(0.6, 0.6, 0.6);
    set_color(0.797, 0.797, 0);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    
    //draw bear legs
    model_view *= Translate(0, -3, 2.5);
    mvstack.push(model_view);
    if (TIME > 31.4694)
    {
        model_view *= Translate(0, 1.5, 0);
        model_view *= RotateZ(-20*sin(80*DegreesToRadians*(TIME - 31.4694)));
        model_view *= Translate(0, -1.5, 0);
        
    }
    mvstack.push(model_view);
    model_view *= Scale(0.8, 1.5, 0.8);
    set_color(0.996, 0.598, 0.199);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1.5, 0);
    //mvstack.push(model_view);
    model_view *= Scale(0.8, 0.6, 0.6);
    set_color(0.797, 0.797, 0);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    
    model_view *= Translate(0, 0, 2);
    //mvstack.push(model_view);
    if (TIME > 31.4694)
    {
        model_view *= Translate(0, 1.5, 0);
        model_view *= RotateZ(20*sin(80*DegreesToRadians*(TIME - 31.4694)));
        model_view *= Translate(0, -1.5, 0);
        
    }
    mvstack.push(model_view);
    model_view *= Scale(0.8, 1.5, 0.8);
    set_color(0.996, 0.598, 0.199);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(0, -1.5, 0);
    model_view *= Scale(0.8, 0.6, 0.6);
    set_color(0.797, 0.797, 0);
    drawSphere();
    model_view = mvstack.top(); mvstack.pop(); //at the space center
    model_view *= Translate(38, -9, 0);
    mvstack.push(model_view);
    model_view *= RotateX(-90);
    model_view *= RotateZ(80*TIME);
    model_view *= Scale(3, 3, 3);
    set_color(0, 0.9961, 0.5);
    drawHexagon();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(19, 2, 0);
    mvstack.push(model_view);
    model_view *= RotateX(-90);
    model_view *= RotateZ(-95*(TIME-4));
    model_view *= Scale(5, 5, 12);
    set_color(0, 0.9961, 0.9961);
    drawHexagon();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(22, 7, 0);
    mvstack.push(model_view);
    model_view *= RotateX(-90);
    model_view *= RotateZ(105*(TIME-2));
    model_view *= Scale(8, 8, 39);
    set_color(0.9961, 0.796875, 0.597656);
    drawHexagon();
    model_view = mvstack.top(); mvstack.pop();
    model_view *= Translate(-80, -10, -30);
    drawStarFlower(1, 1, -PI/4, 1);
    model_view *= Translate(-30, 0, 0);
    drawStarFlower(1, 1, PI/3, 2);
    model_view *= Translate(60, 0, 0);
    drawStarFlower(1, 1, PI/3, 5);
    model_view *= Translate(30, 0, 0);
    drawStarFlower(1, -1, PI/4, 4);
    model_view *= Translate(-120, 0, 0);
    drawStarFlower(1, -1, PI/5, 3);
    model_view *= Translate(-30, 0, 0);
    drawStarFlower(1, 1, 0, 5);
    model_view *= Translate(0, 0, 30);
    drawStarFlower(2, 1, PI/2, 1);
    model_view *= Translate(0, 0, 30);
    drawStarFlower(2, -1, PI/3, 4);
    model_view *= Translate(180, 0, 0);
    drawStarFlower(2, -1, PI/5, 3);
    model_view *= Translate(0, 0, -30);
    drawStarFlower(2, 1, PI/4, 1);
    model_view *= Translate(0, 0, -30);
    drawStarFlower(1, 1, 0, 2);
    model_view *= Translate(-90, 40, 30);
    mvstack.push(model_view);
    if (TIME > 35)
    {
      if (TIME < 40.2206)
      {
          if (TIME <= 39.2294)
          {
              model_view *= Translate(0, 30-11*(TIME-35), 0);
              model_view *= Translate(-7, 0, 0);
              model_view *= RotateY(83*(TIME-35));
              model_view *= Translate(7, 0, 0);
              /*model_view *= Scale(2*(sqrt(TIME - 35)), 2*(sqrt(TIME-35)),
               1.5*(sqrt(TIME-35)));*/
          }
          else 
              model_view *= Translate(-2.5*(TIME-39.2294), -16.5234-8*(TIME-39.2294), 0);
          model_view *= Scale(1.3*(sqrt(TIME - 35)), 1.3*(sqrt(TIME-35)),
                              1.05*(sqrt(TIME-35)));
      }
      else if (TIME >= 40.2206 && TIME < 40.3733)
      {
          model_view *= Translate(-2.478, -24.253-5*(TIME-40.2206), 0);
          model_view *= Translate(0, -9, 0);
          model_view *= RotateZ(5*(TIME-40.2206));
          model_view *= Translate(0, 9, 0);
          model_view *= Scale(2.97, 2.97, 2.399);
      }
      else if (TIME >= 40.3733 && TIME < 41.3733)
      {
          model_view *= Translate(-2.478-0.2*(TIME-40.3733), -25.0165-2*(TIME-40.3733), 0);
          model_view *= Translate(0, -9, 0);
          model_view *= RotateZ(78*(TIME-40.3733));
          model_view *= Translate(0, 9, 0);
          model_view *= Scale(2.97, 2.97, 2.399);
      }
      else if (TIME >= 40.3733 && TIME < 41.3733)
      {
          model_view *= Translate(-2.678, -27.0165, 0);
          model_view *= Translate(0, -8, 0);
          model_view *= RotateZ(2*(TIME=40.3733));
          model_view *= Translate(0, 8, 0);
          model_view *= Scale(2.97, 2.97, 2.399);
      }
      else
      {
          model_view *= Translate(-2.678, -27.0165, 0);
          model_view *= Translate(0, -9, 0);
          model_view *= RotateZ(85);
          model_view *= Translate(0, 9, 0);
          model_view *= Scale(2.97, 2.97, 2.399);
      }
        
        drawDiamond();
    }
    model_view = mvstack.top();
    /*if (TIME >= 36)
    {
        if (TIME < 44.5343)
        {
            model_view *= Translate(50, 30-5*(TIME-36), 0);
            model_view *= Translate(16, 0, 0);
            model_view *= RotateY(83*(TIME-36));
            model_view *= Translate(-16, 0, 0);

        }
        else
        {
            model_view *= Translate(30, -12.6715, 0);
            model_view *= Translate(16, 0, 0);
            model_view *= RotateY(90*(TIME-36));
            model_view *= Translate(-16, 0, 0);
        }
                    
        drawDiamond();
    }*/
    for (double i = 40; i < 44; i+=0.5)
    {
        mvstack.push(model_view);
        if (TIME >= i)
        {
            if (TIME < i + 8.5343)
            {
                model_view *= Translate(50, 30-5*(TIME-i), 0);
                model_view *= Translate(25, 0, 0);
                model_view *= RotateY(90*(TIME-i));
                model_view *= Translate(-25, 0, 0);
            }
            else
            {
                model_view = mvstack.top();
                model_view *= Translate(50, -12.6715, 0);
                model_view *= Translate(25, 0, 0);
                model_view *= RotateY(48.906+90*(TIME-i-8.5343));
                model_view *= Translate(-25, 0, 0);

            }
            drawDiamond();
        }
        model_view = mvstack.top(); mvstack.pop();
    }
        
    glutSwapBuffers();
}

void myReshape(int w, int h)	// Handles window sizing and resizing.
{    
    mat4 projection = Perspective( 50, (float)w/h, 1, 1000 );
    glUniformMatrix4fv( uProjection, 1, GL_FALSE, transpose(projection) );
	glViewport(0, 0, g_width = w, g_height = h);	
}		

void instructions() {	 std::cout <<	"Press:"									<< '\n' <<
										"  r to restore the original view."			<< '\n' <<
										"  0 to restore the original state."		<< '\n' <<
										"  a to toggle the animation."				<< '\n' <<
										"  b to show the next basis's axes."		<< '\n' <<
										"  B to show the previous basis's axes."	<< '\n' <<
										"  q to quit."								<< '\n';	}

void myKey(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':   case 27:				// 27 = esc key
            exit(0); 
		case 'b':
			std::cout << "Basis: " << ++basis_to_display << '\n';
			break;
		case 'B':
			std::cout << "Basis: " << --basis_to_display << '\n';
			break;
        case 'a':							// toggle animation           		
            if(animate) std::cout << "Elapsed time " << TIME << '\n';
            animate = 1 - animate ; 
            break ;
		case '0':							// Add code to reset your object here.
			TIME = 0;	TM.Reset() ;											
        case 'r':
			orientation = mat4();			
            break ;
    }
    glutPostRedisplay() ;
}

int main() 
{
	char title[] = "Title";
	int argcount = 1;	 char* title_ptr = title;
	glutInit(&argcount,		 &title_ptr);
	glutInitWindowPosition (230, 70);
	glutInitWindowSize     (g_width, g_height);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow(title);
	#if !defined(__APPLE__) && !defined(EMSCRIPTEN)
		glewExperimental = GL_TRUE;
		glewInit();
	#endif
    std::cout << "GL version " << glGetString(GL_VERSION) << '\n';
	instructions();
	init();

	glutDisplayFunc(display);
    glutIdleFunc(idleCallBack) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCallBack) ;
    glutMotionFunc(myMotionCallBack) ;
    glutPassiveMotionFunc(myPassiveMotionCallBack) ;

	glutMainLoop();
}