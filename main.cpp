#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <queue>
#include <array>
using std::string;

void readWay_(const string& file, std::vector<sf::Vector2f>& coordinates, std::vector<std::vector<size_t>>& alist) {
	// name of file; coordinates; adjacency list
	sf::Vector2f help;
	std::ifstream reader(file);
	for (size_t i = 0; !reader.eof(); i++) {
		reader >> help.x;
		reader >> help.y;
		coordinates.push_back(help);
		size_t nodeCount;
		reader >> nodeCount;
		std::vector<size_t> nodes(nodeCount);
		for (size_t node = 0; node < nodeCount; node++) {
			reader >> nodes[node];
		}
		alist.push_back(nodes);
	}
	reader.close();
}

void readWay(const string& file, std::vector<sf::Vector2f>& coordinates, std::vector<std::vector<std::pair<size_t, float>>>& alist) {
	sf::Vector2f help;
	std::ifstream reader(file);
	for (size_t i = 0; !reader.eof(); i++) {
		reader >> help.x;
		reader >> help.y;
		coordinates.push_back(help);
		size_t nodeCount;
		std::cout << "for " << i;
		reader >> nodeCount;
		std::vector<std::pair<size_t, float>> nodes(nodeCount);
		for (size_t node = 0; node < nodeCount; node++) {
			reader >> nodes[node].first;
			reader >> nodes[node].second;
		}
		alist.push_back(nodes);
	}
	reader.close();
	std::cout << "COORDINATES" << '\n';
	for (size_t i = 0; i < coordinates.size(); i++) {
		std::cout << coordinates[i].x << " " << coordinates[i].y << '\n';
	}

}

float distance(int x1, int y1, int x2, int y2, float k) {
	float helpx = abs(x1 - x2);
	float helpy = abs(y1 - y2);
	return sqrt(helpx * helpx + helpy * helpy) * k;
}

size_t nearestVertex(sf::Vector2f coordinates, std::vector<sf::Vector2f>& roadCoordinates) {
	float min = INFINITY;
	size_t result;
	for (size_t i = 0; i < roadCoordinates.size(); i++) {
		float helpa = distance(coordinates.x, coordinates.y, roadCoordinates[i].x, roadCoordinates[i].y, 2);
		if (helpa < min) {
			min = helpa;
			result = i;
		}
	}
	return result;
}

template <typename T, typename U, typename V>
struct tripple {
	T first;
	U second;
	V third;
};

std::vector<size_t> findShortestPath(size_t first, size_t second, std::vector<sf::Vector2f>& roadCoordinates, const std::vector<std::vector<std::pair<size_t, float>>>& roadAlist) { // using Dijkstra here
	auto cmp = [](tripple<size_t, float, size_t> a, tripple<size_t, float, size_t> b) {return a.second < b.second;};
	std::priority_queue <tripple<size_t, float, size_t>, std::vector<tripple<size_t, float, size_t>>, decltype(cmp)> q(cmp); // source distance(sort) dest
	std::vector<float> distances(roadAlist.size(), INFINITY);
	std::vector<size_t> optimalsource(roadAlist.size(), roadAlist.size());
	distances[first] = 0;
	q.push(tripple<size_t, float, size_t>{first, 0, first});
	while (!(q.empty())) {
		size_t source = q.top().third;
		//size_t ssource = q.top().first;
		q.pop();
		for (size_t i = 0; i < roadAlist[source].size(); i++) {
			size_t v = roadAlist[source][i].first;
			float dist = distance(roadCoordinates[source].x, roadCoordinates[source].y, roadCoordinates[v].x, roadCoordinates[v].y, roadAlist[source][i].second);
			if (distances[source] + dist < distances[v]) {
				q.push(tripple<size_t, float, size_t>{source, dist, v});
				distances[v] = distances[source] + dist;
				optimalsource[v] = source;
			}
		}
	}
	std::vector<size_t> path;
	for (size_t i = second; i != first; i = optimalsource[i]) {
		path.insert(path.end(), i);
	}
	path.insert(path.end(), first);
	std::cout << "Dijkstra worked";
	return path;
}

