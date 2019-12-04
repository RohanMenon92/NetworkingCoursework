#include<Game.hpp>
//static functions

//initialize functions
void Game::initWindow()
{
	std::cout << "INIT WINDOW HAS BEEN CALLED" << std::endl;
	//creates an SFML window using options from window.ini file.
	std::ifstream ifs("Config/window.ini");
	sf::VideoMode window_bounds(800, 600);
	std::string title = "ClientGame";
	unsigned framerate_limit = 120;
	bool vertical_sync_enabled = false;

	if (ifs.is_open()) {
		std::getline(ifs, title);
		ifs >> window_bounds.width >> window_bounds.height;
		ifs >> framerate_limit;
		ifs >> vertical_sync_enabled;
	}
	ifs.close();

	window = new sf::RenderWindow(window_bounds, title);
	window->setFramerateLimit(framerate_limit);
	window->setVerticalSyncEnabled(vertical_sync_enabled);
}

//constructors/destructors
Game::Game() :
	playerBoxes()
	,bullets()
{
	initWindow();
}

Game::~Game()
{
	delete window;

	//while (!this->states.empty()) {
	//	delete this->states.top();
	//	this->states.pop();
	//}
}

void Game::ProcessInput()
{
	std::cout << "[GAME] PROCESS INPUT" << std::endl;
}

void Game::UpdateSFMLEvents()
{
	while (window->pollEvent(sfEvent))
	{
		if (sfEvent.type == sf::Event::Closed)
			window->close();
	}
}

void Game::Update(sf::Time dt)
{
	std::cout << "[GAME] UPDATE CALL DT:" << dt.asSeconds() << std::endl;
}

void Game::Render()
{
	std::cout << "[GAME] RENDER CALL" << std::endl;
}

////Functions
//void Game::updateDT()
//{
//	//updates the dt variable with the time it takes to update and render one frame
//	this->dt = this->dtClock.restart().asSeconds();
//	//system("cls");
//	//std::cout << this->dt << std::endl;
//}
//
////Captures event for closing window
//void Game::updateSFMLEvents()
//{
//	while (this->window->pollEvent(sfEvent))
//	{
//		if (this->sfEvent.type == sf::Event::Closed)
//			this->window->close();
//	}
//}
//
//void Game::update()
//{
//	this->updateSFMLEvents();
//	mousePos = (sf::Vector2f)sf::Mouse::getPosition(*(this->window));
//	//listOfCircles[0].setPosition(mousePos.x-listOfCircles[0].getRadius(), mousePos.y - listOfCircles[0].getRadius());
//	myPaddle.setPosition(mousePos.x, mousePos.y);
//	//std::cout << "position set to: x" << mousePos.x << " y" << mousePos.y << std::endl;
//
//	//if (!this->states.empty()) {
//	//	this->states.top()->update(this->dt);
//
//	//	/*if (this->states.top()->getQuit()) {
//	//		delete this->states.top();
//	//		this->states.pop();
//	//	}*/
//	//}
//}

//void Game::render()
//{
//	this->window->clear();
//
//	//render items
//	//if (!this->states.empty()) {
//	//	this->states.top()->render();
//	//}
//
//	this->window->draw(enemyPaddle);
//	this->window->draw(listOfCircles[0]);
//	this->window->draw(myPaddle);
//
//	this->window->display();
//}

void Game::RunGame(sf::Vector2f spawnPlayerPosition)
{
	std::cout << "GAME CLIENT IS RUNNING GAME: window" << window->isOpen() << " POSITION "
		<< spawnPlayerPosition.x << " "  << spawnPlayerPosition.y << std::endl;

	while (window->isOpen())
	{
		ProcessInput();
		UpdateSFMLEvents();
		Update(dtClock.restart());
		Render();
	}
}

//void Game::run()
//{
//	//setup ball
//	sf::CircleShape ball(100.f);
//	ball.setFillColor(sf::Color::Green);
//	listOfCircles.push_back(ball);
//	//defined by the 32x32 sprite which is scaled up 10x
//	sf::IntRect cropShape(30, 90, 260, 140);
//	//			  x start, y start, x length, y length
//	sf::Color translucent = sf::Color(255, 255, 255, 180);
//	//sf::Color translucent = sf::Color(255, 0, 0, 180);
//	//setup player paddle
//	if (!bluPaddle.loadFromFile("Resources/bluPaddle.png")) {
//		std::cout << "failed to load sprite" << std::endl;
//	}
//	myPaddle.setTexture(bluPaddle);
//	myPaddle.setTextureRect(cropShape);
//	//change the origin of the paddle to be (0,0)
//	sf::Vector2i bluSpriteSize(cropShape.width, cropShape.height);
//	myPaddle.setOrigin((bluSpriteSize.x / 2), (bluSpriteSize.y / 2));
//	myPaddle.setColor(translucent);
//
//	//setup enemy paddle
//	if (!redPaddle.loadFromFile("Resources/redPaddle.png")) {
//		std::cout << "failed to load sprite" << std::endl;
//	}
//	enemyPaddle.setTexture(redPaddle);
//	enemyPaddle.setTextureRect(cropShape);
//	//change the origin of the paddle to be (0,0)
//	sf::Vector2i redSpriteSize(cropShape.width, cropShape.height);
//	enemyPaddle.setOrigin(redSpriteSize.x / 2, redSpriteSize.y / 2);
//	//set paddle position to the centre of the window
//	sf::Vector2f windowSize = (sf::Vector2f)this->window->getSize();
//	enemyPaddle.setPosition(windowSize.x / 2, windowSize.y / 2);
//	enemyPaddle.setColor(translucent);
//
//	//main loop
//	while (this->window->isOpen())
//	{
//		this->updateDT();
//		this->update();
//		this->render();
//	}
//}