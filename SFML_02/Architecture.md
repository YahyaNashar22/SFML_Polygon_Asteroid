# Architecture:
## Game Engine:
- ### Systems
- ### Entity Manager
  - Entity
	- Component


	---
## Components:
- ### CTransform:
  - +pos:       Vec2f
  - +velocity:  Vec2f
  - +scale:     Vec2f
  - +angle:     float

- ### CCollision:
  - +radius:    float

- ### CScore:
  - +score:     int

 - ### CShape:
   - +circle:    sf::CircleShape

- ### CLifeSpan:
  - +lifeSpan:  int
  - +remaining: int

- ### CInput:
  - +up:        int
  - +down:      int
  - +left:      int
  - +right:     int
  - +shoot:     int

  ---
  ## Entity:
  - -m_components:  CTuple
  - -m_alive:       bool
  - -m_id:          int
  - -m_tag:         string

  - +get< T >:      T&
  - +add< T >:      void
  - +has< T >:      bool

  - +id:            int
  - +isAlive:       bool
  - +tag:           string&
  - +destrpy:       void

  ---
  ### EntityManager:
  - -m_entities:        EntityVec
  - -m_entityMap:       EntitMap
  - -m_toAdd:           EntityVec
  - -m_totalEntities:   int

  - -init():            void
  - +update():          void
  - +addEntity(tag):    sp< Entity >
  - +getEntities():     EntityVec&
  - +getEntities(tag):  EntityVec&
