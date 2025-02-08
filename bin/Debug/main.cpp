#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <chrono>
#include <thread>
#include <sstream>
#include <SFML/Graphics.hpp>

class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;
    bool isHovered; // Add a boolean to track hover state

    Button(const std::string& label, const sf::Font& font, sf::Vector2f size, sf::Vector2f position, sf::Color color)
        : shape(size), text(label, font, 18), isHovered(false) {
        shape.setPosition(position);
        shape.setFillColor(color);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);

        text.setFillColor(sf::Color::White);
        text.setPosition(
            position.x + (size.x - text.getLocalBounds().width) / 2,
            position.y + (size.y - text.getLocalBounds().height) / 2 - 5
        );
    }

    bool isClicked(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) {
        if (isHovered) {
            shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, 200));
            shape.setOutlineThickness(3);
            shape.setOutlineColor(sf::Color::Yellow);
            text.setFillColor(sf::Color::Yellow);
        } else {
            shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, 255));
            shape.setOutlineThickness(2);
            shape.setOutlineColor(sf::Color::White);
            text.setFillColor(sf::Color::White);
        }
        window.draw(shape);
        window.draw(text);
    }

    void setAlpha(int alpha) {
        shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
        text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
    }
};

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    bool isEnd;
    sf::CircleShape circle;
    sf::Text charText;
    sf::Vector2f position;

    TrieNode(sf::Font& font) : isEnd(false) {
        circle.setRadius(20);
        circle.setFillColor(sf::Color::Cyan);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);

        charText.setFont(font);
        charText.setCharacterSize(18);
        charText.setFillColor(sf::Color::Black);
    }

    void setPosition(sf::Vector2f pos) {
        position = pos;
        circle.setPosition(pos);
        charText.setPosition(pos.x + (circle.getRadius() - charText.getLocalBounds().width) / 2,
                             pos.y + (circle.getRadius() - charText.getLocalBounds().height) / 2 - 5);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(circle);
        window.draw(charText);
    }
};

class TrieVisualizer {
public:
    TrieNode* root;
    sf::Font font;
    std::vector<TrieNode*> nodes;
    std::vector<std::pair<TrieNode*, TrieNode*>> edges;
    sf::RectangleShape scrollbar;
    sf::RectangleShape scrollbarHandle;
    float scrollOffset;
    float maxScrollOffset;
    bool isScrolling;

    TrieVisualizer() : scrollOffset(0), maxScrollOffset(0), isScrolling(false) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
        root = new TrieNode(font);
        root->charText.setString(" ");
        nodes.push_back(root);

        scrollbar.setSize(sf::Vector2f(20, 400));
        scrollbar.setPosition(sf::Vector2f(760, 200));
        scrollbar.setFillColor(sf::Color::Cyan);

        scrollbarHandle.setSize(sf::Vector2f(20, 50));
        scrollbarHandle.setFillColor(sf::Color::White);
    }

    void insert(const std::string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                TrieNode* newNode = new TrieNode(font);
                newNode->charText.setString(std::string(1, ch));
                current->children[ch] = newNode;
                nodes.push_back(newNode);
                edges.push_back({current, newNode});
            }
            current = current->children[ch];
        }
        current->isEnd = true;
    }

    void calculatePositions(TrieNode* node, sf::Vector2f position, float horizontalSpacing, float verticalSpacing) {
        if (!node) return;
        node->setPosition(position);
        float x = position.x;
        float y = position.y + verticalSpacing;
        int count = 0;
        for (auto& pair : node->children) {
            calculatePositions(pair.second, sf::Vector2f(x + count * horizontalSpacing, y), horizontalSpacing, verticalSpacing);
            count++;
        }
    }

    void draw(sf::RenderWindow& window) {
        if (!root) return;

        calculatePositions(root, sf::Vector2f(100, 200), 50, 100);

        for (auto& edge : edges) {
            sf::Vector2f parentPos = edge.first->circle.getPosition();
            sf::Vector2f childPos = edge.second->circle.getPosition();
            parentPos.x += 20;
            parentPos.y += 40;
            childPos.x += 20;
            parentPos.y -= scrollOffset;
            childPos.y -= scrollOffset;
            sf::Vertex line[] = { sf::Vertex(parentPos), sf::Vertex(childPos) };
            window.draw(line, 2, sf::Lines);
        }

        for (auto& node : nodes) {
            sf::Vector2f pos = node->circle.getPosition();
            pos.y -= scrollOffset;
            node->circle.setPosition(pos);
            node->charText.setPosition(pos.x + (node->circle.getRadius() - node->charText.getLocalBounds().width) / 2,
                                       pos.y + (node->circle.getRadius() - node->charText.getLocalBounds().height) / 2 - 5);
            node->draw(window);
        }

        window.draw(scrollbar);
        window.draw(scrollbarHandle);
    }

    void reset() {
        for (auto node : nodes) delete node;
        nodes.clear();
        edges.clear();
        root = new TrieNode(font);
        root->charText.setString(" ");
        nodes.push_back(root);
        scrollOffset = 0;
        maxScrollOffset = 0;
    }

    void handleScrollbar(sf::Vector2f mousePos) {
        if (scrollbar.getGlobalBounds().contains(mousePos)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                isScrolling = true;
            }
        }

        if (isScrolling) {
            float handleY = mousePos.y - scrollbar.getPosition().y;
            handleY = std::max(0.f, std::min(handleY, scrollbar.getSize().y - scrollbarHandle.getSize().y));
            scrollbarHandle.setPosition(scrollbar.getPosition().x, scrollbar.getPosition().y + handleY);
            scrollOffset = maxScrollOffset * (handleY / (scrollbar.getSize().y - scrollbarHandle.getSize().y));
        }

        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            isScrolling = false;
        }
    }

    void updateScrollbar() {
        float totalHeight = 200; // Initial y position of the root node
        for (auto& node : nodes) {
            totalHeight = std::max(totalHeight, node->circle.getPosition().y + 100); // Add vertical spacing
        }
        maxScrollOffset = std::max(0.f, totalHeight - 600); // Window height is 600
        scrollbarHandle.setSize(sf::Vector2f(20, std::max(50.f, 400.f * (600.f / totalHeight))));
    }

    void search(const std::string& word) {
        // Reset the color of all nodes
        for (auto& node : nodes) {
            node->circle.setFillColor(sf::Color::Cyan);
        }

        // Start the search from the root
        bool found = false;
        searchHelper(root, word, 0, found);
    }

    void searchHelper(TrieNode* node, const std::string& word, int index, bool& found) {
        if (!node || found) return;

        // Check if the current node's character is part of the search term
        if (index < word.size() && word.find(node->charText.getString()[0]) != std::string::npos) {
            // Highlight the current node
            node->circle.setFillColor(sf::Color::Yellow);

            // If the entire word is found, set found to true
            if (index == word.size() - 1) {
                found = true;
                return;
            }

            // Recursively search the next character in the children
            for (auto& pair : node->children) {
                searchHelper(pair.second, word, index + 1, found);
                if (found) return;
            }
        }

        // Check all children for the next character
        for (auto& pair : node->children) {
            searchHelper(pair.second, word, index, found);
            if (found) return;
        }
    }
};

