/*
*		This Code Was Created By Jeff Molofee 2000
*		A HUGE Thanks To Fredric Echols For Cleaning Up
*		And Optimizing This Code, Making It More Flexible!
*		If You've Found This Code Useful, Please Let Me Know.
*		Visit My Site At nehe.gamedev.net
*/

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <gl\glut.h>
#include <camera.h>
#include "Model_3DS.h"
#include "texture.h"
#include <math.h>
#include <iostream>
#include <string>
#include <sound.h>
Camera MyCamera;
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

														// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}
float JetMass = 6000, rho[10] = { 1.225 ,1.112 , 1.007 , 0.9093 , 0.8194 , 0.7364 ,0.66 ,0.59,0.5258,0.4671 };
float WingArea = 28.95, Cd = 0.1, Cl = 1.5, FanArea = 17, Ve = 340, V0 = 10, gravityAcc = 10,Brakes = 0;


void Debug(float x, std::string Anything = "") {
	std::string s;
	s = std::to_string(x);
	s += " ";
	s += Anything;
	s += "\n";
	LPTSTR long_string = new TCHAR[s.size() + 1];
	strcpy(long_string, s.c_str());
	OutputDebugString(long_string);
}

class Vector3
{
private:
	float x, y, z, mag;
public:
	Vector3() {
		x = y = z = mag = 0;
	}
	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		mag = sqrt(x*x + y*y + z*z);
	}
	void Add(Vector3 V)
	{
		x += V.getx();
		y += V.gety();
		z += V.getz();
		mag = sqrt(x*x + y*y + z*z);
	}
	void mult(float a) {
		x *= a;
		y *= a;
		z *= a;
		mag = sqrt(x*x + y*y + z*z);
	}
	void normalize() {
		if (mag) {
			x /= mag;
			y /= mag;
			z /= mag;
		}
		mag = 1;
	}
	float getx() {
		return x;
	}
	float gety() {
		return y;
	}
	float getz() {
		return z;
	}
	float getmag() {
		return mag;
	}
	void sety(float y) {
		this->y = y;
		mag = sqrt(x*x + y*y + z*z);
	}
	void setx(float x) {
		this->x = x;
		mag = sqrt(x*x + y*y + z*z);
	}
	void setz(float z) {
		this->z = z;
		mag = sqrt(x*x + y*y + z*z);
	}
	static float CosAlpha(Vector3 V1, Vector3 V2) {
		if (V1.getmag() == 0 || V2.getmag() == 0) return 0;
		return (V1.getx()*V2.getx() + V1.gety()*V2.gety() + V1.getz()*V2.getz()) / (V1.getmag()*V2.getmag());
	}
};

class Jet_Airplane {
private:
	Model_3DS Jet;
	Vector3 Location;
	Vector3 Velocity;
	Vector3 Acceleration;
	float mass;
public:
	Jet_Airplane() {

		Jet = Model_3DS();
		Jet.Load("Model//Jet//Jet.3ds");
		Jet.Materials[0].tex.Load("Model//Jet//body.bmp");
		Jet.Materials[1].tex.Load("Model//Jet//body.bmp");
		Jet.Materials[2].tex.Load("Model//Jet//body.bmp");
		Jet.Materials[3].tex.Load("Model//Jet//SL1.bmp");
		Jet.Materials[4].tex.Load("Model//Jet//SL1.bmp");
		Jet.Materials[5].tex.Load("Model//Jet//SL1.bmp");
		Jet.scale = 0.5;
		Location = Vector3(Jet.pos.x, Jet.pos.y, Jet.pos.z);
		Velocity = Vector3();
		Acceleration = Vector3();
	}
	Jet_Airplane(float mass)
	{
		Jet = Model_3DS();
		Jet.Load("Model//Jet//Jet.3ds");
		Jet.Materials[0].tex.Load("Model//Jet//body.bmp");
		Jet.Materials[1].tex.Load("Model//Jet//body.bmp");
		Jet.Materials[2].tex.Load("Model//Jet//body.bmp");
		Jet.Materials[3].tex.Load("Model//Jet//SL1.bmp");
		Jet.Materials[4].tex.Load("Model//Jet//SL1.bmp");
		Jet.Materials[5].tex.Load("Model//Jet//SL1.bmp");
		Jet.scale = 0.5;
		Location = Vector3(Jet.pos.x, Jet.pos.y, Jet.pos.z);
		Velocity = Vector3();
		Acceleration = Vector3();
		this->mass = mass;
	}
	void Draw()
	{
		glColor3ub(255, 255, 255);
		Jet.Draw();
	}
	Vector3 getLocation() {
		return Location;
	}
	Vector3 getVelocity() {
		return Velocity;
	}
	float getposz() {
		return Jet.pos.z;
	}

