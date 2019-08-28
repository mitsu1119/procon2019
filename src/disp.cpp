#include"disp.hpp"

// ---------------------------------------- Print ----------------------------------------
Print::Print(){	
}

Print::~Print(){
}

void Print::renderString(float x, float y, const std::string& str) const{
	float z = 0.0f;
	glRasterPos3f(x, y, z);
	for(size_t i = 0; i < (int_fast32_t)str.size(); ++i){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	}
}

void Print::line(const Field* field) const{
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(line_size);
	glBegin(GL_LINES);
	for(size_t i = 0; i <= field->getWidth(); i++){
		glVertex2i(cell_size * i, 0);
		glVertex2i(cell_size * i, cell_size * field->getHeight());
	}
	for(size_t j = 0; j <= field->getHeight(); j++){
		glVertex2i(0, cell_size * j);
		glVertex2i(cell_size * field->getWidth(), cell_size * j);
	}
	glEnd();
}

void Print::score(const Field* field) const{
	int_fast32_t value = 0;
	glColor3f(0.0f, 0.0f, 0.0f);
	for(size_t j = 0; j < field->getHeight(); j++){
			for(size_t i = 0; i < field->getWidth(); i++){
			value = field->at(i, j)->getValue();
			std::string str = std::to_string(value);
			this->renderString(i * cell_size + 5, (j + 1) * cell_size - 5, str);
		}
	}
}

void Print::panel(const Field* field) const{
	const int_fast32_t half = cell_size / 2;
	glPointSize(agent_size);
	glBegin(GL_POINTS);
	for(size_t j = 0; j < field->getHeight(); j++){
		for(size_t i = 0; i < field->getWidth(); i++){
			if(field->at(i, j)->isPurePanel())
				continue;
			if(field->at(i, j)->isMyPanel())
				glColor3f(0.6f, 0.6f, 1.0f);
			if(field->at(i, j)->isEnemyPanel())
				glColor3f(1.0f, 0.6f, 0.6f);
			glVertex2i(half + cell_size * i, half + cell_size * j);
		}
	}
	glEnd();
}

void Print::agent(Field* field) const{
	const uint_fast32_t half = cell_size / 2;
	uint_fast32_t flag;
	glPointSize(agent_size);
	glBegin(GL_POINTS);
	std::for_each(field->agents.begin(), field->agents.end(), [&, this](auto& a){
			flag = a.getAttr();
			if(flag == MINE_ATTR)
				glColor3f(0.0f, 0.0f, 0.8f);
			if(flag == ENEMY_ATTR)
				glColor3f(0.8f, 0.0f, 0.0f);
			glVertex2i(half+cell_size * a.getX(), half + cell_size * a.getY());
		});
	glEnd();
}

void Print::agentNum(Field* field) const{
	const uint_fast32_t half = cell_size / 2;
	uint_fast32_t flag, mine = 0, enemy = 0;
	std::string value, str;
	glColor3f(0.0f, 0.0f, 0.0f);
	std::for_each(field->agents.begin(), field->agents.end(), [&, this](auto& a){
			flag = a.getAttr();
			if(flag == MINE_ATTR){
				mine++;
				value = std::to_string(mine);
				str = "M" + value;
				this->renderString(half + cell_size * a.getX() - 2, half + cell_size * a.getY(), str);
			}
			if(flag == ENEMY_ATTR){
				enemy++;
				value = std::to_string(enemy);
				str = "E" + value;
				this->renderString(half + cell_size * a.getX() - 2, half+cell_size * a.getY(), str);
			}
		});
}

void Print::point(const Field* field) const{
	;
}

void Print::candidate(Field* field, const std::vector<Direction>& next) const{
	const uint_fast32_t half = cell_size / 2;
	Direction direction;
	uint_fast32_t coordX, coordY;
	glPointSize(agent_size - 8);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	for(size_t i = 0; i < next.size(); i++){
		if(field->agents.at(i).getAttr() == MINE_ATTR){
			direction = next.at(i);
			coordX = field->agents.at(i).getX() + vec_x.at((int_fast32_t)direction);
			coordY = field->agents.at(i).getY() + vec_y.at((int_fast32_t)direction);
			glVertex2i(half + cell_size * coordX, half + cell_size * coordY);
		}
	}
	glEnd();

	
	/*
	const uint_fast32_t half = cell_size / 2;
	Direction direction;
	uint_fast32_t coordX, coordY;
	glPointSize(agent_size - 8);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	for(size_t i = 0; i < next.size(); i++){
		direction = next.at(i);
		coordX = field->agents.at(i).getX() + vec_x.at((int_fast32_t)direction);
		coordY = field->agents.at(i).getY() + vec_y.at((int_fast32_t)direction);
		glVertex2i(half + cell_size * coordX, half + cell_size * coordY);
	}
	glEnd();
	*/
}