class StackVisualizer {
public:
    std::vector<sf::RectangleShape> plates;
    std::vector<sf::Text> plateTexts;
    sf::Font font;

    StackVisualizer() {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
    }

    const int MAX_STACK_SIZE = 9;

    void push(int value) {
        if (plates.size() >= MAX_STACK_SIZE) {
            std::cout << "Stack is full! Cannot add more plates.\n";
            return;  // Prevent adding more plates
        }

        sf::RectangleShape plate(sf::Vector2f(100, 30));
        plate.setFillColor(sf::Color::Cyan);
        plate.setOutlineThickness(2);
        plate.setOutlineColor(sf::Color::White);

        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(value));
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);

        float yPos = 500 - (plates.size() * 40);
        plate.setPosition(350, yPos);
        text.setPosition(350 + (100 - text.getLocalBounds().width) / 2, yPos + 5);

        plates.push_back(plate);
        plateTexts.push_back(text);
    }

    void pop() {
        if (!plates.empty()) {
            plates.pop_back();
            plateTexts.pop_back();
        }
    }

    void draw(sf::RenderWindow& window) {
        for (size_t i = 0; i < plates.size(); ++i) {
            window.draw(plates[i]);
            window.draw(plateTexts[i]);
        }
    }

    void reset() {
        plates.clear();
        plateTexts.clear();
    }

    void search(int value) {
        for (size_t i = 0; i < plates.size(); ++i) {
            if (std::stoi(plateTexts[i].getString().toAnsiString()) == value) {
                plates[i].setFillColor(sf::Color::Yellow); // Highlight the found plate
                return;
            }
        }
        std::cout << "Number does not exist." << std::endl;
    }
};

class QueueNode {
public:
    int value;
    QueueNode* next;
    sf::CircleShape circle;
    sf::Text text;
    sf::RectangleShape arrow;

    QueueNode(int val, const sf::Font& font) : value(val), next(nullptr) {
        circle.setRadius(25);
        circle.setFillColor(sf::Color::Magenta);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);

        text.setFont(font);
        text.setString(std::to_string(val));
        text.setCharacterSize(15);
        text.setFillColor(sf::Color::White);
    }

    void setPosition(sf::Vector2f position) {
        circle.setPosition(position);
        text.setPosition(
            position.x + (circle.getRadius() - text.getLocalBounds().width) / 2 + 12,
            position.y + (circle.getRadius() - text.getLocalBounds().height) / 2 + 6
        );
        arrow.setPosition(position.x + 60, position.y + 20);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(circle);
        window.draw(text);
        if (next) window.draw(arrow);
    }
};

class QueueVisualizer {
public:
    QueueNode* front;
    QueueNode* rear;
    sf::Font font;
    std::vector<QueueNode*> nodes;
    std::vector<sf::RectangleShape> arrows;
    bool isAnimating = false;
    int animationStep = 0;

    QueueVisualizer() : front(nullptr), rear(nullptr) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
    }

    const int MAX_QUEUE_SIZE = 7;

    void enqueue(int value) {
        if (nodes.size() >= MAX_QUEUE_SIZE) {
            std::cout << "Queue is full! Cannot add more elements.\n";
            return;  // Prevent adding more elements
        }

        QueueNode* newNode = new QueueNode(value, font);
        nodes.push_back(newNode);

        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        updatePositions();
    }

    void dequeue() {
        if (!front) return;

        QueueNode* temp = front;
        front = front->next;
        if (!front) rear = nullptr;

        nodes.erase(nodes.begin());
        delete temp;
        updatePositions();
    }

    void updatePositions() {
        float startX = 100;
        float y = 300;
        for (auto& node : nodes) {
            node->setPosition(sf::Vector2f(startX, y));
            startX += 100;

            if (node->next) {
                node->arrow.setSize(sf::Vector2f(30, 2));
                node->arrow.setFillColor(sf::Color::White);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& node : nodes) {
            node->draw(window);
        }
    }

    void reset() {
        front = rear = nullptr;
        for (auto node : nodes) delete node;
        nodes.clear();
    }

    void search(int value) {
        for (auto& node : nodes) {
            if (node->value == value) {
                node->circle.setFillColor(sf::Color::Yellow); // Highlight the found node
                return;
            }
        }
        std::cout << "Number does not exist." << std::endl;
    }
};

class BSTNode {
public:
    int value;
    BSTNode* left;
    BSTNode* right;
    sf::CircleShape circle;
    sf::Text text;

    BSTNode(int val, const sf::Font& font) : value(val), left(nullptr), right(nullptr) {
        circle.setRadius(20);
        circle.setFillColor(sf::Color::Green);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);

        text.setFont(font);
        text.setString(std::to_string(val));
        text.setCharacterSize(15);
        text.setFillColor(sf::Color::White);
        text.setPosition(
            circle.getPosition().x + (circle.getRadius() - text.getLocalBounds().width) / 2,
            circle.getPosition().y + (circle.getRadius() - text.getLocalBounds().height) / 2 - 5
        );
    }

    void setPosition(sf::Vector2f position) {
        circle.setPosition(position);
        text.setPosition(
            position.x + (circle.getRadius() - text.getLocalBounds().width) / 2,
            position.y + (circle.getRadius() - text.getLocalBounds().height) / 2 - 5
        );
    }

    void draw(sf::RenderWindow& window) {
        window.draw(circle);
        window.draw(text);
    }
};