	void Update() {
		Velocity.Add(Acceleration);
		if (Location.gety() <= 0.05)
			Velocity.sety(max(Velocity.gety(), 0.0));
		Location.Add(Velocity);
		Location.sety(max(Location.gety(), 0.0));
		MakePosSameLocation();
		MoveCamera();
		Acceleration.mult(0);
		Draw();

	}

	void MoveCamera() {
		MyCamera.Position.x += Velocity.getx();
		MyCamera.Position.y += Velocity.gety();
		MyCamera.Position.z += Velocity.getz();
		MyCamera.Position.y = max(MyCamera.Position.y, 3.0);
	}

	void MakePosSameLocation() {
		Jet.pos.x = MyCamera.Position.x + 15 * MyCamera.View.x;
		Jet.pos.y = MyCamera.Position.y - 3;
		Jet.pos.z = MyCamera.Position.z + 15 * MyCamera.View.z;
	}

	void ApplyForce(Vector3 Force) {
		Vector3 V = Vector3(Force.getx() / mass, Force.gety() / mass, Force.getz() / mass);
		Acceleration.Add(V);
	}

	void RotateOnX(float Angle) {
		Jet.rot.x = Angle;
	}
	void RotateOnY(float Angle) {
		Jet.rot.y = Angle;
	}
	void RotateOnZ(float Angle) {
		Jet.rot.z = Angle;
	}
	float GetRotateOnX() {
		return Jet.rot.x;
	}
	float GetRotateOnY() {
		return Jet.rot.y;
	}
	float GetRotateOnZ() {
		return Jet.rot.z;
	}
	void SetVelocity(Vector3 Velocity)
	{
		this->Velocity = Velocity;
	}
	void SetVX(float X)
	{
		Velocity.setx(X);
	}
	void SetVZ(float Z)
	{
		Velocity.setz(Z);
	}
};




Sound JetSound;
Jet_Airplane Jet;
GLint StreetTexture;
GLint GroundTexture;
GLint SkyboxFrontTexture;
GLint SkyboxBackTexture;
GLint SkyboxLeftTexture;
GLint SkyboxRightTexture;
GLint SkyboxUpTexture;
GLint SkyboxDownTexture;
int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	INIT SInit = INIT();
	glEnable(GL_TEXTURE_2D);
	MyCamera = Camera();
	StreetTexture = LoadTexture("images//street.bmp");
	GroundTexture = LoadTexture("images//ground.bmp");
	SkyboxFrontTexture = LoadTexture("images//front.bmp");
	SkyboxBackTexture = LoadTexture("images//back.bmp");
	SkyboxLeftTexture = LoadTexture("images//left.bmp");
	SkyboxRightTexture = LoadTexture("images//right.bmp");
	SkyboxUpTexture = LoadTexture("images//top.bmp");
	SkyboxDownTexture = LoadTexture("images//down.bmp");

	SInit.InitOpenAL();
	JetSound = Sound("sound//jets.wav");
	Jet = Jet_Airplane(JetMass);

	MyCamera.Position = { 0,3,15 };
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	return TRUE;										// Initialization Went OK
}

