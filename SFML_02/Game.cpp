#include "Game.h"

#include <iostream>

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& path)
{
	// TODO: read in config file here
	//	 use the premade PlayerConfig, EnemyConfig, BulletConfig
	// variables

	// setup default window parameters
	m_window.create(sf::VideoMode({1280, 720}), "SFML_02");
	m_window.setFramerateLimit(60);

	ImGui::SFML::Init(m_window);

	// scale the imgui ui and text size by 2
	ImGui::GetStyle().ScaleAllSizes(2.0f);
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
	// TODO: add pause fnuctionality in here
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
