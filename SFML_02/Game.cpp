#include "Game.h"

#include <iostream>

Game::Game(const std::string& config) : m_text(m_font) { init(config); }

void Game::init(const std::string& path)
{
	// TODO: read in config file here
	//	 use the premade PlayerConfig, EnemyConfig, BulletConfig
	//	 variables

	// setup default window parameters
	m_window.create(sf::VideoMode({1280, 720}), "SFML Polygon Asteroid");
	m_window.setFramerateLimit(60);

	if (!ImGui::SFML::Init(m_window))
	{
		std::cerr << "Failed to initialize SFML-Window\n";
	}

	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("ARIAL.TTF", 12.0f);

	ImGui::GetStyle().ScaleAllSizes(1.0f);
	ImGui::GetIO().FontGlobalScale = 2.0f;

	spawnPlayer();
	spawnEnemy();
}

std::shared_ptr<Entity> Game::player()
{
	auto& players = m_entities.getEntities("player");

	assert(players.size() == 1);

	return players.front();
}

void Game::run()
{
	// TODO: add pause functionality in here
	//	some systems should function while paused ( rendering )
	//	some systems shouldn't (movement / input)

	while (m_running)
	{
		// update the entity manager
		m_entities.update();

		// required update call to imgui
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		sGUI();
		sRender();

		// increment the current frame
		// may need to be moved when pause implemented
		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	// TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	// TODO: Finish adding all properties of the player with the correct
	// values from the config

	// We create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save
	// typing
	auto entity = m_entities.addEntity("player");

	// Give this entity a Transform so it spawns at (200, 200) with velocity
	// (1,1) and angles 0
	entity->add<CTransform>(Vec2f(200.0f, 200.0f), Vec2f(1.0f, 1.0f), 0.0f);

	// The entity's shape will have radius 32, 8 sides, dark grey fill, and
	// red outline
	entity->add<CShape>(32.0f, 8, sf::Color(10, 10, 10),
			    sf::Color(255, 0, 0), 4.0f);

	// Add an input component to the player so that we can use inputs
	entity->add<CInput>();
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	// TODO: make sure the enemy is spawned properly with the m_enemyConfig
	//	 variables
	//	the enemy must be spawned completely within the bounds of the
	//	window
	auto entity = m_entities.addEntity("enemy");

	entity->add<CTransform>(Vec2f(100.0f, 200.0f), Vec2f(1.0f, 1.0f), 0.0f);

	entity->add<CShape>(32.0f, 4, sf::Color(100, 200, 50),
			    sf::Color(255, 123, 0), 4.0f);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// TODO: spawn small enemies at the location of the input enemy e

	// when we create the smaller enemy, we have to read the values of the
	// original enemy
	// - swn a number of small enemies equal to the vertices of the origial
	// enemy
	// - set each small enemy to the same color as the original, half the
	// size
	// - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target)
{
	// TODO: implement the spawning of a bullet which travels toward target
	//	- bullet speed is given asa scalar speed
	//	- you must set the velocity by using formula in notes
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO: implement your own special weapon
}

void Game::sMovement()
{
	// TODO: implement all entity movement in this function
	//	 you should read the m_player->CInput component to detrmine if
	// the player is moving

	// Sample movement speed update
	auto& transform = player()->get<CTransform>();
	transform.pos.x += transform.velocity.x;
	transform.pos.y += transform.velocity.y;
}

void Game::sLifeSpan()
{
	// TODO: implement all lifespan functionality

	// for all entities
	//	if entity has no lifespan component, skip it
	//	if entity has > 0 remaining lifespan, substract 1
	//	if it has lifespan and is alive
	//		scale its alpha channel properly
	//	if it has lifespan and its time is up
	//		destroy the entity
}

void Game::sCollision()
{
	// TODO: implement all proper collisions between entities
	//	 be sure to use the collision radius, NOT the shape radius
}

void Game::sEnemySpawner()
{
	// TODO: code which implements enemy spawning should go here
}

void Game::sGUI()
{
	ImGui::Begin("Geometry Wars");

	ImGui::Text("Stuff Goes Here");

	ImGui::End();
}

void Game::sRender()

{	m_window.clear();

	// Iterate through all entities ( neglecting the entities that don't have transform or shape components.
	for (auto& e : m_entities.getEntities())
	{
		if (!e->has<CTransform>() || !e->has<CShape>())
		{
			continue;	
		}

		auto& transform = e->get<CTransform>();
		auto& shape	= e->get<CShape>().circle;

		transform.angle += 1.0f;

		shape.setRotation(sf::degrees(transform.angle));
		shape.setPosition(transform.pos);

		m_window.draw(shape);
	}

	// draw the ui last
	ImGui::SFML::Render(m_window);
	m_window.display();
}

void Game::sUserInput()
{
	// TODO: handle user input here
	//	 note that you should only be setting the player's input
	// componenet variables here 	 you should not implementhe player's
	// moement logic here 	the movement system will read the variabls you
	// set in this function

	while (const std::optional event = m_window.pollEvent())
	{
		// pass the event to imgui to b parsed
		ImGui::SFML::ProcessEvent(m_window, *event);

		// this event triggers when the window is closed
		if (event->is<sf::Event::Closed>())
		{
			m_running = false;
		}

		// this event is triggered when a key is pressed
		if (const auto* keyPressed =
			event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->code)
			{
				case sf::Keyboard::Key::W:
					std::cout << "W pressed\n";
					break;
				default:
					break;
			}
		}

		// this event is triggered when a key is released
		if (const auto* keyReleased =
			event->getIf<sf::Event::KeyReleased>())
		{
			switch (keyReleased->code)
			{
				case sf::Keyboard::Key::W:
					std::cout << "W released\n";
					break;
				default:
					break;
			}
		}

		// mouse keys
		if (const auto* mouseClick =
			event->getIf<sf::Event::MouseButtonPressed>())
		{
			// this line ignores mouse events if ImGui is the thing
			// being clicked
			if (ImGui::GetIO().WantCaptureMouse)
			{
				continue;
			}

			if (mouseClick->button == sf::Mouse::Button::Left)
			{
				std::cout << "Left Mouse Button Clicked at ( "
					  << mouseClick->position.x << ","
					  << mouseClick->position.y << " )\n";
				// call spawnBullet here
			}

			if (mouseClick->button == sf::Mouse::Button::Right)
			{
				std::cout << "Right Mouse Button Clicked at ( "
					  << mouseClick->position.x << ","
					  << mouseClick->position.y << " )\n";
				// call spawnSpecialWeapon here
			}
		}
	}
}