PrintDisplay::PrintDisplay(){
}

PrintDisplay::~PrintDisplay(){	
}

void PrintDisplay::print(Field* field, const std::vector<Direction> next){
	this->line(field);
	this->panel(field);
	this->agent(field);
	this->candidate(field, next);
	this->agentNum(field);
	this->score(field);
	this->point(field);
}

void PrintDisplay::print(Field* field){
	;
}

PrintSelfDirectedGame::PrintSelfDirectedGame(){
}

PrintSelfDirectedGame::~PrintSelfDirectedGame(){
}

void PrintSelfDirectedGame::print(Field* field, const std::vector<Direction> next){
	;
}

void PrintSelfDirectedGame::print(Field* field){
	this->line(field);
	this->panel(field);
	this->agent(field);
	this->agentNum(field);
	this->score(field);
	this->point(field);
}

// ---------------------------------------- DisplayWrapper ----------------------------------------

DisplayWrapper::DisplayWrapper(){
	this->random = new Random();
	this->astar = new Astar();
	this->greedy = new Greedy();
}

DisplayWrapper::~DisplayWrapper(){
	delete this->instance;
	delete this->field;
	delete this->print;
	
	delete this->random;
	delete this->astar;
	delete this->greedy;
}

DisplayWrapper* DisplayWrapper::instance = 0;

void DisplayWrapper::init(){
	this->instance->initInstance();
	this->astar->init(this->field);
	gluOrtho2D(0, 100, 100, 0);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowPosition(window_width_position, window_height_position);
	glutInitWindowSize(window_width_size, window_height_size);
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

// ---------------------------------------- Display ----------------------------------------
Display::Display() : mine_flag(0) {
	this->print = new PrintDisplay();
}

Display::~Display(){
}


/*

Display::Display() : agent_flag(0), mine_flag(0), enemy_flag(0) {
	this->print = new PrintDisplay();
}

void Display::setPossible(){
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> coord, buf;
	uint_fast32_t x, y;
	this->possible.clear();
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++)
		coord.push_back(std::make_pair(vec_x.at(i), vec_y.at(i)));
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			std::for_each(coord.begin(), coord.end(), [&, this](auto& c){
					x = a.getX() + c.first;
					y = a.getY() + c.second;	
					if(this->isOutOfRange(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y))
						return;
					buf.push_back(std::make_pair(x, y));
				});
			this->possible.push_back(buf);
			buf.clear();
		});
	this->init();
}

void Display::moveNext(){
	uint_fast32_t count = 0;
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			if(this->next.at(count) == NONE)
				goto RE_CONSIDER;
			if(this->field->canMove(a, this->next.at(count)))
				a.move(this->next.at(count));
			else
				a.move(STOP);
		RE_CONSIDER:			
			count++;
		});
	this->field->applyNextAgents();
}

void Display::selectAgent(uint_fast32_t x, uint_fast32_t y){
	for(size_t i = 0; i < this->field->agents.size(); i++){
		if(this->field->agents.at(i).getX() == x && this->field->agents.at(i).getY() == y){
			if(this->field->agents.at(i).getAttr() == ENEMY_ATTR){
				this->agent_flag = 1;
				this->enemy_flag = i;
			}
			if(this->field->agents.at(i).getAttr() == MINE_ATTR){
				this->agent_flag = 0;
				this->mine_flag = i;
			}
			return;
		}
	}
}

void Display::selectDirection(uint_fast32_t x, uint_fast32_t y){
	std::pair<uint_fast32_t, uint_fast32_t> coord = std::make_pair(x, y);
	if(this->agent_flag){
		for(size_t i = 0; i < this->possible.at(this->enemy_flag).size(); i++){
			if(this->possible.at(this->enemy_flag).at(i) == coord){
				this->next.at(this->enemy_flag) = (Direction)i;
				glutPostRedisplay();
				return;
			}
		}
	}
	else{
		for(size_t i = 0; i < this->possible.at(this->mine_flag).size(); i++){
			if(this->possible.at(this->mine_flag).at(i) == coord){
				this->next.at(this->mine_flag) = (Direction)i;
				glutPostRedisplay();				
				return;
			}
		}
	}
}
*/


void Display::setPossible(){
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> direction, tmp;
	uint_fast32_t x, y;
	
	this->possible.clear();
	this->possible.resize(this->field->agents.size());
	
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++)
		direction.push_back(std::make_pair(vec_x.at(i), vec_y.at(i)));
	
	for(size_t i = 0; i < this->field->agents.size(); i++){
		if(this->field->agents.at(i).getAttr() == MINE_ATTR){
			std::for_each(direction.begin(), direction.end(), [&, this](auto& d){
					x = this->field->agents.at(i).getX() + d.first;
					y = this->field->agents.at(i).getY() + d.second;
					if(this->isOutOfRange(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y))
						return;
					tmp.push_back(std::make_pair(x, y));
				});
			this->possible.at(i) = tmp;
			tmp.clear();
		}
	}
	
	this->init();
}

