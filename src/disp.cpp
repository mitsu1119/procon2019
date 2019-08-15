#include"disp.hpp"

// ---------------------------------------- DisplayWrapper ----------------------------------------

DisplayWrapper::DisplayWrapper(){
	random = new Random();
	astar = new Astar();
}

DisplayWrapper::~DisplayWrapper(){
	delete instance;
	delete field;
	
	delete random;
	delete astar;
}

DisplayWrapper* DisplayWrapper::instance=0;

void DisplayWrapper::init(){
	instance->initInstance();
	gluOrtho2D(0, 100, 100, 0);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowPosition(this->window_width_position, this->window_height_position);
	glutInitWindowSize(this->window_width_size, this->window_height_size);
	glClearColor(1.0, 1.0, 1.0, 1.0);
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
	instance = framework;
}


void DisplayWrapper::setField(Field* object){
	field = object;
}

/*
void DisplayWrapper::reverseBoard(Field& field){
	std::for_each(field.field.begin(), field.field.end(), [this](auto& panel){
			if(panel.isEnemyPanel()){
				panel.setPure();
				panel.setMine();
			}
			else{
				if(panel.isMyPanel()){
					panel.setPure();
					panel.setEnemy();
				}
			}
		});
	std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& a){
			a.reverseAttr();
		});
}	
*/

void DisplayWrapper::line() const{
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(line_size);
	glBegin(GL_LINES);
	for(int i=0;i<=this->field->width;i++){
		glVertex2i(this->cell_size*i, 0);
		glVertex2i(this->cell_size*i, this->cell_size*this->field->height);
	}
	for(int j=0;j<=this->field->height;j++){
		glVertex2i(0, this->cell_size*j);
		glVertex2i(this->cell_size*this->field->width, this->cell_size*j);
	}
	glEnd();
}

void DisplayWrapper::score() const{
	glColor3f(0.0f, 0.0f, 0.0f);
	for(int i=0;i<this->field->width;i++){
		for(int j=0;j<this->field->height;j++){
			int value=this->field->at(i, j)->getValue();
			std::string str=std::to_string(value);
			this->renderString(i*this->cell_size+5, (j+1)*this->cell_size-5, str);
		}
	}
}

void DisplayWrapper::panel() const{
	glPointSize(this->agent_size);
	const int half=this->cell_size/2;
		for(int i=0;i<this->field->width;i++){
			for(int j=0;j<this->field->height;j++){;
			if(this->field->at(i, j)->isPurePanel())
				continue;
			if(this->field->at(i, j)->isMyPanel())
				glColor3f(0.6f, 0.6f, 1.0f);
			if(this->field->at(i, j)->isEnemyPanel())
				glColor3f(1.0f, 0.6f, 0.6f);
			glBegin(GL_POINTS);
			glVertex2i(half+this->cell_size*i, half+this->cell_size*j);
			glEnd();
		}
	}
}

void DisplayWrapper::agent() const{
	const int half=this->cell_size/2;
	uint_fast32_t flag;
	
	glPointSize(this->agent_size);
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			flag = a.getAttr();
			if(flag == MINE_ATTR)
				glColor3f(0.0f, 0.0f, 0.8f);
			if(flag == ENEMY_ATTR)
				glColor3f(0.8f, 0.0f, 0.0f);
			glBegin(GL_POINTS);
			glVertex2i(half+cell_size*a.getX(), half+cell_size*a.getY());
			glEnd();
		});

	static int mine, enemy;
	mine=0;
	enemy=0;
	glColor3f(0.0f, 0.0f, 0.0f);
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			flag = a.getAttr();
			if(flag == MINE_ATTR){
				mine++;
				std::string value=std::to_string(mine);
				std::string str="M"+value;
				this->renderString(half+cell_size*a.getX()-2, half+cell_size*a.getY(), str);
			}
			if(flag == ENEMY_ATTR){
				enemy++;
				std::string value=std::to_string(enemy);
				std::string str="E"+value;
				this->renderString(half+cell_size*a.getX()-2, half+cell_size*a.getY(), str);
			}
		});
}

void DisplayWrapper::point() const{
	;
}

void DisplayWrapper::renderString(float x, float y, const std::string& str) const{
	float z = 0.0f;
	glRasterPos3f(x, y, z);
	for(int i = 0; i < (int)str.size(); ++i){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	}
}

// ---------------------------------------- Display ----------------------------------------

Display::Display() : flag(0) ,mine_flag(0), enemy_flag(0) {
}

Display::~Display(){	
}

void Display::makePossibleList(){
	this->possible_list.clear();
	std::vector<std::pair<int, int>> coord;
	std::vector<int> vec_x={0,1,1,1,0,-1,-1,-1,0};
	std::vector<int> vec_y={-1,-1,0,1,1,1,0,-1,0};
	for(int i=0;i<vec_x.size();i++){
		coord.push_back(std::make_pair(vec_x.at(i), vec_y.at(i)));
	}
	int x, y, bufX, bufY;
	std::vector<std::pair<int, int>> possible;
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			bufX=a.getX();
			bufY=a.getY();
			std::for_each(coord.begin(), coord.end(), [&, this](auto& c){
					x=bufX+c.first;
					y=bufY+c.second;
					possible.push_back(std::make_pair(x, y));
				});
			this->possible_list.push_back(possible);
			possible.clear();
		});
	//this->next_list.clear();
	//this->next_list.resize(this->field->agents.size());
	std::fill(this->next_list.begin(), this->next_list.end(), STOP);
	this->mine_flag=0;
	this->enemy_flag=0;
	this->flag=0;
}

