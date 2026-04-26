# Architecture:
## Game Engine:
- ### Systems
- ### Entity Manager
  - Entity
	- Component


&nbsp;
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
  - +circle:    float

- ### CLifeSpan:
  - +lifeSpan:  int
  - +remaining: int

- ### CInput:
  - +up:        int
  - +down:      int
  - +left:      int
  - +right:     int
  - +shoot:     int

