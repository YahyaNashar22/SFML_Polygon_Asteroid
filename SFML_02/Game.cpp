#include "Game.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

Game::Game(const std::string& config) : m_text(m_font) { init(config); }

void Game::init(const std::string& path)
{
	// seed random
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// TODO: read in config file here
	//	 use the premade PlayerConfig, EnemyConfig, BulletConfig
	//	 variables
	m_enemyConfig.SR   = 32;    // shape radius
	m_enemyConfig.CR   = 32;    // collision radius
	m_enemyConfig.OR   = 255;   // outline red
	m_enemyConfig.OG   = 255;   // outline green
	m_enemyConfig.OB   = 255;   // outline blue
	m_enemyConfig.OT   = 4;	    // outline thickness
	m_enemyConfig.VMIN = 3;	    // min vertices
	m_enemyConfig.VMAX = 8;	    // max vertices
	m_enemyConfig.SMIN = 1.0f;  // min speed
	m_enemyConfig.SMAX = 3.0f;  // max speed
	m_enemyConfig.L	   = 60;    // lifespan later
	m_enemyConfig.SI   = 120;   // spawn interval frames

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

	entity->add<CCollision>(32.0f);

	// Add an input component to the player so that we can use inputs
	entity->add<CInput>();
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	float radius = static_cast<float>(m_enemyConfig.SR);
	float x =
	    radius + static_cast<float>(
			 std::rand() %
			 static_cast<int>(m_window.getSize().x - radius * 2));
	float y =
	    radius + static_cast<float>(
			 std::rand() %
			 static_cast<int>(m_window.getSize().y - radius * 2));

	int vertices =
	    m_enemyConfig.VMIN +
	    (std::rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1));

	float speed =
	    m_enemyConfig.SMIN + (static_cast<float>(std::rand()) / RAND_MAX *
				  (m_enemyConfig.SMAX - m_enemyConfig.SMIN));

	float vx = (std::rand() % 2 == 0 ? -speed : speed);
	float vy = (std::rand() % 2 == 0 ? -speed : speed);

	int r = std::rand() % 256;
	int g = std::rand() % 256;
	int b = std::rand() % 256;

	auto entity = m_entities.addEntity("enemey");

	entity->add<CTransform>(Vec2f(x, y), Vec2f(vx, vy), 0.0f);

	entity->add<CShape>(
	    radius, vertices, sf::Color(r, g, b),
	    sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
	    m_enemyConfig.OT);

	entity->add<CCollision>(static_cast<float>(m_enemyConfig.CR));

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
	auto p = player();

	auto& transform = p->get<CTransform>();
	auto& input	= p->get<CInput>();

	Vec2f direction(0.0f, 0.0f);

	if (input.up)
	{
		direction.y -= 1.0f;
	}
	if (input.down)
	{
		direction.y = 1.0f;
	}
	if (input.left)
	{
		direction.x -= 1.0f;
	}
	if (input.right)
	{
		direction.x += 1.0f;
	}

	float speed = 5.0f;

	// If moving, normalize direction so diagonla movements are not faster
	if (direction.x != 0.0f || direction.y != 0.0f)
	{
		float length = std::sqrt(direction.x * direction.x +
					 direction.y * direction.y);

		direction /= length;
	}

	transform.velocity = direction * speed;

	for (auto& e : m_entities.getEntities())
	{
		if (!e->has<CTransform>())
		{
			continue;
		}

	e->get<CTransform>().pos += e->get<CTransform>().velocity;
	}

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
	if (m_currentFrame - m_lastEnemySpawnTime >= m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::sGUI()
{
	ImGui::Begin("Geometry Wars");

	ImGui::Text("Stuff Goes Here");

	ImGui::End();
}

void Game::sRender()

{
	m_window.clear();

	// Iterate through all entities ( neglecting the entities that don't
	// have transform or shape components.
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
	while (const std::optional event = m_window.pollEvent())
	{
		// pass the event to imgui to b parsed
		ImGui::SFML::ProcessEvent(m_window, *event);

		// this event triggers when the window is closed
		if (event->is<sf::Event::Closed>())
		{
			m_running = false;
		}

		auto& inputs = player()->get<CInput>();

		if (const auto* keyPressed =
			event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->code)
			{
				case sf::Keyboard::Key::W:
					inputs.up = true;
					break;
				case sf::Keyboard::Key::S:
					inputs.down = true;
					break;
				case sf::Keyboard::Key::A:
					inputs.left = true;
					break;
				case sf::Keyboard::Key::D:
					inputs.right = true;
					break;
				default:
					break;
			}
		}

		if (const auto* keyReleased =
			event->getIf<sf::Event::KeyReleased>())
		{
			switch (keyReleased->code)
			{
				case sf::Keyboard::Key::W:
					inputs.up = false;
					break;
				case sf::Keyboard::Key::S:
					inputs.down = false;
					break;
				case sf::Keyboard::Key::A:
					inputs.left = false;
					break;
				case sf::Keyboard::Key::D:
					inputs.right = false;
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
