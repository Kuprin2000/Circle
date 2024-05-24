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

bool verticesFromPoints(sf::VertexArray& vertices, sf::Vector2f& center)
{
	std::cout << "Введите координату x центра окружности: ";
	std::cin >> center.x;
	std::cout << std::endl << "Введите координату y центра окружности: ";
	std::cin >> center.y;

	sf::Vector2f M1;
	sf::Vector2f M2;
	std::cout << "Введите координату x первой точки, принадлежащей дуге окружности: ";
	std::cin >> M1.x;
	std::cout << std::endl << "Введите координату y: ";
	std::cin >> M1.y;
	std::cout << std::endl;
	std::cout << "Введите координату x второй точки: ";
	std::cin >> M2.x;
	std::cout << std::endl << "Введите координату y: ";
	std::cin >> M2.y;

	sf::Vector2f radius_vector_1 = M1 - center;
	sf::Vector2f radius_vector_2 = M2 - center;
	float radius_1 = hypotf(radius_vector_1.x, radius_vector_1.y);
	float radius_2 = hypotf(radius_vector_2.x, radius_vector_2.y);

	if (fabs(radius_1 - radius_2) > FLT_EPSILON)
	{
		return false;
	}

	float start_angle = atan2(-radius_vector_1.y, radius_vector_1.x);
	float finish_angle = atan2(-radius_vector_2.y, radius_vector_2.x);

	if (start_angle > finish_angle)
	{
		std::swap(start_angle, finish_angle);
	}

	float current_angle = start_angle;
	sf::Vector2f new_point;

	bool done = false;
	while (!done)
	{
		if ((current_angle > finish_angle) || fabs(current_angle - finish_angle) < FLT_EPSILON)
		{
			current_angle = finish_angle;
			done = true;
		}

		new_point = center;
		new_point += radius_1 * sf::Vector2f(cosf(current_angle), -sinf(current_angle));
		vertices.append(new_point);

		current_angle += ANGLE_STEP;
	}

	vertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);

	return true;
}

void verticesFromAngles(sf::VertexArray& vertices, sf::Vector2f& center)
{
	float radius;
	std::cout << "Введите координату x центра окружности: ";
	std::cin >> center.x;
	std::cout << std::endl << "Введите координату y центра окружности: ";
	std::cin >> center.y;
	std::cout << std::endl << "Введите радиус окружности: ";
	std::cin >> radius;

	float start_angle;
	float finish_angle;
	std::cout << "Введите угол начала дуги: ";
	std::cin >> start_angle;
	std::cout << std::endl << "Введите угол конца дуги: ";
	std::cin >> finish_angle;

	float current_angle = start_angle;
	sf::Vector2f new_point;

	bool done = false;
	while (!done)
	{
		if ((current_angle > finish_angle) || fabs(current_angle - finish_angle) < FLT_EPSILON)
		{
			current_angle = finish_angle;
			done = true;
		}

		new_point = center;
		new_point += radius * sf::Vector2f(cosf(current_angle), -sinf(current_angle));
		vertices.append(new_point);

		current_angle += ANGLE_STEP;
	}

	vertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);
}

sf::ConvexShape prepareSector(sf::Vector2f& center, const sf::VertexArray& vertices)
{
	sf::ConvexShape sector(vertices.getVertexCount() + 1);

	for (int i = 0; i < vertices.getVertexCount(); ++i)
	{
		sector.setPoint(i, vertices[i].position);
	}

	sector.setPoint(vertices.getVertexCount(), center);

	sector.setFillColor(sf::Color::White);
	sector.setOutlineColor(sf::Color::White);

	return sector;
}

int main()
{
	setlocale(LC_ALL, "russian");

	sf::Vector2f center{};
	sf::VertexArray vertices;

	std::cout << "Выберите способ задания дуги: " << std::endl;
	std::cout << "1. Координаты точек" << std::endl;
	std::cout << "2. Углы " << std::endl;

	int choice;
	std::cin >> choice;

	bool by_dots = false;

	switch (choice)
	{
	case 1:
		by_dots = true;
		break;
	case 2:
		break;
	default:
		std::cout << "Неравильный вариант!";
		return 0;
		break;
	}

	if (by_dots)
	{
		verticesFromPoints(vertices, center);
	}
	else
	{
		verticesFromAngles(vertices, center);
	}

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Draw circle");
	window.setVerticalSyncEnabled(true);

	sf::Clock main_timer;
	sf::Clock frame_timer;
	int frame_time = 0;			// milliseconds
	int mode_time = 0;			// milliseconds

	Mode mode = Mode::ARC;

	sf::ConvexShape sector = prepareSector(center, vertices);
	sf::Transform rotation_transform;
	sf::Transform translation_transform;

	frame_timer.restart();
	while (window.isOpen())
	{
		// узнаем, сколько времени в миллисекундах прошло между кадрами
		frame_time = frame_timer.getElapsedTime().asMilliseconds();
		frame_timer.restart();

		mode_time = main_timer.getElapsedTime().asMilliseconds();

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

			rotation_transform = sf::Transform::Identity;
			rotation_transform.rotate(-ROTATION_SPEED * (float)mode_time * 0.001f, 0.0f, 0.0f);

			translation_transform = sf::Transform::Identity;
			translation_transform.translate(sf::Vector2f(
				TRANSLATION_SPEED * (float)mode_time * 0.001f,
				0.0)
			);

			rotation_transform = sf::Transform::Identity;
			rotation_transform.rotate(ROTATION_SPEED * (float)mode_time * 0.001f, translation_transform.transformPoint(center));

			window.draw(sector, rotation_transform.combine(translation_transform));

			break;

		default:

			throw std::exception("Wrong mode!");
			break;
		}

		window.display();

		if (main_timer.getElapsedTime().asSeconds() > TIME_INTERVAL && mode != Mode::ROTATING_SECTOR)
		{
			main_timer.restart();
			mode = (Mode)((int)mode + 1);
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