void Display::moveNext(){
	for(size_t i = 0; i < this->field->agents.size(); i++){
		if(this->field->agents.at(i).getAttr() == MINE_ATTR){
			if(this->next.at(i) == NONE)
				this->field->agents.at(i).move(STOP);
			if(this->field->canMove(this->field->agents.at(i), this->next.at(i)))
				this->field->agents.at(i).move(this->next.at(i));
		}
	}
}

bool Display::selectAgent(uint_fast32_t x, uint_fast32_t y){
	for(size_t i = 0; i < this->field->agents.size(); i++){
		if(this->field->agents.at(i).getAttr() == MINE_ATTR){
			if(this->field->agents.at(i).getX() == x && this->field->agents.at(i).getY() == y){
				this->mine_flag = i;
				this->next.at(this->mine_flag) = STOP;
				glutPostRedisplay();
				return true;
			}
		}
	}
	return false;
}

void Display::selectDirection(uint_fast32_t x, uint_fast32_t y){
	Direction direction = STOP;
	uint_fast32_t agentX = this->field->agents.at(this->mine_flag).getX();
	uint_fast32_t agentY = this->field->agents.at(this->mine_flag).getY();
	std::pair<uint_fast32_t, uint_fast32_t> coord = std::make_pair(x, y);
	
	for(size_t i = 0; i < this->possible.at(this->mine_flag).size(); i++){
		if(this->possible.at(this->mine_flag).at(i) == coord){
			direction = this->changeDirection(std::make_pair(agentX, agentY), std::make_pair(x, y));
			this->next.at(this->mine_flag) = direction;
			glutPostRedisplay();
			return;
		}
	}
}

void Display::init(){
	std::fill(this->next.begin(), this->next.end(), STOP);
	this->mine_flag = 0;
	
	//this->agent_flag = 0;	
	//this->enemy_flag = 0;
}

const bool Display::isOutOfRange(int button, int state, int x, int y) const{
	const bool launch = (button == GLUT_LEFT_BUTTON && state==GLUT_DOWN);
	const bool range = (x < 0 || x > cell_size * this->field->width || y < 0 || y > cell_size * this->field->height);
	if(!launch || range)
		return true;
	return false;
}

void Display::initInstance(){
	this->next.clear();
	this->next.resize(this->field->agents.size());
	std::fill(this->next.begin(), this->next.end(), STOP);
	this->setPossible();
}

void Display::resize(int w, int h){
	glViewport(0, 0, w, h);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}
	
void Display::display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->print->print(this->field, next);
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

		this->greedy->move(this->field, MINE_ATTR);
		this->greedy->move(this->field, ENEMY_ATTR);
		this->setPossible();
		this->field->print();
		glutPostRedisplay();
		break;
		
	case 'm':
	case 'M':

		this->random->move(this->field, MINE_ATTR);
		this->random->move(this->field, ENEMY_ATTR);
		field->applyNextAgents();
		this->setPossible();
		this->field->print();
		glutPostRedisplay();
		break;
		
	case 'g':
	case 'G':
		
		this->moveNext();
		this->random->move(this->field, ENEMY_ATTR);
		field->applyNextAgents();
		this->setPossible();
		this->field->print();
		glutPostRedisplay();
		break;

	case 't':
	case 'T':

		this->astar->move(this->field, MINE_ATTR);
		//this->astar->move(this->field, ENEMY_ATTR);
		this->random->move(this->field, ENEMY_ATTR);
		
		this->field->applyNextAgents();

		this->setPossible();
		this->field->print();
		glutPostRedisplay();
		break;
		
	case 'd':
	case 'D':
		
		this->greedy->move(this->field, MINE_ATTR);
		//this->random->move(this->field, ENEMY_ATTR);
		//this->greedy->move(this->field, ENEMY_ATTR);
		
		this->field->applyNextAgents();
		this->setPossible();
		this->field->print();
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
	if(this->isOutOfRange(button, state, x, y))
		return;
	const uint_fast32_t coordX = x / cell_size;
	const uint_fast32_t coordY = y / cell_size;
	if(this->selectAgent(coordX, coordY))
		return;
	this->selectDirection(coordX, coordY);
}

void Display::motion(int x, int y){
	;
}

selfDirectedGame::selfDirectedGame(){
	this->print = new PrintSelfDirectedGame();
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