bool SoundOn = false;
void SoundControl()
{
	if (Jet.getVelocity().getmag() > 1)
		SoundOn = 1;
	else SoundOn = 0;
	if (keys['M']) {
		if (SoundOn)
			JetSound.Play();
	}
	if (!SoundOn) JetSound.Stop();
}

void DrawStreet()
{
	glBindTexture(GL_TEXTURE_2D, StreetTexture);
	glBegin(GL_POLYGON);

	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0, 0);
	glVertex3f(-3, 0, 50);
	glTexCoord2f(1, 0);
	glVertex3f(3, 0, 50);
	glTexCoord2f(1, 100);
	glVertex3f(3, 0, -100000);
	glTexCoord2f(0, 100);
	glVertex3f(-3, 0, -100000);
	glColor3f(0.0, 0.0, 0.0);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, StreetTexture);
	glBegin(GL_POLYGON);

	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0, 0);
	glVertex3f(27, 0, 50);
	glTexCoord2f(1, 0);
	glVertex3f(33, 0, 50);
	glTexCoord2f(1, 15);
	glVertex3f(33, 0, -100000);
	glTexCoord2f(0, 15);
	glVertex3f(27, 0, -100000);
	glColor3f(0.0, 0.0, 0.0);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, StreetTexture);
	glBegin(GL_POLYGON);

	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0, 0);
	glVertex3f(57, 0, 50);
	glTexCoord2f(1, 0);
	glVertex3f(63, 0, 50);
	glTexCoord2f(1, 15);
	glVertex3f(63, 0, -100000);
	glTexCoord2f(0, 15);
	glVertex3f(57, 0, -100000);
	glColor3f(0.0, 0.0, 0.0);

	glEnd();
}

void DrawGround()
{
	glBindTexture(GL_TEXTURE_2D, GroundTexture);
	glBegin(GL_POLYGON);

	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(0, 0);
	glVertex3f(-100000, -0.5, 100000);
	glTexCoord2f(400, 0);
	glVertex3f(100000, -0.5, 100000);
	glTexCoord2f(400, 400);
	glVertex3f(100000, -0.5, -100000);
	glTexCoord2f(0, 400);
	glVertex3f(-100000, -0.5, -100000);
	glColor3f(0.0, 0.0, 0.0);

	glEnd();
}

void DrawSkyBox()
{
	glPushMatrix();
	glScalef(500000, 500000, 500000);

	glBindTexture(GL_TEXTURE_2D, SkyboxFrontTexture);
	glBegin(GL_QUADS);
	//front
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); 		glVertex3f(0.5, -0.5, 0.5);
	glTexCoord2f(1, 1);			glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f(0, 1);			glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f(0, 0);			glVertex3f(-0.5, -0.5, 0.5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, SkyboxBackTexture);
	glBegin(GL_QUADS);
	//back
	glTexCoord2f(0, 0);		glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f(0, 1);		glVertex3f(0.5, 0.5, -0.5);
	glTexCoord2f(1, 1);		glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f(1, 0);		glVertex3f(-0.5, -0.5, -0.5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, SkyboxRightTexture);
	glBegin(GL_QUADS);
	//right
	glTexCoord2f(1, 0);		glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f(1, 1);		glVertex3f(0.5, 0.5, -0.5);
	glTexCoord2f(0, 1);		glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f(0, 0);		glVertex3f(0.5, -0.5, 0.5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, SkyboxLeftTexture);
	glBegin(GL_QUADS);
	//left
	glTexCoord2f(1, 0);		glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f(1, 1);		glVertex3f(-0.5, 0.5, 0.5);
	glTexCoord2f(0, 1);		glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f(0, 0);		glVertex3f(-0.5, -0.5, -0.5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, SkyboxUpTexture);
	glBegin(GL_QUADS);
	//up
	glTexCoord2f(1, 0);		glVertex3f(0.5, 0.5, 0.5);
	glTexCoord2f(1, 1);		glVertex3f(0.5, 0.5, -0.5);
	glTexCoord2f(0, 1);		glVertex3f(-0.5, 0.5, -0.5);
	glTexCoord2f(0, 0);		glVertex3f(-0.5, 0.5, 0.5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, SkyboxDownTexture);
	glBegin(GL_QUADS);
	//down
	glTexCoord2f(1, 0);		glVertex3f(0.5, -0.5, -0.5);
	glTexCoord2f(1, 1);		glVertex3f(0.5, -0.5, 0.5);
	glTexCoord2f(0, 1);		glVertex3f(-0.5, -0.5, 0.5);
	glTexCoord2f(0, 0);		glVertex3f(-0.5, -0.5, -0.5);

	glEnd();
	glPopMatrix();

}