class BSTVisualizer {
public:
    BSTNode* root;
    sf::Font font;
    std::vector<std::pair<BSTNode*, BSTNode*>> edges;
    std::vector<BSTNode*> nodes;
    int currentStep = 0;
    bool isAnimating = false;

    BSTVisualizer() : root(nullptr) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
    }

    void insert(int value) {
        root = insertRecursive(root, value);
    }

    const int MAX_BST_DEPTH = 3;

    BSTNode* insertRecursive(BSTNode* node, int value, int depth = 0) {
        if (node == nullptr) {
            BSTNode* newNode = new BSTNode(value, font);
            nodes.push_back(newNode);
            return newNode;
        }

        if (value < node->value) {
            if (depth >= MAX_BST_DEPTH) {
                std::cout << "Max BST depth reached! Cannot insert more nodes.\n";
                return node;  // Prevent deeper insertion
            }
            node->left = insertRecursive(node->left, value, depth + 1);
            edges.push_back({node, node->left});
        } else if (value > node->value) {
            if (depth >= MAX_BST_DEPTH) {
                std::cout << "Max BST depth reached! Cannot insert more nodes.\n";
                return node;  // Prevent deeper insertion
            }
            node->right = insertRecursive(node->right, value, depth + 1);
            edges.push_back({node, node->right});
        }

        return node;
    }

    void drawTree(sf::RenderWindow& window, BSTNode* node, float x, float y, float horizontalSpacing = 100) {
        if (node == nullptr) return;

        node->setPosition(sf::Vector2f(x, y));

        for (int i = 0; i < currentStep && i < edges.size(); ++i) {
            auto& edge = edges[i];
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(edge.first->circle.getPosition().x + 20, edge.first->circle.getPosition().y + 40)),
                sf::Vertex(sf::Vector2f(edge.second->circle.getPosition().x + 20, edge.second->circle.getPosition().y))
            };
            window.draw(line, 2, sf::Lines);
        }

        for (int i = 0; i < currentStep && i < nodes.size(); ++i) {
            nodes[i]->draw(window);
        }

        if (node->left) {
            drawTree(window, node->left, x - horizontalSpacing, y + 120, horizontalSpacing / 2);
        }

        if (node->right) {
            drawTree(window, node->right, x + horizontalSpacing, y + 120, horizontalSpacing / 2);
        }
    }

    void updateAnimation() {
        if (isAnimating && currentStep < nodes.size() + edges.size()) {
            currentStep++;
        }
    }

    void draw(sf::RenderWindow& window) {
        drawTree(window, root, 400, 170, 100);
    }

    void reset() {
        root = nullptr;
        edges.clear();
        nodes.clear();
        currentStep = 0;
        isAnimating = false;
    }

    void search(int value) {
        BSTNode* current = root;
        while (current) {
            if (current->value == value) {
                current->circle.setFillColor(sf::Color::Yellow); // Highlight the found node
                return;
            } else if (value < current->value) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        std::cout << "Number does not exist." << std::endl;
    }
};

class AVLNode {
public:
    int value;
    int height;
    AVLNode* left;
    AVLNode* right;
    sf::CircleShape circle;
    sf::Text valueText;
    sf::Text balanceFactorText;

    AVLNode(int val, const sf::Font& font) : value(val), height(1), left(nullptr), right(nullptr) {
        circle.setRadius(20);
        circle.setFillColor(sf::Color::Blue);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);

        valueText.setFont(font);
        valueText.setString(std::to_string(val));
        valueText.setCharacterSize(18);
        valueText.setFillColor(sf::Color::White);

        balanceFactorText.setFont(font);
        balanceFactorText.setString("0");
        balanceFactorText.setCharacterSize(12);
        balanceFactorText.setFillColor(sf::Color::Yellow);
    }

    void setPosition(sf::Vector2f position) {
        circle.setPosition(position);
        valueText.setPosition(
            position.x + (circle.getRadius() - valueText.getLocalBounds().width) / 2,
            position.y + (circle.getRadius() - valueText.getLocalBounds().height) / 2 - 5
        );
        balanceFactorText.setPosition(
            position.x + circle.getRadius() - balanceFactorText.getLocalBounds().width / 2,
            position.y + circle.getRadius() * 2 - 5
        );
    }

    void updateBalanceFactor(int bf) {
        balanceFactorText.setString(std::to_string(bf));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(circle);
        window.draw(valueText);
        window.draw(balanceFactorText);
    }
};

class AVLVisualizer {
public:
    AVLNode* root;
    sf::Font font;
    std::vector<AVLNode*> nodes;
    bool isAnimating = false;

    AVLVisualizer() : root(nullptr) {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
    }

    int getHeight(AVLNode* node) {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;

        x->updateBalanceFactor(getBalance(x));
        y->updateBalanceFactor(getBalance(y));

        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;

        x->updateBalanceFactor(getBalance(x));
        y->updateBalanceFactor(getBalance(y));

        return y;
    }

    const int MAX_AVL_DEPTH = 3;

    AVLNode* insertRecursive(AVLNode* node, int value, int depth = 0) {
        if (!node) {
            AVLNode* newNode = new AVLNode(value, font);
            nodes.push_back(newNode);
            return newNode;
        }

        if (value < node->value) {
            if (depth >= MAX_AVL_DEPTH) {
                std::cout << "Max AVL depth reached! Cannot insert " << value << ".\n";
                return node;  // Return the existing node, not nullptr
            }

            node->left = insertRecursive(node->left, value, depth + 1);
        } else if (value > node->value) {
            if (depth >= MAX_AVL_DEPTH) {
                std::cout << "Max AVL depth reached! Cannot insert " << value << ".\n";
                return node;  // Return the existing node, not nullptr
            }
            node->right = insertRecursive(node->right, value, depth + 1);
        } else
            return node;

        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);
        node->updateBalanceFactor(balance);

