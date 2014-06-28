#if defined(__APPLE__)
	#include <OpenGL/OpenGl.h>
	#include <GLUT/GLUT.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/freeglut.h>
#endif

#include <stdio.h>
#include <vector>
#include "model.h"
#include <math.h>
using namespace std;

bool visible;
int eixx, eixy, behaviour,cam,first,tafaner,vert,campos,light,light0,light1,light2;
double value,posx,posy,rotx,roty,homx,homz,aspect,fovy,fovw,bottom,top,left,right,dist,radi,escalator,wi,le,auxfov,lposx,lposz;
float xv,zv,acceleration,refX,refZ,angy,xl,zl,angaux;
Model lego;

void ajuda(void){
	fprintf(stdout, "Ajuda General\n");
	fprintf(stdout, "--------------\n");
	fprintf(stdout, "ESC: Sortir de la aplicació\n");
	fprintf(stdout, "h: mostrar ajuda\n");
	fprintf(stdout, "p: selecciona la camara axonometrica/perspectiva\n");
	fprintf(stdout, "v: fa visibles/invisibles les parets\n");
	fprintf(stdout, "r: torna la camara al estat inicial\n");
	fprintf(stdout, "c: activa/desactiva el mode de vista 'primera persona'\n");
	fprintf(stdout, "i: activa/desactiva la il·luminació\n");
	fprintf(stdout, "0: activa/desactiva GL_LIGHT0 (camara de llum groguenca)\n");
	fprintf(stdout, "1: activa/desactiva GL_LIGHT1 (llum de camara)\n");
	fprintf(stdout, "2: activa/desactiva GL_LIGHT2 (camara solidaria amb el cotxe)\n");
	fprintf(stdout, "m: canvia la posició de la GL_LIGHT0\n");
	fprintf(stdout, "n: alterna entre normal per cara / normal per vertex\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Ajuda Mode primera persona\n");
	fprintf(stdout, "---------------------------\n");
	fprintf(stdout, "w: avançar\n");
	fprintf(stdout, "s: retrocedir\n");
	fprintf(stdout, "d: girar cap a la dreta\n");
	fprintf(stdout, "a: girar cap a l'esquerra\n");
	fprintf(stdout, "z: augmentar la velocitat de moviment\n");
	fprintf(stdout, "x: disminuir la velocitat de moviment\n");
	fprintf(stdout, "t: activar/desactivar mode tafaner\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Ajuda Mode tafaner\n");
	fprintf(stdout, "-------------------\n");
	fprintf(stdout, "->: girar l'angle de visio cap a la dreta\n");
	fprintf(stdout, "<-: girar l'angle de visio cap a l'esquerra\n");
}


void start(void){
	eixx = eixy = behaviour = cam = first = vert = 0;
	visible = true;
	value = posx = rotx = roty = homx = homz= auxfov = 0;
	aspect = 1;
	radi = sqrt((5*5) + (5*5) + (1.5*1.5));
	dist = radi+4;
	float temp = (atan( tan( asin( radi/dist )))/aspect);
	fovy = 2*temp;
    fovw = 70;
	bottom = left = top = right = radi;
	acceleration = 0.1;
	angy = angaux = 0;
	lposx = lposz = 5;
	campos = light0 = light2 = 0;
	light = light1 = 1;
	zv = 0;
	xv = 0;
	refX = xl = 0;
	refZ = zl = 1;
}

vector<double> caixaMinima(Model lego){

	vector<double> caixa(6);
	caixa[0] = lego.vertices()[0];
    caixa[1] = lego.vertices()[0];
   	caixa[2] = lego.vertices()[1];
   	caixa[3] = lego.vertices()[1];
   	caixa[4] = lego.vertices()[2];
   	caixa[5] = lego.vertices()[2];

    for(int i=3; i < lego.vertices().size(); i=i+3){
    	if(lego.vertices()[i] < caixa[0])  caixa[0] = lego.vertices()[i];
    	if(lego.vertices()[i] > caixa[1])  caixa[1] = lego.vertices()[i];
    	if(lego.vertices()[i+1] < caixa[2])  caixa[2] = lego.vertices()[i+1];
    	if(lego.vertices()[i+1] > caixa[3])  caixa[3] = lego.vertices()[i+1];
    	if(lego.vertices()[i+2] < caixa[4])  caixa[4] = lego.vertices()[i+2];
    	if(lego.vertices()[i+2] > caixa[5])  caixa[5] = lego.vertices()[i+2];

	}
	return caixa;
}