void ControlCamera()
{
	if (keys['S'])
		MyCamera.MoveForward(-0.5);
	if (keys['W'])
		MyCamera.MoveForward(0.5);
	if (keys['D'])
		MyCamera.MoveRight(0.5);
	if (keys['A'])
		MyCamera.MoveRight(-0.5);
	if (keys[VK_RIGHT])
		MyCamera.RotateY(5);
	if (keys[VK_LEFT])
		MyCamera.RotateY(-5);
	if (keys[VK_UP])
		MyCamera.RotateX(5);
	if (keys[VK_DOWN])
		MyCamera.RotateX(-5);
}



Vector3 Direction = Vector3(0, 0, -1);
Vector3 Wing = Vector3(1, 0, 0);

float GetRho() {
	int indx = Jet.getLocation().gety() / 1000;
	indx = min(indx, 9);
	return rho[indx];
}

void GravityForce() {
	Vector3 Gravity = Vector3(0, -JetMass * gravityAcc / 20, 0);
	Jet.ApplyForce(Gravity);
}

void ReactionForce() {
	Vector3 Reaction = Vector3(0, JetMass * gravityAcc / 20, 0);
	Jet.ApplyForce(Reaction);
}

float ThrustX = 0, ThrustY = 0, ThrustZ = -1;
void ThrustForce() {
	if (keys['W']) {
		float Velocity = Jet.getVelocity().getmag() * Vector3::CosAlpha(Jet.getVelocity(), Vector3(ThrustX, ThrustY, ThrustZ));
		float Magnitude = GetRho()*V0*FanArea*(Ve - min(V0 + Velocity, Ve)) / 20;
		Vector3 Thrust = Vector3(ThrustX, ThrustY, ThrustZ);
		Thrust.mult(Magnitude);
		Jet.ApplyForce(Thrust);
	}
}

void DragForce() {
	float Velocity = Jet.getVelocity().getmag();
	float Magnitude = (0.5*Cd*GetRho()*WingArea*Velocity*Velocity) / 20;
	Magnitude += Brakes;
	Vector3 Drag = Vector3(Jet.getVelocity().getx(), Jet.getVelocity().gety(), Jet.getVelocity().getz());
	Drag.normalize();
	Drag.mult(-Magnitude);
	Jet.ApplyForce(Drag);
}

Vector3 GetLiftDirection() {
	float X = Direction.getz()*Wing.gety() - Direction.gety()*Wing.getz();
	float Y = -(Direction.getz()*Wing.getx() - Direction.getx()*Wing.getz());
	float Z = Direction.gety()*Wing.getx() - Direction.getx()*Wing.gety();
	if (Y < 0)
	{
		X *= -1;
		Y *= -1;
		Z *= -1;
	}
	return Vector3(X, Y, Z);
}

