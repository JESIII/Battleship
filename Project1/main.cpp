#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
using namespace std;
class cell {
public:
	sf::RectangleShape rect;
	bool ship = false;
	bool found = false;
};
bool login(size_t *player) {
	string firstword;
	cout << "To play a game type:\n'connect 192.168.0.1' or 'host'\n";
	cin >> firstword;
	if (firstword == "host") {
		sf::TcpListener listener;
		// bind the listener to a port
		if (listener.listen(53000) != sf::Socket::Done)
		{
			cout << "Connection error.\n";
			return false;
		}

		// accept a new connection
		sf::TcpSocket client;
		if (listener.accept(client) != sf::Socket::Done)
		{
			cout << "Connection error.\n";
			return false;
		}
		cout << "Successfully became host\n";
		*player = 1;
	}
	else {
		*player = 2;
		string ip;
		cin >> ip;
		sf::TcpSocket socket;
		sf::Socket::Status status = socket.connect(ip, 53000);
		if (status != sf::Socket::Done)
		{
			cout << "Connection error.\n";
			return false;
		}
		cout << "Successfully became client\n";
	}
}
int main()
{
	bool strategyphase = true, turnphase = false, moved = false;
	size_t player, whosturn = 1;
	sf::TcpSocket socket, client;
	sf::TcpListener listener;
	//if (!login(&player)) return 0;
	string firstword;
	cout << "To play a game type:\n'connect 192.168.0.1' or 'host'\n";
	cin >> firstword;
	if (firstword == "host") {
		// bind the listener to a port
		if (listener.listen(53000) != sf::Socket::Done)
		{
			cout << "Connection error.\n";
			return false;
		}

		// accept a new connection
		if (listener.accept(client) != sf::Socket::Done)
		{
			cout << "Connection error.\n";
			return false;
		}
		cout << "Successfully became host\n";
		player = 1;
	}
	else {
		player = 2;
		string ip;
		cin >> ip;
		sf::Socket::Status status = socket.connect(ip, 53000);
		if (status != sf::Socket::Done)
		{
			cout << "Connection error.\n";
			return false;
		}
		cout << "Successfully became client\n";
	}
	sf::RenderWindow window(sf::VideoMode(400, 806), "Battleship");
	window.clear(sf::Color::Black);
	size_t mousey = sf::Mouse::getPosition(window).y / 40;
	size_t mousex = sf::Mouse::getPosition(window).x / 40;
	sf::RectangleShape line(sf::Vector2f(400.f, 2.f));
	line.setPosition(0, 402);
	line.setFillColor(sf::Color::Red);
	cell homerects[10][10];
	cell awayrects[10][10];
	for (size_t i = 0; i < 10; i++) {
		for (size_t j = 0; j < 10; j++) {
			homerects[i][j].rect.setSize(sf::Vector2f(40, 40));
			homerects[i][j].rect.setPosition(i * 40, j * 40 + 406);
			homerects[i][j].rect.setOutlineColor(sf::Color::Black);
			homerects[i][j].rect.setOutlineThickness(2);
			awayrects[i][j].rect.setSize(sf::Vector2f(40, 40));
			awayrects[i][j].rect.setPosition(i * 40, j * 40);
			awayrects[i][j].rect.setOutlineColor(sf::Color::Black);
			awayrects[i][j].rect.setOutlineThickness(2);
			awayrects[i][j].found = false;
			homerects[i][j].found = false;
			homerects[i][j].ship = false;
			awayrects[i][j].ship = false;
		}
	}
	size_t placed = 1;
	size_t size;
	while (window.isOpen())
	{
		mousey = sf::Mouse::getPosition(window).y / 40;
		mousex = sf::Mouse::getPosition(window).x / 40;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		for (size_t i = 0; i < 10; i++) {
			for (size_t j = 0; j < 10; j++) {
				if (homerects[i][j].ship == false) {
					window.draw(homerects[i][j].rect);
				}
				window.draw(awayrects[i][j].rect);
			}
		}
		window.draw(line);
		if (strategyphase) {
			char orientation = 'v';
			switch (placed) {
			case 1:
				size = 5;
				break;
			case 2:
				size = 4;
				break;
			case 3:
				size = 3;
				break;
			case 4:
				size = 3;
				break;
			case 5:
				size = 2;
				break;
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && orientation == 'v') orientation = 'h';
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && orientation == 'h') orientation = 'v';
			for (size_t i = 0; i < 10; i++) {
				for (size_t j = 0; j < 10; j++) {
					if (homerects[i][j].ship == false)homerects[i][j].rect.setFillColor(sf::Color::White);
				}
			}
			for (size_t s = 0; s < size; s++) {
				if (orientation == 'v') homerects[mousex % 10][(mousey + s - 406) % 10].rect.setFillColor(sf::Color::Blue);
				else homerects[(mousex + s)% 10][(mousey - 406) % 10 ].rect.setFillColor(sf::Color::Blue);
			}
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						for (size_t s = 0; s < size; s++) {
							if (orientation == 'v') {
								homerects[mousex % 10][(mousey + s - 406) % 10].rect.setFillColor(sf::Color::Blue);
								homerects[mousex % 10][(mousey + s - 406) % 10].ship = true;
							}
							else {
								homerects[(mousex + s) % 10][(mousey - 406) % 10].rect.setFillColor(sf::Color::Blue);
								homerects[(mousex + s) % 10][(mousey - 406) % 10].ship = true;
							}
						}
						placed++;
					}
				}
			}
			if (placed == 6) {
				for (size_t i = 0; i < 10; i++) {
					for (size_t j = 0; j < 10; j++) {
						if (homerects[i][j].ship == true) {
							sf::Packet packet;
							packet.append(&homerects[i][j], sizeof(cell));
							if (player == 2) {
								if (socket.send(packet) != sf::Socket::Done)
								{
									cout << "Sent packet.";
								}
							}
							else if (player == 1) {
								if (client.send(packet) != sf::Socket::Done)
								{
									cout << "Sent packet.";
								}
							}
						}
					}
				}
				size_t x,y;
				sf::Packet packet;
				if (player == 1) {
					while (client.receive(packet)) {
						packet >> x >> y;
						awayrects[x][y].ship = true;
						cout << "Received packet.";
					}
				}
				else {
					while (socket.receive(packet)) {
						packet >> x >> y;
						awayrects[x][y].ship = true;
						cout << "Received packet.";
					}
				}
				strategyphase = false;
				turnphase = true;
				cout << "Strategy Phase Ended\n";
				cout << "Turn Phase Begins.\n";
			}
		}
		else if (turnphase) {
			if (player == 1) {
				if (whosturn == 1) {
					while (window.pollEvent(event)) {
						if (event.type == sf::Event::MouseButtonPressed)
						{
							if (event.mouseButton.button == sf::Mouse::Left)
							{
								if (awayrects[mousex % 10][mousey % 10].found == false && awayrects[mousex % 10][mousey % 10].ship == true) {
									awayrects[mousex % 10][mousey % 10].rect.setFillColor(sf::Color::Red);
									whosturn = 2;
								}
								else if (awayrects[mousex % 10][mousey % 10].found == false && awayrects[mousex % 10][mousey % 10].ship == false) {
									awayrects[mousex % 10][mousey % 10].rect.setFillColor(sf::Color::Yellow);
									whosturn = 2;
								}
							}
						}
					}
				}
			}
			else if (player == 2) {
				if (whosturn == 2) {
					while (window.pollEvent(event)) {
						switch (event.type) {
						case sf::Event::MouseButtonPressed:
							if (event.mouseButton.button == sf::Mouse::Left)
							{
								if (mousey <= 400) {
									if (awayrects[mousex % 10][mousey % 10].found == false && awayrects[mousex % 10][mousey % 10].ship == true) {
										awayrects[mousex % 10][mousey % 10].rect.setFillColor(sf::Color::Red);
										whosturn = 2;
									}
									else if (awayrects[mousex % 10][mousey % 10].found == false && awayrects[mousex % 10][mousey % 10].ship == false) {
										awayrects[mousex % 10][mousey % 10].rect.setFillColor(sf::Color::Yellow);
										whosturn = 2;
									}
								}
							}
							break;
						}
					}
				}
			}
		}
		window.display();
	}
	return 0;
}