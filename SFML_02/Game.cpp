#include "Game.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

Game::Game(const std::string& config) : m_text(m_font) { init(config); }

void Game::init(const std::string& path)
{
	// seed random
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// read from config.txt
	std::fstream fin("config.txt");
	std::string  key;

	// Window configs
	unsigned int w		= 1280;
	unsigned int h		= 720;
	unsigned int fps	= 60;
	bool	     fullscreen = 0;

	// Font configs
	std::string  font      = "ARIAL.TTF";
	unsigned int font_size = 24;
	int	     font_r    = 255;
	int	     font_g    = 255;
	int	     font_b    = 255;

	while (fin >> key)
	{
		if (key == "Window")
		{
			fin >> w >> h >> fps >> fullscreen;
		}
		else if (key == "Font")
		{
			fin >> font >> font_size >> font_r >> font_g >> font_b;
		}
		else if (key == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >>
			    m_playerConfig.FR >> m_playerConfig.FG >>
			    m_playerConfig.FB >> m_playerConfig.OR >>
			    m_playerConfig.OG >> m_playerConfig.OB >>
			    m_playerConfig.OT >> m_playerConfig.V >>
			    m_playerConfig.S;
		}
		else if (key == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >>
			    m_enemyConfig.OR >> m_enemyConfig.OG >>
			    m_enemyConfig.OB >> m_enemyConfig.OT >>
			    m_enemyConfig.VMIN >> m_enemyConfig.VMAX >>
			    m_enemyConfig.L >> m_enemyConfig.SI >>
			    m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
		}
		else if (key == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >>
			    m_bulletConfig.FR >> m_bulletConfig.FG >>
			    m_bulletConfig.FB >> m_bulletConfig.OR >>
			    m_bulletConfig.OG >> m_bulletConfig.OB >>
			    m_bulletConfig.OT >> m_bulletConfig.V >>
			    m_bulletConfig.L >> m_bulletConfig.S;
		}
	}

	m_window.create(
	    sf::VideoMode({w, h}), "SFML Polygon Asteroid",
	    fullscreen ? sf::State::Fullscreen : sf::State::Windowed);
	m_window.setFramerateLimit(fps);

	if (!m_font.openFromFile(font))
	{
		std::cerr << "Failed to load font\n";
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(font_size);
	m_text.setFillColor(sf::Color(static_cast<std::uint8_t>(font_r),
				      static_cast<std::uint8_t>(font_g),
				      static_cast<std::uint8_t>(font_b)));
	m_text.setPosition({20.0f, 20.0f});

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
		sLifeSpan();
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

	auto entity = m_entities.addEntity("enemy");

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
	if (!e->has<CTransform>() || !e->has<CShape>())
	{
		return;
	}

	auto& enemyTransform = e->get<CTransform>();
	auto& enemyShape     = e->get<CShape>().circle;

	Vec2f center = enemyTransform.pos;

	size_t points = enemyShape.getPointCount();

	float smallRadius = enemyShape.getRadius() / 2.0f;
	float speed	  = 4.0f;

	const float PI = 3.14159265f;

	for (size_t i = 0; i < points; i++)
	{
		float angle = (2.0f * PI * i) / static_cast<float>(points);

		Vec2f velocity(std::cos(angle) * speed,
			       std::sin(angle) * speed);

		auto small = m_entities.addEntity("smallEnemy");

		small->add<CTransform>(center, velocity, 0.0f);
		small->add<CShape>(smallRadius, points,
				   enemyShape.getFillColor(),
				   enemyShape.getOutlineColor(), 2);
		small->add<CCollision>(smallRadius);
		small->add<CLifeSpan>(60);
	}
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target)
{
	auto& transform = entity->get<CTransform>();

	Vec2f playerPos = transform.pos;
	Vec2f direction = target - playerPos;

	float length =
	    std::sqrt(direction.x * direction.x + direction.y * direction.y);

	if (length == 0.0f)
	{
		return;
	}

	direction /= length;

	Vec2f bulletVelocity = direction * m_bulletConfig.S;

	auto bullet = m_entities.addEntity("bullet");

	bullet->add<CTransform>(playerPos, bulletVelocity, 0.0f);

	bullet->add<CShape>(
	    static_cast<float>(m_bulletConfig.SR), m_bulletConfig.V,
	    sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
	    sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
	    m_bulletConfig.OT);

	bullet->add<CCollision>(static_cast<float>(m_bulletConfig.CR));

	bullet->add<CLifeSpan>(m_bulletConfig.L);
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
	for (auto& e : m_entities.getEntities())
	{
		if (!e->has<CLifeSpan>())
		{
			continue;
		}
		auto& lifespan = e->get<CLifeSpan>();

		lifespan.remaining--;

		if (e->has<CShape>())
		{
			auto& shape   = e->get<CShape>().circle;
			auto  fill    = shape.getFillColor();
			auto  outline = shape.getOutlineColor();

			float ratio = static_cast<float>(lifespan.remaining) /
				      static_cast<float>(lifespan.lifespan);

			if (ratio < 0.0f)
			{
				ratio = 0.0f;
			}

			auto alpha = static_cast<std::uint8_t>(255 * ratio);

			fill.a	  = alpha;
			outline.a = alpha;

			shape.setFillColor(fill);
			shape.setOutlineColor(outline);
		}

		if (lifespan.remaining <= 0)
		{
			e->destroy();
		}
	}
}

void Game::sCollision()
{
	const float windowWidth	 = static_cast<float>(m_window.getSize().x);
	const float windowHeight = static_cast<float>(m_window.getSize().y);

	// screen border collision
	for (auto& e : m_entities.getEntities())
	{
		if (!e->has<CTransform>() || !e->has<CCollision>())
		{
			continue;
		}

		auto& transform = e->get<CTransform>();
		auto& collision = e->get<CCollision>();

		float r = collision.radius;

		// Bullets get destroyed when leaving screen
		if (e->tag() == "bullet")
		{
			if (transform.pos.x < 0 ||
			    transform.pos.x > windowWidth ||
			    transform.pos.y < 0 ||
			    transform.pos.y > windowHeight)
			{
				e->destroy();
			}

			continue;
		}

		// player/enemies clamp/bounce
		if (transform.pos.x - r < 0)
		{
			transform.pos.x = r;
			transform.velocity.x *= -1;
		}
		if (transform.pos.x + r > windowWidth)
		{
			transform.pos.x = windowWidth - r;
			transform.velocity.x *= -1;
		}
		if (transform.pos.y - r < 0)
		{
			transform.pos.y = r;
			transform.velocity.y *= -1;
		}
		if (transform.pos.y + r > windowHeight)
		{
			transform.pos.y = windowHeight - r;
			transform.velocity.y *= -1;
		}
	}

	// Bullets Enemies Collision
	for (auto& bullet : m_entities.getEntities("bullet"))
	{
		if (!bullet->isActive() || !bullet->has<CTransform>() ||
		    !bullet->has<CCollision>())
		{
			continue;
		}

		// enemies
		for (auto& enemy : m_entities.getEntities("enemy"))
		{
			if (!enemy->isActive() || !enemy->has<CTransform>() ||
			    !enemy->has<CCollision>())
			{
				continue;
			}

			float distance = bullet->get<CTransform>().pos.dist(
			    enemy->get<CTransform>().pos);
			float combineRadius = bullet->get<CCollision>().radius +
					      enemy->get<CCollision>().radius;

			if (distance < combineRadius)
			{
				bullet->destroy();
				enemy->destroy();

				m_score += 1;
				spawnSmallEnemies(enemy);

				break;
			}
		}

		// small enemies
		for (auto& enemy : m_entities.getEntities("smallEnemy"))
		{
			if (!enemy->isActive() || !enemy->has<CTransform>() ||
			    !enemy->has<CCollision>())
			{
				continue;
			}

			float distance = bullet->get<CTransform>().pos.dist(
			    enemy->get<CTransform>().pos);
			float combineRadius = bullet->get<CCollision>().radius +
					      enemy->get<CCollision>().radius;

			if (distance < combineRadius)
			{
				bullet->destroy();
				enemy->destroy();

				m_score += 2;

				break;
			}
		}
	}

	// Player / Enemy Collision
	auto p = player();

	for (auto& enemy : m_entities.getEntities("enemy"))
	{
		if (!enemy->isActive() || !enemy->has<CTransform>() ||
		    !enemy->has<CCollision>())
		{
			continue;
		}

		float distance =
		    p->get<CTransform>().pos.dist(enemy->get<CTransform>().pos);
		float combineRadius = p->get<CCollision>().radius +
				      enemy->get<CCollision>().radius;

		if (distance < combineRadius)
		{
			auto& transform = p->get<CTransform>();
			transform.pos =
			    Vec2f(windowWidth / 2, windowHeight / 2);
			transform.velocity = Vec2f(0.0f, 0.0f);
			m_score		   = 0;

			enemy->destroy();
		}
	}

	// Player / SmallEnemy Collision
	for (auto& enemy : m_entities.getEntities("smallEnemy"))
	{
		if (!enemy->isActive() || !enemy->has<CTransform>() ||
		    !enemy->has<CCollision>())
		{
			continue;
		}

		float distance =
		    p->get<CTransform>().pos.dist(enemy->get<CTransform>().pos);
		float combineRadius = p->get<CCollision>().radius +
				      enemy->get<CCollision>().radius;

		if (distance < combineRadius)
		{
			auto& transform = p->get<CTransform>();
			transform.pos =
			    Vec2f(windowWidth / 2, windowHeight / 2);
			transform.velocity = Vec2f(0.0f, 0.0f);
			m_score		   = 0;

			enemy->destroy();
		}
	}
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

	m_text.setString("Score: " + std::to_string(m_score));
	m_window.draw(m_text);
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
				Vec2f mousePos(
				    static_cast<float>(mouseClick->position.x),
				    static_cast<float>(mouseClick->position.y));

				spawnBullet(player(), mousePos);
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