float LiftX = 0, LiftY = 1, LiftZ = 0;
void LiftForce() {
	float Velocity = Jet.getVelocity().getmag()*Vector3::CosAlpha(Jet.getVelocity(), Vector3(ThrustX, ThrustY, ThrustZ));
	float Magnitude = 0.5*Cl*GetRho()*WingArea*Velocity*Velocity / 20;

	Vector3 Lift = Vector3(LiftX, LiftY, LiftZ);
	Lift.mult(Magnitude);
	/*glLineWidth(8);
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	glVertex3f(MyCamera.Position.x + 15 * MyCamera.View.x, MyCamera.Position.y - 3, MyCamera.Position.z + 15 * MyCamera.View.z);
	glVertex3f(MyCamera.Position.x + 15 * MyCamera.View.x + Lift.getx(), MyCamera.Position.y - 3 + Lift.gety(), MyCamera.Position.z + 15 * MyCamera.View.z + Lift.getz());
	glEnd();*/

	Jet.ApplyForce(Lift);
}

float ToRadian(float degree)
{
	return degree*PI / 180;
}

float MyAssert(float val)
{
	val = max(val, -1);
	val = min(val, 1);
	return val;
}

Vector3 GetAngles(float X, float Y, float Z)
{
	float Theta = asin(MyAssert(Y));
	float alpha = acos(MyAssert(X));
	float delta = acos(MyAssert(Z));
	if (cos(Theta) != 0) {
		alpha = acos(MyAssert(X / cos(Theta)));
		delta = acos(MyAssert(Z / cos(Theta)));
	}
	if (Z > 0 && alpha != 0) alpha *= -1;
	if (X < 0 && delta != 0) delta *= -1;
	return Vector3(alpha, Theta, delta);
}

Vector3 GetAxis(Vector3 Angles)
{
	float X = cos(Angles.gety())*cos(Angles.getx());
	float Y = sin(Angles.gety());
	float Z = cos(Angles.gety())*cos(Angles.getz());
	return Vector3(X, Y, Z);
}