void Display::moveNextList(){
	int buf=0;
	Direction b;
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			if(this->field->canMove(a, this->next_list.at(buf))){
				a.move(this->next_list.at(buf));
			}
			else{
				a.move(STOP);
			}
			buf++;
		});
	this->field->applyNextAgents();
}

void Display::initInstance(){
	this->next_list.clear();
	this->next_list.resize(this->field->agents.size());
	std::fill(this->next_list.begin(), this->next_list.end(), STOP);
	this->makePossibleList();
}

void Display::resize(int w, int h){
	glViewport(0, 0, w, h);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}
	
void Display::display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->line();
	this->panel();
	this->agent();
	this->candidate();
	this->score();
	glFlush();
}

void Display::keyboard(unsigned char key, int x, int y){
	switch(key){
	case 'q':
	case 'Q':
	case '\033':
		
		std::exit(0);
		break;
		
	case 'w':
	case 'W':
		
		this->makePossibleList();
		this->field->testMoveAgent();
		this->field->print();
		glutPostRedisplay();
		break;
	case 'm':
	case 'M':

		this->makePossibleList();
		this->random->move(this->field, MINE_ATTR);
		this->random->move(this->field, ENEMY_ATTR);
		field->applyNextAgents();		
		this->field->print();
		glutPostRedisplay();
		break;
		
	case 'g':
	case 'G':
		
		this->makePossibleList();
		this->moveNextList();
		this->field->print();
		glutPostRedisplay();
		break;


		//実験用
	case 't':
	case 'T':
		
		this->field->genRandMap();
		this->astar->move(this->field, MINE_ATTR);
		this->astar->move(this->field, MINE_ATTR);
		glutPostRedisplay();		
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
	const bool range=(x<0||x>cell_size*this->field->width||y<0||y>cell_size*this->field->height);
	if(!launch||range){
		return;
		std::cout<<"error"<<std::endl;
	} 
	const int coordX=x/cell_size;
	const int coordY=y/cell_size;
	std::cout<<"("<<coordX+1<<","<<coordY+1<<")"<<std::endl;

	//動かすエージェントの選択処理
	int buf=0,count_mine=0,count_enemy=0;
	for(int i=0;i<this->field->agents.size();i++){
		if(this->field->agents.at(i).getX()==coordX&&this->field->agents.at(i).getY()==coordY){
			if(this->field->agents.at(i).getAttr()==ENEMY_ATTR){
				this->flag=1;
				this->enemy_flag=i;
				buf=1;
				count_enemy++;
				std::cout<<"Enemy"<<count_enemy<<" agent"<<std::endl;
			}
			if(this->field->agents.at(i).getAttr()==MINE_ATTR){
				this->flag=0;
				this->mine_flag=i;
				buf=1;
				count_mine++;
				std::cout<<"Mine"<<count_mine<<" agent"<<std::endl;
			}
		}
	}
	if(buf)  return;

	int hoge;
	//動かす方向の選択処理
	std::pair<int, int> coord=std::make_pair(coordX, coordY);
	if(flag){
		for(int i=0;i<this->possible_list.at(enemy_flag).size();i++){
			if(this->possible_list.at(enemy_flag).at(i)==coord){
				this->next_list.at(enemy_flag)=(Direction)i;
				glutPostRedisplay();
				return;
			}
		}
	}
	else{
		for(int i=0;i<this->possible_list.at(mine_flag).size();i++){
			if(this->possible_list.at(mine_flag).at(i)==coord){
				this->next_list.at(mine_flag)=(Direction)i;
				glutPostRedisplay();				
				return;
			}
		}
	}
	
	std::cout<<"error"<<std::endl;
}

void Display::candidate() const{
	std::vector<int> vec_x={0,1,1,1,0,-1,-1,-1,0};
	std::vector<int> vec_y={-1,-1,0,1,1,1,0,-1,0};
	Direction buf;
	int coordX, coordY;
	const int half=this->cell_size/2;
	glPointSize(agent_size-8);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	for(int i=0;i<this->field->agents.size();i++){
		buf=this->next_list.at(i);
		coordX=this->field->agents.at(i).getX()+vec_x.at((int)buf);
		coordY=this->field->agents.at(i).getY()+vec_y.at((int)buf);
		glVertex2i(half+cell_size*coordX, half+cell_size*coordY);
	}
	glEnd();
}

void Display::motion(int x, int y){
	;
}

selfDirectedGame::selfDirectedGame(){	
}

selfDirectedGame::~selfDirectedGame(){
}

void selfDirectedGame::resize(int w, int h){
	;
}

void selfDirectedGame::display(){
	;
}

void selfDirectedGame::keyboard(unsigned char key, int x, int y){
	;
}

void selfDirectedGame::specialKeyboard(int key, int x, int y){
	;
}

void selfDirectedGame::mouse(int button, int state, int x, int y){
	;
}

void selfDirectedGame::motion(int x, int y){
	;
}