        if (balance > 1 && value < node->left->value)
            return rightRotate(node);
        if (balance < -1 && value > node->right->value)
            return leftRotate(node);
        if (balance > 1 && value > node->left->value) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && value < node->right->value) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void insert(int value) {
        root = insertRecursive(root, value);
    }

    void calculatePositions(AVLNode* node, float x, float y, float hSpacing, std::unordered_map<AVLNode*, sf::Vector2f>& positions) {
        if (!node) return;
        positions[node] = sf::Vector2f(x, y);
        calculatePositions(node->left, x - hSpacing, y + 120, hSpacing / 2, positions);
        calculatePositions(node->right, x + hSpacing, y + 120, hSpacing / 2, positions);
    }

    void draw(sf::RenderWindow& window) {
        if (!root) return;

        std::unordered_map<AVLNode*, sf::Vector2f> positions;
        calculatePositions(root, 400, 170, 100, positions);

        std::vector<std::pair<AVLNode*, AVLNode*>> edges;
        std::queue<AVLNode*> q;
        q.push(root);
        while (!q.empty()) {
            AVLNode* current = q.front();
            q.pop();
            if (current->left) {
                edges.emplace_back(current, current->left);
                q.push(current->left);
            }
            if (current->right) {
                edges.emplace_back(current, current->right);
                q.push(current->right);
            }
        }

        for (auto& edge : edges) {
            sf::Vector2f parentPos = positions[edge.first];
            sf::Vector2f childPos = positions[edge.second];
            parentPos.x += 20;
            parentPos.y += 40;
            childPos.x += 20;
            sf::Vertex line[] = { sf::Vertex(parentPos), sf::Vertex(childPos) };
            window.draw(line, 2, sf::Lines);
        }

        for (auto& node : nodes) {
            if (positions.find(node) != positions.end()) {
                node->setPosition(positions[node]);
                node->draw(window);
            }
        }
    }

    void reset() {
        for (auto node : nodes) delete node;
        nodes.clear();
        root = nullptr;
    }

    void search(int value) {
        AVLNode* current = root;
        while (current) {
            if (current->value == value) {
                current->circle.setFillColor(sf::Color::Yellow); // Highlight the found node
                return;
            } else if (value < current->value) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        std::cout << "Number does not exist." << std::endl;
    }
};

class GraphNode {
public:
    int value;
    sf::CircleShape circle;
    sf::Text text;
    sf::Vector2f position;

    GraphNode(int val, const sf::Font& font) : value(val) {
        circle.setRadius(20);
        circle.setFillColor(sf::Color::Yellow);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);

        text.setFont(font);
        text.setString(std::to_string(val));
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);
    }

    void setPosition(sf::Vector2f pos) {
        position = pos;
        circle.setPosition(pos);
        text.setPosition(
            pos.x + (circle.getRadius() - text.getLocalBounds().width) / 2,
            pos.y + (circle.getRadius() - text.getLocalBounds().height) / 2 - 5
        );
    }

    void draw(sf::RenderWindow& window) {
        window.draw(circle);
        window.draw(text);
    }
};

class GraphVisualizer {
public:
    std::vector<GraphNode*> nodes;
    std::vector<std::pair<GraphNode*, GraphNode*>> edges;
    sf::Font font;

    GraphVisualizer() {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
        }
    }

    void addNode(int value) {
        GraphNode* newNode = new GraphNode(value, font);
        nodes.push_back(newNode);
    }

    void addEdge(int start, int end) {
        GraphNode* startNode = nullptr;
        GraphNode* endNode = nullptr;

        for (auto& node : nodes) {
            if (node->value == start) startNode = node;
            if (node->value == end) endNode = node;
        }

        if (startNode && endNode) {
            edges.push_back({startNode, endNode});
        }
    }

    void calculatePositions() {
        float angle = 2 * M_PI / nodes.size();
        float radius = 200;
        sf::Vector2f center(400, 300);

        for (size_t i = 0; i < nodes.size(); ++i) {
            float x = center.x + radius * cos(angle * i);
            float y = center.y + radius * sin(angle * i);
            nodes[i]->setPosition(sf::Vector2f(x, y));
        }
    }

    void draw(sf::RenderWindow& window) {
        calculatePositions();

        for (auto& edge : edges) {
            sf::Vector2f startPos = edge.first->circle.getPosition();
            sf::Vector2f endPos = edge.second->circle.getPosition();
            startPos.x += 20;
            startPos.y += 20;
            endPos.x += 20;
            endPos.y += 20;
            sf::Vertex line[] = { sf::Vertex(startPos), sf::Vertex(endPos) };
            window.draw(line, 2, sf::Lines);
        }

        for (auto& node : nodes) {
            node->draw(window);
        }
    }

    void reset() {
        for (auto node : nodes) delete node;
        nodes.clear();
        edges.clear();
    }

    void search(int value) {
        for (auto& node : nodes) {
            if (node->value == value) {
                node->circle.setFillColor(sf::Color::Yellow); // Highlight the found node
                return;
            }
        }
        std::cout << "Number does not exist." << std::endl;
    }
};

class Visualizer {
public:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture titleTexture;
    sf::Sprite titleSprite;

    std::vector<Button> buttons;
    std::vector<Button> treeButtons;
    Button backButton;
    Button visualizeButton;
    Button resetButton;
    Button addNodeButton;
    Button addEdgeButton;
    Button searchButton; // Add the Search button
    bool showMainPage = true;
    bool showTreePage = false;
    bool showBSTVisualization = false;
    bool showAVLVisualization = false;
    bool showTrieVisualization = false;
    bool showGraphVisualization = false;
    std::string currentAlgorithm = "";

    BSTVisualizer bstVisualizer;
    AVLVisualizer avlVisualizer;
    QueueVisualizer queueVisualizer;
    StackVisualizer stackVisualizer;
    TrieVisualizer trieVisualizer;
    GraphVisualizer graphVisualizer;

    sf::RectangleShape inputBox;
    sf::Text inputText;
    sf::Text instructionsText;
    std::string userInput;
    bool isInputActive = false;
    bool isScrolling = false;