Vector3 RotateAround(Vector3 U, Vector3 V, float Angle)
{
	float a = V.getx(), b = V.gety(), c = V.getz();
	float r = ToRadian(Angle);
	float q0 = cos(r / 2);
	float q1 = sin(r / 2)*a;
	float q2 = sin(r / 2)*b;
	float q3 = sin(r / 2)*c;
	float Mat[3][3];
	Mat[0][0] = q0*q0 + q1*q1 - q2*q2 - q3*q3;
	Mat[0][1] = 2 * (q1*q2 - q0*q3);
	Mat[0][2] = 2 * (q1*q3 + q0*q2);
	Mat[1][0] = 2 * (q1*q2 + q0*q3);
	Mat[1][1] = q0*q0 - q1*q1 + q2*q2 - q3*q3;
	Mat[1][2] = 2 * (q2*q3 - q0*q1);
	Mat[2][0] = 2 * (q3*q1 - q0*q2);
	Mat[2][1] = 2 * (q3*q2 + q0*q1);
	Mat[2][2] = q0*q0 - q1*q1 - q2*q2 + q3*q3;
	float X = U.getx(), Y = U.gety(), Z = U.getz();
	float ResX = X*Mat[0][0] + Y*Mat[0][1] + Z*Mat[0][2];
	float ResY = X*Mat[1][0] + Y*Mat[1][1] + Z*Mat[1][2];
	float ResZ = X*Mat[2][0] + Y*Mat[2][1] + Z*Mat[2][2];
	return Vector3(ResX, ResY, ResZ);
}
void RightAndLeft()
{
	float Velocity = Jet.getVelocity().getmag();
	if (!Velocity) return;
	float ROT = 1091 * tan(ToRadian(Jet.GetRotateOnZ())) / Velocity;
	ROT /= 20;
	Jet.RotateOnY(Jet.GetRotateOnY() + ROT);
	MyCamera.RotateY(ROT);
	float ROTRadian = ToRadian(ROT);
	// Thrust Modification
	Vector3 TA = GetAngles(ThrustX, ThrustY, ThrustZ);
	TA.Add(Vector3(ROTRadian, 0, ROTRadian));
	Vector3 Res = GetAxis(TA);
	ThrustX = Res.getx(); ThrustY = Res.gety(); ThrustZ = Res.getz();

	// Wing Modification
	TA = GetAngles(Wing.getx(), Wing.gety(), Wing.getz());
	TA.Add(Vector3(ROTRadian, 0, ROTRadian));
	float WX = TA.getx(), WZ = TA.getz();
	Res = GetAxis(TA);
	Wing.setx(Res.getx()); Wing.sety(Res.gety()); Wing.setz(Res.getz());

	// Velocity Modification
	Vector3 JETVEL = Jet.getVelocity();
	float MAG = JETVEL.getmag();
	TA = GetAngles(JETVEL.getx()/MAG , JETVEL.gety()/MAG, JETVEL.getz()/MAG);
	TA.Add(Vector3(ROTRadian, 0, ROTRadian));
	Res = GetAxis(TA);
	Res.mult(MAG);
	Jet.SetVelocity(Res);
	
	JETVEL = Jet.getVelocity();
	MAG = JETVEL.getmag();
	if (keys[VK_RIGHT])
	{
		float NewAngle = Jet.GetRotateOnZ() - 1;
		NewAngle = max(NewAngle, -35);
		/*if (Jet.GetRotateOnZ() != -35) {
			Res = RotateAround(Vector3(JETVEL.getx()/MAG, JETVEL.gety() / MAG, JETVEL.getz() / MAG), Vector3(ThrustX, ThrustY, ThrustZ), 1);
			Res.mult(MAG);
			Jet.SetVelocity(Res);
		}*/
		Jet.RotateOnZ(NewAngle);

		// Lift Modification
		/*if (NewAngle == 0) { LiftX = 0; LiftY = 1; LiftZ = 0; }
		else {
			TA = GetAngles(LiftX, LiftY, LiftZ);
			if (NewAngle < 0) {
				TA.setx(WX);
				TA.sety(max(TA.gety() - ToRadian(1), ToRadian(55)));
				TA.setz(WZ);
			}
			else
			{
				TA.setx(WX + ToRadian(180));
				TA.sety(TA.gety() + ToRadian(1));
				TA.setz(WZ + ToRadian(180));
			}
			Res = GetAxis(TA);
			LiftX = Res.getx(); LiftY = Res.gety(); LiftZ = Res.getz();

		}*/

	}
	if (keys[VK_LEFT])
	{
		float NewAngle = Jet.GetRotateOnZ() + 1;
		NewAngle = min(NewAngle, 35);
		/*if (Jet.GetRotateOnZ() != 35) {
			Res = RotateAround(Vector3(JETVEL.getx() / MAG, JETVEL.gety() / MAG, JETVEL.getz() / MAG), Vector3(ThrustX, ThrustY, ThrustZ), -1);
			Res.mult(MAG);
			Jet.SetVelocity(Res);
		}*/
		Jet.RotateOnZ(NewAngle);

		// Lift Modification
		/*if (NewAngle == 0) { LiftX = 0; LiftY = 1; LiftZ = 0; }
		else {
			TA = GetAngles(LiftX, LiftY, LiftZ);
			if (NewAngle > 0) {
				TA.setx(WX + ToRadian(180));
				TA.sety(max(TA.gety() - ToRadian(1), ToRadian(55)));
				TA.setz(WZ + ToRadian(180));
			}
			else
			{
				TA.setx(WX);
				TA.sety(TA.gety() + ToRadian(1));
				TA.setz(WZ);
			}
			Res = GetAxis(TA);
			LiftX = Res.getx(); LiftY = Res.gety(); LiftZ = Res.getz();
		}
		*/

	}

}

