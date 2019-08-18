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
	glLineWidth(this->line_size);
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
	for(size_t i = 0; i < field->getWidth(); i++){
		for(size_t j = 0; j < field->getHeight(); j++){
			value = field->at(i, j)->getValue();
			std::string str = std::to_string(value);
			this->renderString(i * cell_size + 5, (j + 1) * cell_size - 5, str);
		}
	}
}

void Print::panel(const Field* field) const{
	glPointSize(this->agent_size);
	const int_fast32_t half = cell_size / 2;
	for(size_t i = 0; i < field->getWidth(); i++){
		for(size_t j = 0; j < field->getHeight(); j++){
			if(field->at(i, j)->isPurePanel())
				continue;
			if(field->at(i, j)->isMyPanel())
				glColor3f(0.6f, 0.6f, 1.0f);
			if(field->at(i, j)->isEnemyPanel())
				glColor3f(1.0f, 0.6f, 0.6f);
			glBegin(GL_POINTS);
			glVertex2i(half + cell_size * i, half + cell_size * j);
			glEnd();
		}
	}
}

void Print::agent(Field* field) const{
	const int_fast32_t half = cell_size / 2;
	uint_fast32_t flag;
	glPointSize(this->agent_size);
	std::for_each(field->agents.begin(), field->agents.end(), [&, this](auto& a){
			flag = a.getAttr();
			if(flag == MINE_ATTR)
				glColor3f(0.0f, 0.0f, 0.8f);
			if(flag == ENEMY_ATTR)
				glColor3f(0.8f, 0.0f, 0.0f);
			glBegin(GL_POINTS);
			glVertex2i(half+cell_size * a.getX(), half + cell_size * a.getY());
			glEnd();
		});
}

void Print::agentNum(Field* field) const{
	const int_fast32_t half = cell_size / 2;
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

void Print::candidate(Field* field, const std::vector<Direction>& next_list) const{
	const uint_fast32_t half = cell_size / 2;
	Direction direction;
	uint_fast32_t coordX, coordY;
	glPointSize(this->agent_size - 8);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_POINTS);
	for(size_t i = 0; i < field->agents.size(); i++){
		direction = next_list.at(i);
		coordX = field->agents.at(i).getX() + vec_x.at((int_fast32_t)direction);
		coordY = field->agents.at(i).getY() + vec_y.at((int_fast32_t)direction);
		glVertex2i(half + cell_size * coordX, half + cell_size * coordY);
	}
	glEnd();
}

PrintDisplay::PrintDisplay(){
}

PrintDisplay::~PrintDisplay(){	
}

void PrintDisplay::print(Field* field, const std::vector<Direction> next_list){
	this->line(field);
	this->panel(field);
	this->agent(field);
	this->candidate(field, next_list);
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

void PrintSelfDirectedGame::print(Field* field, const std::vector<Direction> next_list){
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

// ---------------------------------------- Display ----------------------------------------

Display::Display() : flag(0) ,mine_flag(0), enemy_flag(0) {
	this->print = new PrintDisplay();
}

Display::~Display(){
}

void Display::makePossibleList(){
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> coord, possible;
	uint_fast32_t x, y;
	this->possible_list.clear();
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++)
		coord.push_back(std::make_pair(vec_x.at(i), vec_y.at(i)));
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			std::for_each(coord.begin(), coord.end(), [&, this](auto& c){
					x = a.getX() + c.first;
					y = a.getY() + c.second;
					possible.push_back(std::make_pair(x, y));
				});
			this->possible_list.push_back(possible);
			possible.clear();
		});
	this->init();
}

void Display::moveNextList(){
	uint_fast32_t count = 0;
	std::for_each(this->field->agents.begin(), this->field->agents.end(), [&, this](auto& a){
			if(this->next_list.at(count) == NONE)
				goto RE_CONSIDER;
			if(this->field->canMove(a, this->next_list.at(count)))
				a.move(this->next_list.at(count));
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
				this->flag = true;
				this->enemy_flag = i;
			}
			if(this->field->agents.at(i).getAttr() == MINE_ATTR){
				this->flag = false;
				this->mine_flag = i;
			}
		}
	}
}

void Display::selectDirection(uint_fast32_t x, uint_fast32_t y){
	std::pair<uint_fast32_t, uint_fast32_t> coord = std::make_pair(x, y);
	if(this->flag){
		for(size_t i = 0; i < this->possible_list.at(this->enemy_flag).size(); i++){
			if(this->possible_list.at(this->enemy_flag).at(i) == coord){
				this->next_list.at(this->enemy_flag) = (Direction)i;
				glutPostRedisplay();
				return;
			}
		}
	}
	else{
		for(size_t i = 0; i < this->possible_list.at(this->mine_flag).size(); i++){
			if(this->possible_list.at(this->mine_flag).at(i) == coord){
				this->next_list.at(this->mine_flag) = (Direction)i;
				glutPostRedisplay();				
				return;
			}
		}
	}
}

void Display::init(){
	std::fill(this->next_list.begin(), this->next_list.end(), STOP);
	this->mine_flag = 0;
	this->enemy_flag = 0;
	this->flag = false;
}

const bool Display::isOutOfRange(int button, int state, int x, int y) const{
	const bool launch = (button == GLUT_LEFT_BUTTON && state==GLUT_DOWN);
	const bool range = (x < 0 || x > cell_size * this->field->width || y < 0 || y > cell_size * this->field->height);
	if(!launch || range)
		return true;
	return false;
}

void Display::initInstance(){
	this->next_list.clear();
	this->next_list.resize(this->field->agents.size());
	std::fill(this->next_list.begin(), this->next_list.end(), NONE);
	this->makePossibleList();
}

void Display::resize(int w, int h){
	glViewport(0, 0, w, h);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}
	
void Display::display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->print->print(this->field, next_list);
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
		
		this->field->testMoveAgent();
		this->makePossibleList();
		this->field->print();
		glutPostRedisplay();
		break;
		
	case 'm':
	case 'M':

		this->random->move(this->field, MINE_ATTR);
		this->random->move(this->field, ENEMY_ATTR);
		field->applyNextAgents();
		this->makePossibleList();
		this->field->print();
		glutPostRedisplay();
		break;
		
	case 'g':
	case 'G':
		
		this->moveNextList();
		this->makePossibleList();
		this->field->print();
		glutPostRedisplay();
		break;

	case 't':
	case 'T':

		//this->greedy->move(this->field, MINE_ATTR);
		//this->random->move(this->field, ENEMY_ATTR);
		//this->greedy->move(this->field, ENEMY_ATTR);
		//this->field->genRandMap();
		this->astar->move(this->field, MINE_ATTR);
		//this->astar->move(this->field, MINE_ATTR);
		
		this->field->applyNextAgents();
		this->makePossibleList();
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
	this->selectAgent(coordX, coordY);
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