    // Blinking cursor variables
    bool cursorVisible;
    sf::Clock cursorClock;
    sf::Time cursorBlinkInterval;

    struct DataStructureInfo {
        std::string description, bestComplexity, averageComplexity, advantages, disadvantages;
    };

    std::unordered_map<std::string, DataStructureInfo> dataStructures = {
        {"Stacks", {"LIFO data structure.", "O(1)", "O(1)", "Efficient for undo operations.", "No random access."}},
        {"Queues", {"FIFO data structure.", "O(1)", "O(1)", "Used in scheduling.", "No reverse traversal."}},
        {"Graphs", {"Nodes connected by edges.", "Depends", "Depends", "Models networks.", "Complex to implement."}},
        {"Trees", {"", "", "", "", ""}},
        {"Algorithms", {"", "", "", "", ""}}
    };

    Visualizer() :
        window(sf::VideoMode(800, 600), "Data Structure Visualizer", sf::Style::Default),
        backButton("Back", font, sf::Vector2f(90, 50), sf::Vector2f(5, 5), sf::Color(50, 50, 50)),
        visualizeButton("THANK YOU", font, sf::Vector2f(150, 50), sf::Vector2f(325, 500), sf::Color(100, 200, 100)),
        resetButton("Reset", font, sf::Vector2f(100, 50), sf::Vector2f(500, 500), sf::Color(160, 160, 160)),
        addNodeButton("Add Node", font, sf::Vector2f(100, 50), sf::Vector2f(100, 500), sf::Color(120, 120, 120)),
        addEdgeButton("Add Edge", font, sf::Vector2f(100, 50), sf::Vector2f(300, 500), sf::Color(160, 160, 160)),
        searchButton("Search", font, sf::Vector2f(100, 50), sf::Vector2f(650, 500), sf::Color(160, 160, 160)), // Initialize the Search button
        cursorVisible(true), cursorBlinkInterval(sf::seconds(0.5)) // Initialize cursor variables
    {
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
            return;
        }

        sf::Image icon;
        if (!icon.loadFromFile("logo.png")) {  // Change to the path of your icon
            std::cerr << "Error loading icon!" << std::endl;
        } else {
            window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }

        if (!backgroundTexture.loadFromFile("background.png")) {
            std::cerr << "Error loading background image!" << std::endl;
        } else {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setColor(sf::Color(255, 255, 255, 80)); // Set 50% opacity

            backgroundSprite.setScale(
                window.getSize().x / static_cast<float>(backgroundTexture.getSize().x),
                window.getSize().y / static_cast<float>(backgroundTexture.getSize().y)
            );
        }

        if (!titleTexture.loadFromFile("WORDART2.png")) {  // Replace with your image path
            std::cerr << "Error loading Word Art image!" << std::endl;
        } else {
            titleSprite.setTexture(titleTexture);
            titleSprite.setPosition(90, 90);  // Adjust position
            titleSprite.setScale(0.9f, 0.9f); // Adjust size if needed
        }

        backButton.text.setPosition(
            backButton.shape.getPosition().x + 25,  // Move right
            backButton.shape.getPosition().y + 15    // Move down
        );

        resetButton.text.setPosition(
            resetButton.shape.getPosition().x + 27,  // Move right
            resetButton.shape.getPosition().y + 15    // Move down
        );

        searchButton.text.setPosition(
            searchButton.shape.getPosition().x + 23,  // Move right
            searchButton.shape.getPosition().y + 15    // Move down
        );

        addNodeButton.text.setPosition(
            addNodeButton.shape.getPosition().x + 12,  // Move right
            addNodeButton.shape.getPosition().y + 13    // Move down
        );

         visualizeButton.text.setPosition(
            visualizeButton.shape.getPosition().x + 20,  // Move right
            visualizeButton.shape.getPosition().y + 13    // Move down
        );



        addEdgeButton.text.setPosition(
            addEdgeButton.shape.getPosition().x + 12,  // Move right
            addEdgeButton.shape.getPosition().y + 13    // Move down
        );

        std::vector<std::string> algorithms = {"Stacks", "Queues", "Trees", "Graphs", "About"};
        float y = 200;
        for (const auto& algo : algorithms) {
            buttons.push_back(Button(algo, font, sf::Vector2f(200, 50), sf::Vector2f(300, y), sf::Color(150, 150, 150)));
            y += 70;
        }

        std::vector<std::string> treeTypes = {"BST", "AVL", "Trie"};
        y = 200;
        for (const auto& treeType : treeTypes) {
            treeButtons.push_back(Button(treeType, font, sf::Vector2f(200, 50), sf::Vector2f(300, y), sf::Color(150, 150, 150)));
            y += 70;
        }

        inputBox.setSize(sf::Vector2f(400, 50));
        inputBox.setPosition(200, 100);
        inputBox.setFillColor(sf::Color::White);
        inputBox.setOutlineThickness(2);
        inputBox.setOutlineColor(sf::Color::Black);

        inputText.setFont(font);
        inputText.setCharacterSize(24);
        inputText.setFillColor(sf::Color::Black);
        inputText.setPosition(210, 110);