void LeftAndRight()
{
	if (Jet.getVelocity().getmag() == 0) return;
	Vector3 VL = GetAngles(Jet.getVelocity().getx()/Jet.getVelocity().getmag(), Jet.getVelocity().gety() / Jet.getVelocity().getmag(), Jet.getVelocity().getz() / Jet.getVelocity().getmag());
	Vector3 TL = GetAngles(ThrustX, ThrustY, ThrustZ);
	float Angle = VL.getx() - TL.getx();
	if (Angle != 0) {
		Jet.RotateOnY(Jet.GetRotateOnY() + Angle * 180 / PI);
		MyCamera.RotateY(Angle * 180 / PI);

		// Thrust Modification
		TL.Add(Vector3(Angle, 0, Angle));
		Vector3 Res = GetAxis(TL);
		ThrustX = Res.getx(); ThrustY = Res.gety(); ThrustZ = Res.getz();
		// Lift Modification
		TL = GetAngles(LiftX, LiftY, LiftZ);
		TL.Add(Vector3(Angle, 0, Angle));
		Res = GetAxis(TL);
		LiftX = Res.getx(); LiftY = Res.gety(); LiftZ = Res.getz();
	}

	if (keys[VK_RIGHT])
	{
		if (Jet.GetRotateOnZ() != -35) {
			Vector3 X = RotateAround(Vector3(LiftX, LiftY, LiftZ), Vector3(ThrustX, ThrustY, ThrustZ), 1);
			LiftX = X.getx(); LiftY = X.gety(); LiftZ = X.getz();
		}
		float NewAngle = Jet.GetRotateOnZ() - 1;
		NewAngle = max(NewAngle, -35);
		Jet.RotateOnZ(NewAngle);

		
	}
	if (keys[VK_LEFT])
	{
		if (Jet.GetRotateOnZ() != 35) {
			Vector3 X = RotateAround(Vector3(LiftX, LiftY, LiftZ), Vector3(ThrustX, ThrustY, ThrustZ), -1);
			LiftX = X.getx(); LiftY = X.gety(); LiftZ = X.getz();
		}
		float NewAngle = Jet.GetRotateOnZ() + 1;
		NewAngle = min(NewAngle, 35);
		Jet.RotateOnZ(NewAngle);

	}
}

void SlowDown()
{
	if (Jet.getVelocity().getmag() <= 0.1) return;
	if (keys['S'])
	{
		Brakes = 1000;
	}
	else Brakes = 0;
}

void DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	MyCamera.Render();
	DrawStreet();
	DrawGround();
	DrawSkyBox();

	GravityForce();
	ThrustForce();		
	DragForce();
	LiftForce();
	LeftAndRight();
	SlowDown();
	SoundControl();


	

	Jet.Update();
	
	glFlush();											// Done Drawing The Quad	

														//DO NOT REMOVE THIS
	SwapBuffers(hDC);
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*
*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	static PAINTSTRUCT ps;

	switch (uMsg)									// Check For Windows Messages
	{
	case WM_PAINT:
		DrawGLScene();
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_TIMER:
		DrawGLScene();
		return 0;

	case WM_ACTIVATE:							// Watch For Window Activate Message
	{
		if (!HIWORD(wParam))					// Check Minimization State
		{
			active = TRUE;						// Program Is Active
		}
		else
		{
			active = FALSE;						// Program Is No Longer Active
		}

		return 0;								// Return To The Message Loop
	}

	case WM_SYSCOMMAND:							// Intercept System Commands
	{
		switch (wParam)							// Check System Calls
		{
		case SC_SCREENSAVE:					// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
			return 0;							// Prevent From Happening
		}
		break;									// Exit
	}

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done = FALSE;								// Bool Variable To Exit Loop

														// Ask The User Which Screen Mode They Prefer
														//if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen = FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Madhat NeHe Template", 640, 480, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}


	//Set drawing timer to 20 frame per second
	UINT timer = SetTimer(hWnd, 0, 50, (TIMERPROC)NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;

}
