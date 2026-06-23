#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
using namespace sf;
using namespace std;

class Player{
	private:
		const float START_SPEED = 200;
		Vector2f m_Position;
		Texture m_Texture;
		Sprite m_Sprite;

		Vector2f m_Resolution;
		IntRect m_Arena; // Size of Current Arena
		int m_TileSize;

		bool m_LeftPressed;
		bool m_RightPressed;
		bool m_UpPressed;
		bool m_DownPressed;

		float m_Speed;

	public:
		Player();
		Sprite getSprite();
		FloatRect getPosition();
		Vector2f getCenter();

		void spawn(IntRect arena, Vector2f resolution, int tileSize);
		void update(float elapsedTime, Vector2i mouseScreenPosition);

		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();

		void stopLeft();
		void stopRight();
		void stopUp();
		void stopDown();

};

Player::Player(){
	m_Speed = START_SPEED;
	m_Texture.loadFromFile("player.png");
	m_Sprite.setTexture(m_Texture);

	m_Sprite.setOrigin(25, 25);
	//m_Sprite.setPosition(1920/2.0f, 1080/2.0f);
	m_Position = m_Sprite.getPosition(); // keep updated
}

Sprite Player::getSprite(){
	return m_Sprite;
}

FloatRect Player::getPosition(){
	return m_Sprite.getGlobalBounds();
}

Vector2f Player::getCenter(){
	return m_Position;
}

void Player::moveLeft(){
	m_LeftPressed = true;
}

void Player::moveRight(){
	m_RightPressed = true;
}

void Player::moveUp(){
	m_UpPressed = true;
}

void Player::moveDown(){
	m_DownPressed = true;
}

void Player::stopLeft(){
	m_LeftPressed = false;
}

void Player::stopRight(){
	m_RightPressed = false;
}

void Player::stopUp(){
	m_UpPressed = false;
}

void Player::stopDown(){
	m_DownPressed = false;
}

void Player::spawn(IntRect arena, Vector2f resolution, int tileSize){
	// Place the player in the middle of the arena
	m_Position.x = arena.width / 2;
	m_Position.y = arena.height / 2;

	// Copy the details of the arena to the player's m_Arena
	m_Arena.left = arena.left;
	m_Arena.width = arena.width;
	m_Arena.top = arena.top;
	m_Arena.height = arena.height;

	// Remember how big the tiles are in this arena
	m_TileSize = tileSize;

	// Store the resolution for future use
	m_Resolution.x = resolution.x;
	m_Resolution.y = resolution.y;
}

void Player::update(float elapsedTime, Vector2i mouseScreenPosition){
	// Move the player from the current position to where the player moves
	if(m_UpPressed){
		m_Position.y -= m_Speed * elapsedTime;
	}
	if(m_DownPressed){
		m_Position.y += m_Speed * elapsedTime;
	}
	if(m_RightPressed){
		m_Position.x +=  m_Speed * elapsedTime;
	}
	if(m_LeftPressed){
		m_Position.x -=  m_Speed * elapsedTime;
	}

	m_Sprite.setPosition(m_Position);

	// Keep the player inside the arena
	if(m_Position.x > m_Arena.width - m_TileSize){
		m_Position.x = m_Arena.width - m_TileSize;
	}
	if(m_Position.x < m_Arena.left + m_TileSize){
		m_Position.x = m_Arena.left + m_TileSize;
	}
	if(m_Position.y < m_Arena.top + m_TileSize){
		m_Position.y = m_Arena.top + m_TileSize;
	}
	if(m_Position.y > m_Arena.height - m_TileSize){
		m_Position.y = m_Arena.height - m_TileSize;
	}

	// Calculate the angle the player is facing and rotate it to the direction the cursor is currently facing
	float angle;
	// Calculate the difference betweem x and y position of the cursor position to the player present at the center of the arena (dx,dy)
	float dx = mouseScreenPosition.x - m_Resolution.x/2;
	float dy = mouseScreenPosition.y - m_Resolution.y/2;
	// Use the formula of atan2(dx,dy) -> this gives the result in radians
	// Convert the angle in radians to degrees using the formula -> "rad*(180/pi)""
	angle = (atan2(dy,dx)*180)/3.141;

	// Set the rotation and NOT the position of the player
	m_Sprite.setRotation(angle);
}