        instructionsText.setFont(font);
        instructionsText.setCharacterSize(18);
        instructionsText.setFillColor(sf::Color::White);
        instructionsText.setPosition(130, 160);
        instructionsText.setString("       Enter node values separated by spaces and click 'Add Node'.\nEnter start and end node values separated by a space and click 'Add Edge'.");
    }

    void resetInputBox() {
        userInput.clear();
        inputText.setString("");
        isInputActive = false;
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

                backgroundSprite.setScale(
                    event.size.width / static_cast<float>(backgroundTexture.getSize().x),
                    event.size.height / static_cast<float>(backgroundTexture.getSize().y)
                );

                // Adjust positions and sizes of UI elements based on the new window size
                float scaleX = event.size.width / 800.0f;
                float scaleY = event.size.height / 600.0f;

                titleText.setCharacterSize(static_cast<unsigned int>(36 * std::min(scaleX, scaleY)));
                titleText.setPosition(90 * scaleX, 50 * scaleY);

                for (auto& button : buttons) {
                    button.shape.setSize(sf::Vector2f(button.shape.getSize().x * scaleX, button.shape.getSize().y * scaleY));
                    button.shape.setPosition(sf::Vector2f(button.shape.getPosition().x * scaleX, button.shape.getPosition().y * scaleY));
                    button.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                    button.text.setPosition(sf::Vector2f(button.text.getPosition().x * scaleX, button.text.getPosition().y * scaleY));
                }

                for (auto& button : treeButtons) {
                    button.shape.setSize(sf::Vector2f(button.shape.getSize().x * scaleX, button.shape.getSize().y * scaleY));
                    button.shape.setPosition(sf::Vector2f(button.shape.getPosition().x * scaleX, button.shape.getPosition().y * scaleY));
                    button.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                    button.text.setPosition(sf::Vector2f(button.text.getPosition().x * scaleX, button.text.getPosition().y * scaleY));
                }

                backButton.shape.setSize(sf::Vector2f(backButton.shape.getSize().x * scaleX, backButton.shape.getSize().y * scaleY));
                backButton.shape.setPosition(sf::Vector2f(backButton.shape.getPosition().x * scaleX, backButton.shape.getPosition().y * scaleY));
                backButton.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                backButton.text.setPosition(sf::Vector2f(backButton.text.getPosition().x * scaleX, backButton.text.getPosition().y * scaleY));

                visualizeButton.shape.setSize(sf::Vector2f(visualizeButton.shape.getSize().x * scaleX, visualizeButton.shape.getSize().y * scaleY));
                visualizeButton.shape.setPosition(sf::Vector2f(visualizeButton.shape.getPosition().x * scaleX, visualizeButton.shape.getPosition().y * scaleY));
                visualizeButton.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                visualizeButton.text.setPosition(sf::Vector2f(visualizeButton.text.getPosition().x * scaleX, visualizeButton.text.getPosition().y * scaleY));

                resetButton.shape.setSize(sf::Vector2f(resetButton.shape.getSize().x * scaleX, resetButton.shape.getSize().y * scaleY));
                resetButton.shape.setPosition(sf::Vector2f(resetButton.shape.getPosition().x * scaleX, resetButton.shape.getPosition().y * scaleY));
                resetButton.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                resetButton.text.setPosition(sf::Vector2f(resetButton.text.getPosition().x * scaleX, resetButton.text.getPosition().y * scaleY));

                addNodeButton.shape.setSize(sf::Vector2f(addNodeButton.shape.getSize().x * scaleX, addNodeButton.shape.getSize().y * scaleY));
                addNodeButton.shape.setPosition(sf::Vector2f(addNodeButton.shape.getPosition().x * scaleX, addNodeButton.shape.getPosition().y * scaleY));
                addNodeButton.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                addNodeButton.text.setPosition(sf::Vector2f(addNodeButton.text.getPosition().x * scaleX, addNodeButton.text.getPosition().y * scaleY));

                addEdgeButton.shape.setSize(sf::Vector2f(addEdgeButton.shape.getSize().x * scaleX, addEdgeButton.shape.getSize().y * scaleY));
                addEdgeButton.shape.setPosition(sf::Vector2f(addEdgeButton.shape.getPosition().x * scaleX, addEdgeButton.shape.getPosition().y * scaleY));
                addEdgeButton.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                addEdgeButton.text.setPosition(sf::Vector2f(addEdgeButton.text.getPosition().x * scaleX, addEdgeButton.text.getPosition().y * scaleY));

                searchButton.shape.setSize(sf::Vector2f(searchButton.shape.getSize().x * scaleX, searchButton.shape.getSize().y * scaleY));
                searchButton.shape.setPosition(sf::Vector2f(searchButton.shape.getPosition().x * scaleX, searchButton.shape.getPosition().y * scaleY));
                searchButton.text.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                searchButton.text.setPosition(sf::Vector2f(searchButton.text.getPosition().x * scaleX, searchButton.text.getPosition().y * scaleY));

                inputBox.setSize(sf::Vector2f(inputBox.getSize().x * scaleX, inputBox.getSize().y * scaleY));
                inputBox.setPosition(sf::Vector2f(inputBox.getPosition().x * scaleX, inputBox.getPosition().y * scaleY));
                inputBox.setOutlineThickness(static_cast<int>(2 * std::min(scaleX, scaleY)));

                inputText.setCharacterSize(static_cast<unsigned int>(24 * std::min(scaleX, scaleY)));
                inputText.setPosition(sf::Vector2f(inputText.getPosition().x * scaleX, inputText.getPosition().y * scaleY));

                instructionsText.setCharacterSize(static_cast<unsigned int>(18 * std::min(scaleX, scaleY)));
                instructionsText.setPosition(sf::Vector2f(instructionsText.getPosition().x * scaleX, instructionsText.getPosition().y * scaleY));

                // Update scrollbar position and size for Trie visualization
                if (showTrieVisualization) {
                    trieVisualizer.scrollbar.setSize(sf::Vector2f(20 * scaleX, 400 * scaleY));
                    trieVisualizer.scrollbar.setPosition(sf::Vector2f(760 * scaleX, 200 * scaleY));
                    trieVisualizer.scrollbarHandle.setSize(sf::Vector2f(20 * scaleX, trieVisualizer.scrollbarHandle.getSize().y * scaleY));
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                if (showMainPage) {
                    for (auto& button : buttons) {
                        if (button.isClicked(mousePos)) {
                            currentAlgorithm = button.text.getString();
                            if (currentAlgorithm == "Trees") {
                                showTreePage = true;
                                showMainPage = false;
                            } else if (currentAlgorithm == "Queues") {
                                showMainPage = false;
                                queueVisualizer.reset();
                            } else if (currentAlgorithm == "Stacks") {
                                showMainPage = false;
                                stackVisualizer.reset();
                            } else if (currentAlgorithm == "Graphs") {
                                showGraphVisualization = true;
                                showMainPage = false;
                                graphVisualizer.reset();
                            } else if (currentAlgorithm == "About") {
    showMainPage = false;
    showGraphVisualization = false;}


                            else {
                                showMainPage = false;
                            }
                            resetInputBox(); // Reset input box when transitioning to a new page
                            break;
                        }
                    }
                } else if (showTreePage) {
                    if (backButton.isClicked(mousePos)) {
                        showTreePage = false;
                        showMainPage = true;
                        resetInputBox(); // Reset input box when going back to the main page
                    } else {
                        for (auto& button : treeButtons) {
                            if (button.isClicked(mousePos)) {
                                currentAlgorithm = button.text.getString();
                                if (currentAlgorithm == "BST") {
                                    showBSTVisualization = true;
                                    showTreePage = false;
                                    bstVisualizer.reset();
                                } else if (currentAlgorithm == "AVL") {
                                    showAVLVisualization = true;
                                    showTreePage = false;
                                    avlVisualizer.reset();
                                } else if (currentAlgorithm == "Trie") {
                                    showTrieVisualization = true;
                                    showTreePage = false;
                                    trieVisualizer.reset();
                                }
                                resetInputBox(); // Reset input box when transitioning to a new visualization
                                break;
                            }
                        }
                    }
                } else if (showBSTVisualization || showAVLVisualization || showTrieVisualization || currentAlgorithm == "Queues" || currentAlgorithm == "Stacks" || currentAlgorithm == "About" || showGraphVisualization) {
                    if (backButton.isClicked(mousePos)) {
                        if (showBSTVisualization || showAVLVisualization || showTrieVisualization) {
                            showBSTVisualization = false;
                            showAVLVisualization = false;
                            showTrieVisualization = false;
                            showTreePage = true;
                            showMainPage = false;
                            cursorVisible = false;

                            bstVisualizer.reset();
                            avlVisualizer.reset();
                            trieVisualizer.reset();
                            queueVisualizer.reset();
                            stackVisualizer.reset();
                        } else {
                            showBSTVisualization = false;
                            showAVLVisualization = false;
                            showTrieVisualization = false;
                            showTreePage = false;
                            showMainPage = true;
                            bstVisualizer.reset();
                            avlVisualizer.reset();
                            trieVisualizer.reset();
                            queueVisualizer.reset();
                            stackVisualizer.reset();
                            graphVisualizer.reset();
                        }
                        resetInputBox(); // Reset input box when going back to the main page
                    } else if (inputBox.getGlobalBounds().contains(mousePos)) {
                        isInputActive = true;
                    } else if (resetButton.isClicked(mousePos)) {
                        if (showBSTVisualization) bstVisualizer.reset();
                        if (showAVLVisualization) avlVisualizer.reset();
                        if (showTrieVisualization) trieVisualizer.reset();
                        if (currentAlgorithm == "Queues") queueVisualizer.reset();
                        if (currentAlgorithm == "Stacks") stackVisualizer.reset();
                        if (showGraphVisualization) graphVisualizer.reset();
                        resetInputBox(); // Reset input box when resetting the visualization
                    } else if (addNodeButton.isClicked(mousePos) && showGraphVisualization) {
                        std::stringstream ss(userInput);
                        int value;
                        while (ss >> value) graphVisualizer.addNode(value);
                        userInput.clear();
                        inputText.setString("");
                    } else if (addEdgeButton.isClicked(mousePos) && showGraphVisualization) {
                        std::stringstream ss(userInput);
                        int start, end;
                        while (ss >> start >> end) graphVisualizer.addEdge(start, end);
                        userInput.clear();
                        inputText.setString("");
                    } else if (searchButton.isClicked(mousePos)) { // Handle Search button click
                        if (showBSTVisualization) {
                            int value;
                            std::stringstream ss(userInput);
                            if (ss >> value) {
                                bstVisualizer.search(value);
                            }
                        } else if (showAVLVisualization) {
                            int value;
                            std::stringstream ss(userInput);
                            if (ss >> value) {
                                avlVisualizer.search(value);
                            }
                        } else if (showTrieVisualization) {
                            std::string word;
                            std::stringstream ss(userInput);
                            if (ss >> word) {
                                trieVisualizer.search(word);
                            }
                        } else if (currentAlgorithm == "Queues") {
                            int value;
                            std::stringstream ss(userInput);
                            if (ss >> value) {
                                queueVisualizer.search(value);
                            }
                        } else if (currentAlgorithm == "Stacks") {
                            int value;
                            std::stringstream ss(userInput);
                            if (ss >> value) {
                                stackVisualizer.search(value);
                            }
                        } else if (showGraphVisualization) {
                            int value;
                            std::stringstream ss(userInput);
                            if (ss >> value) {
                                graphVisualizer.search(value);
                            }
                        }
                        userInput.clear();
                        inputText.setString("");
                    } else {
                        isInputActive = false;
                    }
                }

                if (showTrieVisualization) {
                    trieVisualizer.handleScrollbar(mousePos);
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});

                if (showMainPage) {
                    for (auto& button : buttons) {
                        button.isHovered = button.isClicked(mousePos);
                    }
                } else if (showTreePage) {
                    backButton.isHovered = backButton.isClicked(mousePos);
                    for (auto& button : treeButtons) {
                        button.isHovered = button.isClicked(mousePos);
                    }
                } else if (showBSTVisualization || showAVLVisualization || showTrieVisualization || currentAlgorithm == "Queues" || currentAlgorithm == "Stacks" || showGraphVisualization) {
                    backButton.isHovered = backButton.isClicked(mousePos);
                    resetButton.isHovered = resetButton.isClicked(mousePos);
                    if (showGraphVisualization) {
                        addNodeButton.isHovered = addNodeButton.isClicked(mousePos);
                        addEdgeButton.isHovered = addEdgeButton.isClicked(mousePos);
                    }
                    searchButton.isHovered = searchButton.isClicked(mousePos);
                }
            }

            if (event.type == sf::Event::TextEntered && isInputActive) {
                if (event.text.unicode == '\b' && !userInput.empty()) {
                    userInput.pop_back();
                } else if (event.text.unicode >= '0' && event.text.unicode <= '9' || event.text.unicode == ' ' ||
                           (event.text.unicode >= 'a' && event.text.unicode <= 'z') ||
                           (event.text.unicode >= 'A' && event.text.unicode <= 'Z')) {
                    userInput += static_cast<char>(event.text.unicode);
                }
                inputText.setString(userInput);
            }

            if (event.type == sf::Event::KeyPressed && isInputActive) {
                if (event.key.code == sf::Keyboard::Enter) {
                    std::stringstream ss(userInput);
                    std::string word;
                    if (showBSTVisualization) {
                        int value;
                        while (ss >> value) bstVisualizer.insert(value);
                        bstVisualizer.isAnimating = true;
                    } else if (showAVLVisualization) {
                        int value;
                        while (ss >> value) avlVisualizer.insert(value);
                    } else if (showTrieVisualization) {
                        while (ss >> word) trieVisualizer.insert(word);
                    } else if (currentAlgorithm == "Queues") {
                        int value;
                        while (ss >> value) queueVisualizer.enqueue(value);
                    } else if (currentAlgorithm == "Stacks") {
                        int value;
                        while (ss >> value) stackVisualizer.push(value);
                    }
                    userInput.clear();
                    inputText.setString("");
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled && showTrieVisualization) {
                if (event.mouseWheelScroll.delta > 0) {
                    // Scroll up
                    trieVisualizer.scrollOffset = std::max(0.f, trieVisualizer.scrollOffset - 20.f);
                } else if (event.mouseWheelScroll.delta < 0) {
                    // Scroll down
                    trieVisualizer.scrollOffset = std::min(trieVisualizer.maxScrollOffset, trieVisualizer.scrollOffset + 20.f);
                }
                trieVisualizer.updateScrollbar();
            }
        }
    }

    void update() {
        if (bstVisualizer.isAnimating) {
            bstVisualizer.updateAnimation();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        if (showTrieVisualization) {
            trieVisualizer.updateScrollbar();
        }

        // Update cursor visibility
        if (cursorClock.getElapsedTime() >= cursorBlinkInterval && isInputActive) {
            cursorVisible = !cursorVisible;
            cursorClock.restart();
        }
    }

    void render() {
        window.clear(sf::Color(50, 50, 50));

        window.draw(backgroundSprite);

        if (showMainPage) {
            window.draw(titleSprite);
            for (auto& button : buttons) button.draw(window);
        } else if (showTreePage) {
            sf::Text treeTitle;
            treeTitle.setFont(font);
            treeTitle.setString("Tree Types");
            treeTitle.setCharacterSize(30);
            treeTitle.setFillColor(sf::Color::White);
            treeTitle.setPosition(325, 60);
            window.draw(treeTitle);

            for (auto& button : treeButtons) button.draw(window);
            backButton.draw(window);
        } else if (showBSTVisualization) {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString("BST Visualization");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            window.draw(inputBox);
            window.draw(inputText);

            bstVisualizer.draw(window);

            backButton.draw(window);
            resetButton.draw(window);
            searchButton.draw(window); // Draw the Search button
        } else if (showAVLVisualization) {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString("AVL Visualization");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            window.draw(inputBox);
            window.draw(inputText);

            avlVisualizer.draw(window);

            backButton.draw(window);
            resetButton.draw(window);
            searchButton.draw(window); // Draw the Search button
        } else if (showTrieVisualization) {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString("Trie Visualization");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            window.draw(inputBox);
            window.draw(inputText);

            trieVisualizer.draw(window);

            backButton.draw(window);
            resetButton.draw(window);
            searchButton.draw(window); // Draw the Search button
        } else if (currentAlgorithm == "Queues") {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString("Queue Visualization");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            window.draw(inputBox);
            window.draw(inputText);

            queueVisualizer.draw(window);

            backButton.draw(window);
            resetButton.draw(window);
            searchButton.draw(window); // Draw the Search button
        } else if (currentAlgorithm == "Stacks") {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString("Stack Visualization");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            window.draw(inputBox);
            window.draw(inputText);

            stackVisualizer.draw(window);

            backButton.draw(window);
            resetButton.draw(window);
            searchButton.draw(window); // Draw the Search button
        } else if (showGraphVisualization) {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString("Graph Visualization");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            window.draw(inputBox);
            window.draw(inputText);
            window.draw(instructionsText);

            addNodeButton.draw(window);
            addEdgeButton.draw(window);
            searchButton.draw(window); // Draw the Search button

            graphVisualizer.draw(window);

            backButton.draw(window);
            resetButton.draw(window);
        } else {
            sf::Text algorithmTitle;
            algorithmTitle.setFont(font);
            algorithmTitle.setString(currentAlgorithm + " The Maker");
            algorithmTitle.setCharacterSize(30);
            algorithmTitle.setFillColor(sf::Color::White);
            algorithmTitle.setPosition(280, 50);
            window.draw(algorithmTitle);

            auto& info = dataStructures[currentAlgorithm];
            sf::Text descriptionText;
            descriptionText.setFont(font);
            descriptionText.setCharacterSize(18);
            descriptionText.setFillColor(sf::Color::White);

            std::vector<std::string> texts = {"Name: " + info.description, "Shifan Abrar Rashid" + info.bestComplexity,
                                              "Education: " + info.averageComplexity, "Bachelor of Science in Computer Science at the University of Cincinnati" + info.advantages,
                                              "Made with C++, SFML 2.6, IDE:CODEBLOCKS" + info.disadvantages};
            float y = 150;
            for (const auto& line : texts) {
                descriptionText.setString(line);
                descriptionText.setPosition(50, y);
                window.draw(descriptionText);
                y += 50;
            }

            backButton.draw(window);
            visualizeButton.draw(window);
        }

        if (isScrolling) {
            inputBox.setFillColor(sf::Color(255, 255, 255, 128));
            resetButton.setAlpha(128);
        } else {
            inputBox.setFillColor(sf::Color(255, 255, 255, 255));
            resetButton.setAlpha(255);
        }

        // Draw the cursor if visible and input box is active
        if (cursorVisible && isInputActive) {
            sf::RectangleShape cursor(sf::Vector2f(2, inputText.getCharacterSize()));
            cursor.setFillColor(sf::Color::Black);
            cursor.setPosition(inputText.getPosition().x + inputText.getLocalBounds().width, inputText.getPosition().y);
            window.draw(cursor);
        }

        window.display();
    }
};

int main() {
    Visualizer visualizer;
    visualizer.run();
    return 0;
}
