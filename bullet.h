#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
using namespace sf;

class Bullet{
	private:
		Vector2f m_Position;
		RectangleShape m_BulletShape;
		bool m_InFlight = false;
		float m_BulletSpeed = 1000;
		float m_BulletSpeedX;
		float m_BulletSpeedY;

		// Boundaries so the bullet doesn't fly forever
		float m_MaxX;
		float m_MinX;
		float m_MaxY;
		float m_MinY;
	public:
		Bullet();
		void stop();
		bool isInFlight();
		FloatRect getPosition();
		RectangleShape getShape();

		void shoot(float startX, float startY, float targetX, float targetY);
		void update(float elapsedTime);
};

Bullet::Bullet(){
	m_BulletShape.setSize(Vector2f(5, 5));
	m_BulletShape.setFillColor(Color::Yellow);
}

void Bullet::stop(){
	m_InFlight = false;
}

bool Bullet::isInFlight(){
	return m_InFlight;
}

FloatRect Bullet::getPosition(){
	return m_BulletShape.getGlobalBounds();
}

RectangleShape Bullet::getShape(){
	return m_BulletShape;
}

void Bullet::shoot(float startX, float startY, float targetX, float targetY){
	m_InFlight = true;
	m_Position.x = startX;
	m_Position.y = startY;

	// Gradient approach
	float gradient = (targetY - startY) / (targetX - startX);
	if(gradient < 0){
		gradient = gradient*-1;
	}

	m_BulletSpeedX = m_BulletSpeed * (1.0/(1 + gradient));
	m_BulletSpeedY = m_BulletSpeedX * gradient;

	// Fix the directions
	if(targetX < startX){
		m_BulletSpeedX *= -1;
	}
	if(targetY < startY){
		m_BulletSpeedY *= -1;
	}

	// Set a max range of 1000 pixels
	float range = 1000;
	m_MinX = startX - range;
	m_MaxX = startX + range;
	m_MinY = startY - range;
	m_MaxY = startY + range;

	m_BulletShape.setPosition(m_Position);
}

void Bullet::update(float elapsedTime){
	// Update the bullet position variables
	m_Position.x += m_BulletSpeedX * elapsedTime;
	m_Position.y += m_BulletSpeedY * elapsedTime;

	// Move the bullet
	m_BulletShape.setPosition(m_Position);

	// Has the bullet gone out of range?
	if(m_Position.x < m_MinX || m_Position.x > m_MaxX || m_Position.y < m_MinY || m_Position.y > m_MaxY){
		m_InFlight = false;
	}
}
