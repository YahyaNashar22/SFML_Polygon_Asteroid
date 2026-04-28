#pragma once

#include <SFML/Graphics.hpp>

#include "EntityManager.hpp"

class Game
{
	sf::RenderWindow m_window;    // the window we draw to
	EntityManager	 m_entities;  // vector of entities
	sf::Font	 m_font;      // font to draw
	sf::Text	 m_text;      // score text
	PlayerConfig	 m_playerConfig;
	EnemyConfig	 m_enemyConfig;
	BulletConfig	 m_bulletConfig;
	sf::Clock	 m_deltaClock;
	int		 m_score	      = 0;
	int		 m_currentFrame	      = 0;
	int		 m_lastEnemySpawnTime = 0;
	bool		 m_paused  = false;  // wether we update game logic
	bool		 m_running = true;

	void init(const std::string&
		      config);	// initialize the GameState with a config file
	void setPaused(bool paused);  // pause the game

	void sMovement();   // System: Entity position / movement update
	void sUserInput();  // System: User Input
	void sLifeSpan();   // System: LifeSpan
	void sRender();	    // System: Render / Drawing
	void sGUI();
	void sEnemySpawner();  // System: Spawns Enemies
	void sCollision();     // System: Collisions

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

	std::shared_ptr<Entity> player();

       public:
	Game(const std::string& config);  // constructor, takes in game config

	void run();
};