void llum0(double x, double z){
	GLfloat pos[] = {x,0.5,z,1};
	glLightfv(GL_LIGHT0,GL_POSITION,pos);
}

void transformacions(void){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(first){
		vector<double> caixa = caixaMinima(lego);
		double aux = (caixa[3] - caixa[2]);
		double scale = 1.5/(caixa[5]-caixa[4]);
		if(!tafaner) gluLookAt(xv,scale*aux,zv,xv + refX,scale*aux,zv+refZ,0,1,0);
		else gluLookAt(xv,scale*aux,zv,xv+xl,scale*aux,zv+zl,0,1,0);
		
		glPushMatrix();
		glLoadIdentity();
		GLfloat pos[] = {0,0,-0.6,1};
		glLightfv(GL_LIGHT2,GL_POSITION,pos);
		glPopMatrix();
	}
	else {
		glTranslated(0,0,-dist);
		glRotated(roty,1,0,0);
		glRotated(rotx,0,1,0);
		GLfloat pos[] = {xv,0.8,zv,1};
		glLightfv(GL_LIGHT2,GL_POSITION,pos);
	}
	llum0(lposx,lposz);

}

void camara(void){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(first) gluPerspective(fovw,aspect,0.01,2*radi);
	else {
		if(!cam) glOrtho(-left,right,-bottom,top,dist-radi,dist+radi);
		else gluPerspective(((fovy)*180/M_PI),aspect,dist-radi,dist+radi);
	}
}

void iluminacio(void){

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT2);

	GLfloat colorLight[] = {1,1,0,1};
	glLightfv(GL_LIGHT0,GL_SPECULAR,colorLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,colorLight);

	GLfloat colorLight1[] = {1,1,1,1};
	glLightfv(GL_LIGHT1,GL_SPECULAR,colorLight1);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,colorLight1);

	GLfloat colorLight2[] = {1,1,1,1};
	glLightfv(GL_LIGHT2,GL_SPECULAR,colorLight2);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,colorLight2);

}

void reset(void){
	start();
	camara();
	transformacions();
	iluminacio();

}


void pintaModel(Model lego){

	vector <double> caixa = caixaMinima(lego);
	double x = ((caixa[1] + caixa[0])/2) ;
	double y = ((caixa[3] + caixa[2])/2) ;
	double z = ((caixa[5] + caixa[4])/2) ;
	double scale = 1.5/(caixa[5]-caixa[4]);
	
	glTranslated(homx,0,homz);
	glTranslated(xv,0,zv);
	glTranslated(0,- (scale*(caixa[2]-y))/*imprecisio?*/ +0.0001 /*imprecisio?*/,0);
	glRotated((angy*180/M_PI),0,1,0);
	glScaled(scale,scale,scale);
	glTranslated(-x,-y,-z);
	int mida = lego.faces().size();

    for(int i=0; i < mida; ++i){

    	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    	const Face &FTemp = lego.faces()[i];
    	glColor4fv(Materials[FTemp.mat].diffuse);
    	if(light) {
    	    GLfloat black[] = {0,0,0,1};
    		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,Materials[FTemp.mat].diffuse);
    		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,Materials[FTemp.mat].ambient);
    		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
    	}
       	glBegin(GL_TRIANGLES);
       	
       	if(!vert) glNormal3dv(FTemp.normalC);

       	if(vert) glNormal3dv(&lego.normals()[FTemp.n[0]]);
    	glVertex3dv(&lego.vertices()[FTemp.v[0]]);
    	
    	if(vert) glNormal3dv(&lego.normals()[FTemp.n[1]]);
		glVertex3dv(&lego.vertices()[FTemp.v[1]]);
		
		if(vert) glNormal3dv(&lego.normals()[FTemp.n[2]]);
		glVertex3dv(&lego.vertices()[FTemp.v[2]]);
       	
       	glEnd();
	}
}