std::vector<size_t> findShortestPathTrans(sf::Vector2f first, sf::Vector2f second, std::vector<sf::Vector2f>& coordinates, std::vector<std::vector<std::pair<size_t, float>>>& Alist) {
	coordinates.push_back(first);
	std::vector<std::pair<size_t, float>> helpadj(Alist.size());
	for (size_t i = 0; i < Alist.size(); i++) {
		helpadj[i].first = i;
		helpadj[i].second = 2;
	}
	Alist.push_back(helpadj);
	for (size_t i = 0; i < Alist.size(); i++) {
		Alist[i].push_back(std::pair<size_t, float>{Alist.size(), 2});
	}
	coordinates.push_back(second);
	Alist.push_back(std::vector<std::pair<size_t, float>>{});
	std::vector<size_t> ans = findShortestPath(Alist.size() - 2, Alist.size() - 1, coordinates, Alist);
	coordinates.pop_back();coordinates.pop_back();
	Alist.pop_back();Alist.pop_back();
	for (size_t i = 0; i < Alist.size(); i++) {
		Alist[i].pop_back();
	}
	ans.pop_back();
	ans.erase(ans.begin());
	return ans;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Navigation");

	sf::Texture background;
	sf::Texture roads;
	sf::Texture transport;
	sf::Texture redPoint;
	sf::Texture bluePoint;

	background.loadFromFile("./images/background.png");
	roads.loadFromFile("./images/roads1.png");
	transport.loadFromFile("./images/public1.png");
	redPoint.loadFromFile("./images/red.png");
	bluePoint.loadFromFile("./images/blue.png");

	sf::Sprite backgroundRender(background); 
	sf::Sprite roadsRender(roads);
	sf::Sprite publicTransportRender(transport);
	sf::Sprite redRender(redPoint);
	sf::Sprite blueRender(bluePoint);

	std::vector<sf::Vector2f> roadCoordinates;
	std::vector<sf::Vector2f> transCoordinates;
	std::vector<std::vector<std::pair<size_t, float>>>roadAlist;
	std::vector<std::vector<std::pair<size_t, float>>>transAlist;
	std::vector<size_t> pathSave;

	readWay("roads.way", roadCoordinates, roadAlist);
	readWay("trans.way", transCoordinates, transAlist);
	redRender.setPosition(-100, -100);
	blueRender.setPosition(-100, -100);

	bool onFoot = true, second = false, first = false;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed && sf::Keyboard::Space == event.key.code) {
				onFoot = !onFoot;
				std::cout << "SPACE IS PRESSED\n";
				if (onFoot) {
					pathSave = findShortestPathTrans(blueRender.getPosition(), redRender.getPosition(), transCoordinates, transAlist);
				}
				else {
					pathSave = findShortestPath(nearestVertex(blueRender.getPosition(), roadCoordinates), nearestVertex(redRender.getPosition(), roadCoordinates), roadCoordinates, roadAlist);
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				if (sf::Mouse::Left == event.key.code) {
					std::cout << "LeftMouse IS PRESSED\n";
					blueRender.setPosition(pos.x, pos.y);
					first = true;
				}
				else if (sf::Mouse::Right == event.key.code) {
					std::cout << "RightMouse IS PRESSED\n";
					redRender.setPosition(pos.x, pos.y);
					second = true;
				}
				if (second && first) {
					std::cout << "second and first";
					if (onFoot) {
						pathSave = findShortestPathTrans(blueRender.getPosition(), redRender.getPosition(), transCoordinates, transAlist);
					}
					else {
						pathSave = findShortestPath(nearestVertex(blueRender.getPosition(), roadCoordinates), nearestVertex(redRender.getPosition(), roadCoordinates), roadCoordinates, roadAlist);
					}
				}
			}
		}

		window.clear(sf::Color::White);
		window.setKeyRepeatEnabled(false);
		window.draw(backgroundRender);
		window.draw(roadsRender);
		if (onFoot) {
			window.draw(publicTransportRender);
		}
		if (pathSave.size()) {
			sf::Vertex line1[] =
			{
					sf::Vertex(redRender.getPosition()),
					sf::Vertex((!onFoot) ? roadCoordinates[pathSave[0]] : transCoordinates[pathSave[0]]) // pathSave[pathSave.size() - pathSave.size()]
			};
			sf::Vertex line2[] =
			{
					sf::Vertex(blueRender.getPosition()),
					sf::Vertex((!onFoot) ? roadCoordinates[pathSave[pathSave.size() - 1]] : transCoordinates[pathSave[pathSave.size() - 1]])// FIRST - BLUE SECOND - RED
			};
			window.draw(line1, 10, sf::Lines);
			window.draw(line2, 10, sf::Lines);
			std::vector<sf::Vector2f>* arr[2] = { &roadCoordinates, &transCoordinates };
			for (int i = 1; i < pathSave.size(); i++) {
				sf::Vector2f a = (*(arr[onFoot]))[pathSave[i - 1]];
				sf::Vector2f b = (*(arr[onFoot]))[pathSave[i]];
				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(a)),
					sf::Vertex(sf::Vector2f(b))
				};
				window.draw(line, 10, sf::Lines); 

			}
		}
		if (pathSave.size() < 1 && second && first) {
			sf::Vertex line5[]{
				sf::Vertex(sf::Vector2f(blueRender.getPosition())),
				sf::Vertex(sf::Vector2f(redRender.getPosition())),
			};
			window.draw(line5, 10, sf::Lines);
		}
		else {
			for (int i = 0; i < pathSave.size(); i++) {
				std::cout << '\n' << "path is " << pathSave[i];
			}
		}
		window.draw(redRender);
		window.draw(blueRender);
		window.display();
	}
}