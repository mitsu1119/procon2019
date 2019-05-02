#include"disp.hpp"

// ---------------------------------------- DisplayWrapper ----------------------------------------

DisplayWrapper* DisplayWrapper::instance=0;

void DisplayWrapper::init(){
	gluOrtho2D(0, 100, 100, 0);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowPosition(this->window_width_position, this->window_height_position);
	glutInitWindowSize(this->window_width_size, this->window_height_size);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void DisplayWrapper::start(int argc, char *argv[]){
	glutInit(&argc, argv);
  glutCreateWindow("procon2019");
	init();
	glutDisplayFunc(displayWrapper);
	glutKeyboardFunc(keyboardWrapper);
	glutReshapeFunc(resizeWrapper);
	glutMouseFunc(mouseWrapper);
	glFlush();
	glutMainLoop();
}

void DisplayWrapper::resizeWrapper(int w, int h){
	instance->resize(w,h);
}

void DisplayWrapper::displayWrapper(){
	instance->display();	
}

void DisplayWrapper::keyboardWrapper(unsigned char key, int x, int y){
	instance->keyboard(key, x, y);
}

void DisplayWrapper::specialKeyboardWrapper(int key, int x, int y){
	instance->specialKeyboard(key, x, y);	
}

void DisplayWrapper::mouseWrapper(int button, int state, int x, int y){
	instance->mouse(button, state, x, y);
}

void DisplayWrapper::motionWrapper(int x, int y){
	instance->motion(x, y);
}

void DisplayWrapper::setInstance(DisplayWrapper* framework){
	instance=framework;
}

void DisplayWrapper::setField(Field* object){
	field=object;
}

// ---------------------------------------- Display ----------------------------------------

void Display::resize(int w, int h){
	glViewport(0, 0, w, h);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}
	
void Display::display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->line();
	//	this->panel();
	this->score();
	this->agent();
	glFlush();
}

void Display::keyboard(unsigned char key, int x, int y){
	switch(key){
	case 'q':
	case 'Q':
	case '\033':
		std::exit(0);
	case 'c':
	case 'C':
		this->field->print();
		std::cout<<std::endl;
		break;
	default:
		break;
	}	
}

void Display::specialKeyboard(int key, int x, int y){
	;
}

void Display::mouse(int button, int state, int x, int y){
	const bool launch=(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN);
	const bool range=(x<0||x>cell_size*this->field->getWidth()||y<0||y>cell_size*this->field->getHeight());
	if(!launch||range)  return;
	const int coordX=x/cell_size+1;
	const int coordY=y/cell_size+1;
	std::cout<<std::endl<<"("<<coordX<<","<<coordY<<")";
}

void Display::motion(int x, int y){
	;
}
	
const void Display::line(){
	glColor3f(0.0f, 1.0f, 0.0);
	glLineWidth(line_size);
	glBegin(GL_LINES);
	for(int i=0;i<=this->field->getWidth();i++){
		glVertex2i(this->cell_size*i, 0);
		glVertex2i(this->cell_size*i, this->cell_size*this->field->getHeight());
	}
	for(int j=0;j<=this->field->getHeight();j++){
		glVertex2i(0, this->cell_size*j);
		glVertex2i(this->cell_size*this->field->getWidth(), this->cell_size*j);
	}
	glEnd();
}

const void Display::score(){
	glColor3f(0.0f, 1.0f, 0.0);
	for(int i=0;i<this->field->getWidth();i++){
		for(int j=0;j<this->field->getHeight();j++){
			int value=this->field->at(i, j)->getValue();
			std::string str=std::to_string(value);
			this->renderString(i*this->cell_size+5, (j+1)*this->cell_size-5, str);
		}
	}
}

const void Display::panel(){
	
}

const void Display::agent(){
	
}

const void Display::renderString(float x, float y, const std::string& str){
	float z = 0.0f;
	glRasterPos3f(x, y, z);
	for(int i = 0; i < (int)str.size(); ++i){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	}
}