void pintaNinot(void) {

	glPushMatrix();

	glColor3f(1,1,1);
	if(light){
		GLfloat white[] = {1, 1, 1, 1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,white);
    		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	glutSolidSphere(0.4,35,35);

	/*-------------------------------*/// mod|id|////

	glColor3f(1,1,1);
	
	if(light){
		GLfloat white[] = {1, 1, 1, 1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,white);
    		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
   	}
	glTranslated(0,0.6,0);
	glutSolidSphere(0.2,35,35);

	glRotated(90,0,1,0);
	glColor3f(1,0,0);

	/*-------------------------------*////mod|id|///

	glPopMatrix();
	glPushMatrix();
	glTranslated(0.1,0.6,0);
	glRotated(90,0,1,0);

	glColor3f(1,0.2,0);
	if(light){
		GLfloat orange[] = {1,0.2,0, 1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,orange);
    		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,orange);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	glutSolidCone(0.1,0.2,35,35);

	glPopMatrix();
}

void pintaQuad(double qdist, int i, int j){
	glPushMatrix();
	glTranslated(-5+qdist+i,0,-5+qdist+j);
	glNormal3f(0,1,0);
	glBegin(GL_QUADS);
		glVertex3f(-qdist,0,-qdist);	
		glVertex3f(qdist,0,-qdist);
		glVertex3f(qdist,0,qdist);
		glVertex3f(-qdist,0,qdist);
	glEnd();
	glPopMatrix();
}

void pinta10000(void) {
	for (int i=0; i<1; ++i) {
        for (int j=0; j<1; ++j) {
        	pintaQuad(1,i,j);
        }
    }
}

void pintaTerra(void){
	glColor3f(0,0,0.9);
	if(light){
		GLfloat blue[] = {0,0,0.9,1};
		GLfloat white[] = {1,1,1,1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,blue);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	pinta10000();
}

void pintaParets(void) {

	glPushMatrix();
	
	glTranslated(2.5,0.75+0.0001,-1.5);
	glScalef(4,1.5,0.2);
	
	glColor3f(0,0.5,0);
	if(light){
		GLfloat black[] = {0,0,0,1};
		GLfloat green[] = {0,0.5,0, 1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,green);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	glutSolidCube(1);
	
	glColor3f(0.2,0.2,0.2);
	if(light){
		GLfloat trace[] = {0.2,0.2,0.2, 1};
		GLfloat black[] = {0,0,0,1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,trace);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	glutWireCube(1);
	
	glPopMatrix();
	glPushMatrix();

	glTranslated(-4.9,0.75+0.0001,0);
	glScalef(0.2,1.5,10);
	
	glColor3f(0,0.5,0);
	if(light){
		GLfloat black[] = {0,0,0,1};
		GLfloat green[] = {0,0.5,0, 1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,green);
   		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	glutSolidCube(1);
	
	glColor3f(0.2,0.2,0.2);
	if(light){
		GLfloat trace[] = {0.2,0.2,0.2, 1};
		GLfloat black[] = {0,0,0,1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,trace);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,black);
		glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
	}
	glutWireCube(1);

	glPopMatrix();
}

void pinta4Ninots(void) {
	glPushMatrix();
	glTranslated(+2.5,0.4,+2.5);
	pintaNinot();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2.5,0.4,-2.5);
	pintaNinot();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2.5,0.4,+2.5);
	pintaNinot();
	glPopMatrix();

	glPushMatrix();
	glTranslated(2.5,0.4,-2.5);
	pintaNinot();
	glPopMatrix();
}


void refresh(void){

	glPushMatrix();//per la seguent vegada que entri a la funcio

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1,1,1);
	//glutWireSphere(radi,30,30);

	pintaTerra();

	if(visible) pintaParets();
 
	pinta4Ninots();

	pintaModel(lego);
	glPopMatrix();

	glutSwapBuffers();


}

void reshape(int width,int length){

	glViewport(0, 0, width, length); 
	wi = width; 
	le = length; 
	aspect = (wi/le);

	if (wi>=le) { 
		right = top*aspect;
		left = bottom*aspect;
	} else { 
		float temp = atan( tan( asin( radi/dist ) )/aspect);
		fovy=2*temp; 
		top = right/aspect;
		bottom = left/aspect;
	}
	camara();
	transformacions();
}

void charFunc(unsigned char c, int x, int y){

	if(c == 'p') {
		cam = 1 - cam;
		camara();
		transformacions();
	}
	if(c == 'c'){
		first = 1 - first;
		camara();
		transformacions();
	}
	else if (c == 27) exit(0);
	else if (c == 'v') visible = 1 - visible;
	else if (c == 'h') ajuda();
	else if (c == 'r') reset();
	else if (c == 'n') vert = 1 - vert;
	else if (c == 'i') {
		light = 1-light;
		if(!light) glDisable(GL_LIGHTING);
		else glEnable(GL_LIGHTING);
	}
	else if (c == '0') {
		light0 = 1-light0;
		if(!light0) glDisable(GL_LIGHT0);
		else glEnable(GL_LIGHT0);
	}
	else if (c == '1') {
		light1 = 1-light1;
		if(!light1) glDisable(GL_LIGHT1);
		else glEnable(GL_LIGHT1);
	}
	else if (c == '2') {
		light2 = 1-light2;
		if(!light2) glDisable(GL_LIGHT2);
		else glEnable(GL_LIGHT2);
	}
	else if (c == 'm') {
		campos = (campos+1)%5;
		if(campos == 0) lposx = lposz = 5;
		else if(campos == 1) lposz = -5;
		else if(campos == 2) lposx = -5;
		else if(campos == 3) lposz = 5;
		else if(campos == 4) lposx = lposz = 0;
		llum0(lposx,lposz);
	}
	if(first) {
		if (c == 'w' && xv+refX >= -5 && xv+refX <= 5 && zv+refZ >= -5 && zv+refZ <= 5) {
			xv += refX*acceleration;
			zv += refZ*acceleration;
		}
		else if (c == 's' && xv-refX >= -5 && xv-refX <= 5 && zv-refZ >= -5 && zv-refZ <= 5) {
			xv -= refX*acceleration;
			zv -= refZ*acceleration;
		}
		else if (c == 'd') {
			angy -= 0.03;
            refX = sin(angy);
            refZ = cos(angy);

            angaux -= 0.03;
            xl = sin(angaux);
			zl = cos(angaux);
        }
		else if (c == 'a') {
			angy += 0.03;
			refX = sin(angy);
            refZ = cos(angy);

            angaux += 0.03;
            xl = sin(angaux);
			zl = cos(angaux);
		}
		else if (c == 'z' && acceleration < 0.94) {
			acceleration += 0.05;
		}
		else if (c == 'x' && acceleration > 0.06) {
			acceleration -= 0.05;
		}
		else if(c == 't') tafaner = 1 -tafaner; 

		camara();
		transformacions();
	}



	glutPostRedisplay();
}

void capture(int boto, int estat, int x, int y)
{
	if(boto == GLUT_LEFT_BUTTON && estat == GLUT_DOWN) {
		posx = x;
		posy = y;
	}
}

void move(int x, int y)
{
	if(GLUT_ACTIVE_SHIFT == glutGetModifiers()) {

		if(y > posy && (fovy+0.5*2*M_PI/360) < (100*M_PI/180)) {
			fovy+=0.5*2*M_PI/360;
			left=dist*sin(fovy/2);
			right=dist*sin(fovy/2);
			top=dist*sin(fovy/2);
			bottom=dist*sin(fovy/2);
		}

		else if(y < posy && (fovy-0.5*2*M_PI/360) > 0) {
			fovy-=0.5*2*M_PI/360;
			left=dist*sin(fovy/2);
			right=dist*sin(fovy/2);
			top=dist*sin(fovy/2);
			bottom=dist*sin(fovy/2);
		}
		
		if (aspect >= 1) { 
			right = top*aspect;
			left = bottom*aspect;
		} else {  
			top = right/aspect;
			bottom = left/aspect;
		}
		camara();

	}

	else {

		if(x > posx) rotx += 1; 
		else if(x < posx) rotx -= 1;
		
		if(y > posy) roty += 1;
		else if(y < posy) roty -= 1;
	}

	posx = x;
	posy = y;
	
	transformacions();
	glutPostRedisplay();

}
void specialDetect(int key, int x, int y){
	if(key == GLUT_KEY_LEFT){
		angaux +=  0.3;

		xl = sin(angaux);
		zl = cos(angaux);
	}
	else if(key == GLUT_KEY_RIGHT){
		angaux -=  0.3;

		xl = sin(angaux);
		zl = cos(angaux);
	}
	transformacions();
	glutPostRedisplay();
}

int main(int argc, const char* argv[]){

	glutInit(&argc, (char **) argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600,600);
	glutCreateWindow("IDI: Entrega 2 OpenGL");

	start();

	lego.load("porsche.obj");
	glEnable(GL_DEPTH_TEST);

	iluminacio();

	glEnable(GL_NORMALIZE);
	camara();
	transformacions();

	glutDisplayFunc(refresh);
	glutReshapeFunc(reshape);
	glutMotionFunc(move);
	glutMouseFunc(capture);
	glutKeyboardFunc(charFunc);
	glutSpecialFunc(specialDetect);

	glutMainLoop();

	return 0;
}
