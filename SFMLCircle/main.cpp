#include <iostream>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

static const int WINDOW_WIDTH = 1280;				// window width (pixels)
static const int WINDOW_HEIGHT = 720;				// window height (pixels)
static const float PI = 3.14159265359;				// PI constant
static const float ANGLE_STEP = 2.0 * PI / 50.0;	// angle step to construct circle (rad)
static const float ROTATION_SPEED = 50.0;			// segment rotation speed (deg/sec)
static const float TRANSLATION_SPEED = 200.0;		// segment transletion speed (pixels/sec)
static const int TIME_INTERVAL = 2;					// time interval to switch modes (sec)

enum class Mode
{
	ARC,
	SECTOR,
	ROTATING_SECTOR,
	COUNT
};

struct UserInput
{
	sf::Vector2f center = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
	float radius = 100.0;
	float start_angle = 0.0;
	float end_angle = 1.5 * PI;
};

UserInput getUserInput()
{
	// TODO: get user input
	// TODO: get arc as two points (use atan2f())

	return UserInput();
}

sf::VertexArray prepareVertices(const UserInput& user_input)
{
	float current_angle = user_input.start_angle;
	sf::Vector2f new_point;
	sf::VertexArray result;

	bool done = false;
	while (!done)
	{
		if ((current_angle > user_input.end_angle) || fabs(current_angle - user_input.end_angle) < FLT_EPSILON)
		{
			current_angle = user_input.end_angle;
			done = true;
		}

		new_point = user_input.center;
		new_point += user_input.radius * sf::Vector2f(cosf(current_angle), -sinf(current_angle));
		result.append(new_point);

		current_angle += ANGLE_STEP;
	}

	result.setPrimitiveType(sf::PrimitiveType::LinesStrip);
	return result;
}

// TODO: don't use ConvexShape
sf::ConvexShape prepareSector(const UserInput& user_input, const sf::VertexArray& vertices)
{
	sf::ConvexShape sector(vertices.getVertexCount() + 1);

	for (int i = 0; i < vertices.getVertexCount(); ++i)
	{
		sector.setPoint(i, vertices[i].position);
	}

	sector.setPoint(vertices.getVertexCount(), user_input.center);

	sector.setFillColor(sf::Color::White);
	sector.setOutlineColor(sf::Color::White);

	return sector;
}

int main()
{
	const UserInput user_input = getUserInput();

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Draw circle");
	window.setVerticalSyncEnabled(true);

	sf::Clock main_timer;
	sf::Clock frame_timer;
	int frame_time = 0;			// milliseconds

	Mode mode = Mode::ARC;
	sf::VertexArray vertices = prepareVertices(user_input);
	sf::ConvexShape sector = prepareSector(user_input, vertices);
	sf::Transform rotation_transform;
	sf::Transform translation_transform;

	frame_timer.restart();
	while (window.isOpen())
	{
		// узнаем, сколько времени в миллисекундах прошло между кадрами
		frame_time = frame_timer.getElapsedTime().asMilliseconds();
		frame_timer.restart();

		window.clear(sf::Color::Black);

		switch (mode)
		{
		case Mode::ARC:

			window.draw(vertices);
			break;

		case Mode::SECTOR:

			window.draw(sector);
			break;

		case Mode::ROTATING_SECTOR:

			// TODO: think about it
			translation_transform.translate(
				sf::Vector2f(
					TRANSLATION_SPEED * (float)frame_time * 0.001f,
					0.0
				)
			);

			rotation_transform.rotate(ROTATION_SPEED * (float)frame_time * 0.001f, user_input.center);
			break;

		default:

			throw std::exception("Wrong mode!");
			break;
		}

		window.display();

		if (main_timer.getElapsedTime().asSeconds() > TIME_INTERVAL)
		{
			main_timer.restart();
			if (mode != Mode::ROTATING_SECTOR)
			{
				mode = (Mode)((int)mode + 1);
			}
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
	}

	return 0;
}