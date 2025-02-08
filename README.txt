This code is a comprehensive implementation of a data structure visualizer using the SFML library in C++. Below is a detailed documentation of the code, explaining the purpose and functionality of each class and method.

### Includes and Namespaces
- **Headers**: The code includes various headers for standard libraries, SFML graphics, and other utilities.
- **Namespace**: The code uses the `std` namespace for convenience.

### Classes

#### `Button`
- **Purpose**: Represents a clickable button with text.
- **Attributes**:
  - `shape`: The rectangle shape of the button.
  - `text`: The text displayed on the button.
  - `isHovered`: Boolean to check if the button is hovered.
- **Methods**:
  - `Button(const std::string& label, const sf::Font& font, sf::Vector2f size, sf::Vector2f position, sf::Color color)`: Constructor to initialize the button.
  - `bool isClicked(sf::Vector2f mousePos)`: Checks if the button is clicked.
  - `void draw(sf::RenderWindow& window)`: Draws the button on the window.
  - `void setAlpha(int alpha)`: Sets the alpha transparency of the button.

#### `TrieNode`
- **Purpose**: Represents a node in a Trie data structure.
- **Attributes**:
  - `children`: A map of child nodes.
  - `isEnd`: Boolean indicating if the node is the end of a word.
  - `circle`: The graphical representation of the node.
  - `charText`: The character displayed in the node.
  - `position`: The position of the node.
- **Methods**:
  - `TrieNode(sf::Font& font)`: Constructor to initialize the node.
  - `void setPosition(sf::Vector2f pos)`: Sets the position of the node.
  - `void draw(sf::RenderWindow& window)`: Draws the node on the window.

#### `TrieVisualizer`
- **Purpose**: Visualizes a Trie data structure.
- **Attributes**:
  - `root`: The root node of the Trie.
  - `font`: The font used for text.
  - `nodes`: A vector of Trie nodes.
  - `edges`: A vector of edges connecting the nodes.
  - `scrollbar`, `scrollbarHandle`: Scrollbar components.
  - `scrollOffset`, `maxScrollOffset`: Scroll offsets.
  - `isScrolling`: Boolean to check if scrolling is active.
- **Methods**:
  - `TrieVisualizer()`: Constructor to initialize the visualizer.
  - `void insert(const std::string& word)`: Inserts a word into the Trie.
  - `void calculatePositions(TrieNode* node, sf::Vector2f position, float horizontalSpacing, float verticalSpacing)`: Calculates the positions of the nodes.
  - `void draw(sf::RenderWindow& window)`: Draws the Trie on the window.
  - `void reset()`: Resets the Trie.
  - `void handleScrollbar(sf::Vector2f mousePos)`: Handles scrollbar interactions.
  - `void updateScrollbar()`: Updates the scrollbar.
  - `void search(const std::string& word)`: Searches for a word in the Trie.
  - `void searchHelper(TrieNode* node, const std::string& word, int index, bool& found)`: Helper method for searching.

#### `StackVisualizer`
- **Purpose**: Visualizes a Stack data structure.
- **Attributes**:
  - `plates`: A vector of stack plates.
  - `plateTexts`: A vector of texts on the plates.
  - `font`: The font used for text.
  - `scrollbar`, `scrollbarHandle`: Scrollbar components.
  - `scrollOffset`, `maxScrollOffset`: Scroll offsets.
  - `isScrolling`: Boolean to check if scrolling is active.
- **Methods**:
  - `StackVisualizer()`: Constructor to initialize the visualizer.
  - `void push(int value)`: Pushes a value onto the stack.
  - `void pop()`: Pops a value from the stack.
  - `void draw(sf::RenderWindow& window)`: Draws the stack on the window.
  - `void reset()`: Resets the stack.
  - `void handleScrollbar(sf::Vector2f mousePos)`: Handles scrollbar interactions.
  - `void updateScrollbar()`: Updates the scrollbar.
  - `void search(int value)`: Searches for a value in the stack.

#### `QueueNode`
- **Purpose**: Represents a node in a Queue data structure.
- **Attributes**:
  - `value`: The value of the node.
  - `next`: Pointer to the next node.
  - `circle`: The graphical representation of the node.
  - `text`: The text displayed in the node.
  - `arrow`: The arrow connecting to the next node.
- **Methods**:
  - `QueueNode(int val, const sf::Font& font)`: Constructor to initialize the node.
  - `void setPosition(sf::Vector2f position)`: Sets the position of the node.
  - `void draw(sf::RenderWindow& window)`: Draws the node on the window.

#### `QueueVisualizer`
- **Purpose**: Visualizes a Queue data structure.
- **Attributes**:
  - `front`, `rear`: Pointers to the front and rear of the queue.
  - `font`: The font used for text.
  - `nodes`: A vector of queue nodes.
  - `arrows`: A vector of arrows connecting the nodes.
  - `scrollbar`, `scrollbarHandle`: Scrollbar components.
  - `scrollOffset`, `maxScrollOffset`: Scroll offsets.
  - `isScrolling`: Boolean to check if scrolling is active.
- **Methods**:
  - `QueueVisualizer()`: Constructor to initialize the visualizer.
  - `void enqueue(int value)`: Enqueues a value into the queue.
  - `void dequeue()`: Dequeues a value from the queue.
  - `void updatePositions()`: Updates the positions of the nodes.
  - `void draw(sf::RenderWindow& window)`: Draws the queue on the window.
  - `void reset()`: Resets the queue.
  - `void handleScrollbar(sf::Vector2f mousePos)`: Handles scrollbar interactions.
  - `void updateScrollbar()`: Updates the scrollbar.
  - `void search(int value)`: Searches for a value in the queue.

#### `BSTNode`
- **Purpose**: Represents a node in a Binary Search Tree (BST) data structure.
- **Attributes**:
  - `value`: The value of the node.
  - `left`, `right`: Pointers to the left and right children.
  - `circle`: The graphical representation of the node.
  - `text`: The text displayed in the node.
- **Methods**:
  - `BSTNode(int val, const sf::Font& font)`: Constructor to initialize the node.
  - `void setPosition(sf::Vector2f position)`: Sets the position of the node.
  - `void draw(sf::RenderWindow& window)`: Draws the node on the window.

#### `BSTVisualizer`
- **Purpose**: Visualizes a Binary Search Tree (BST) data structure.
- **Attributes**:
  - `root`: The root node of the BST.
  - `font`: The font used for text.
  - `edges`: A vector of edges connecting the nodes.
  - `nodes`: A vector of BST nodes.
  - `scrollbar`, `scrollbarHandle`: Scrollbar components.
  - `scrollOffset`, `maxScrollOffset`: Scroll offsets.
  - `isScrolling`: Boolean to check if scrolling is active.
  - `currentStep`, `isAnimating`: Animation-related attributes.
- **Methods**:
  - `BSTVisualizer()`: Constructor to initialize the visualizer.
  - `void insert(int value)`: Inserts a value into the BST.
  - `BSTNode* insertRecursive(BSTNode* node, int value, int depth)`: Recursively inserts a value into the BST.
  - `void drawTree(sf::RenderWindow& window, BSTNode* node, float x, float y, float horizontalSpacing)`: Draws the BST on the window.
  - `void updateAnimation()`: Updates the animation.
  - `void draw(sf::RenderWindow& window)`: Draws the BST on the window.
  - `void reset()`: Resets the BST.
  - `void handleScrollbar(sf::Vector2f mousePos)`: Handles scrollbar interactions.
  - `void updateScrollbar()`: Updates the scrollbar.
  - `void search(int value)`: Searches for a value in the BST.

#### `AVLNode`
- **Purpose**: Represents a node in an AVL Tree data structure.
- **Attributes**:
  - `value`: The value of the node.
  - `height`: The height of the node.
  - `left`, `right`: Pointers to the left and right children.
  - `circle`: The graphical representation of the node.
  - `valueText`: The text displayed for the value.
  - `balanceFactorText`: The text displayed for the balance factor.
- **Methods**:
  - `AVLNode(int val, const sf::Font& font)`: Constructor to initialize the node.
  - `void setPosition(sf::Vector2f position)`: Sets the position of the node.
  - `void updateBalanceFactor(int bf)`: Updates the balance factor text.
  - `void draw(sf::RenderWindow& window)`: Draws the node on the window.

#### `AVLVisualizer`
- **Purpose**: Visualizes an AVL Tree data structure.
- **Attributes**:
  - `root`: The root node of the AVL Tree.
  - `font`: The font used for text.
  - `nodes`: A vector of AVL nodes.
  - `scrollbar`, `scrollbarHandle`: Scrollbar components.
  - `scrollOffset`, `maxScrollOffset`: Scroll offsets.
  - `isScrolling`: Boolean to check if scrolling is active.
  - `avlInsertionTime`, `estimatedBSTTime`: Time-related attributes.
- **Methods**:
  - `AVLVisualizer()`: Constructor to initialize the visualizer.
  - `int getHeight(AVLNode* node)`: Gets the height of a node.
  - `int getBalance(AVLNode* node)`: Gets the balance factor of a node.
  - `AVLNode* rightRotate(AVLNode* y)`: Performs a right rotation.
  - `AVLNode* leftRotate(AVLNode* x)`: Performs a left rotation.
  - `AVLNode* insertRecursive(AVLNode* node, int value, int depth)`: Recursively inserts a value into the AVL Tree.
  - `void insert(int value)`: Inserts a value into the AVL Tree.
  - `void calculatePositions(AVLNode* node, float x, float y, float hSpacing, std::unordered_map<AVLNode*, sf::Vector2f>& positions)`: Calculates the positions of the nodes.
  - `void draw(sf::RenderWindow& window)`: Draws the AVL Tree on the window.
  - `void reset()`: Resets the AVL Tree.
  - `void handleScrollbar(sf::Vector2f mousePos)`: Handles scrollbar interactions.
  - `void updateScrollbar()`: Updates the scrollbar.
  - `void search(int value)`: Searches for a value in the AVL Tree.

#### `GraphNode`
- **Purpose**: Represents a node in a Graph data structure.
- **Attributes**:
  - `value`: The value of the node.
  - `circle`: The graphical representation of the node.
  - `text`: The text displayed in the node.
  - `position`: The position of the node.
- **Methods**:
  - `GraphNode(int val, const sf::Font& font)`: Constructor to initialize the node.
  - `void setPosition(sf::Vector2f pos)`: Sets the position of the node.
  - `void draw(sf::RenderWindow& window)`: Draws the node on the window.

#### `GraphVisualizer`
- **Purpose**: Visualizes a Graph data structure.
- **Attributes**:
  - `nodes`: A vector of graph nodes.
  - `edges`: A vector of edges connecting the nodes.
  - `font`: The font used for text.
  - `boundary`: The boundary of the graph.
  - `scrollbar`, `scrollbarHandle`: Scrollbar components.
  - `scrollOffset`, `maxScrollOffset`: Scroll offsets.
  - `isScrolling`: Boolean to check if scrolling is active.
- **Methods**:
  - `GraphVisualizer()`: Constructor to initialize the visualizer.
  - `void addNode(int value)`: Adds a node to the graph.
  - `void addEdge(int start, int end)`: Adds an edge to the graph.
  - `void calculatePositions()`: Calculates the positions of the nodes.
  - `void draw(sf::RenderWindow& window)`: Draws the graph on the window.
  - `void reset()`: Resets the graph.
  - `void handleScrollbar(sf::Vector2f mousePos)`: Handles scrollbar interactions.
  - `void updateScrollbar()`: Updates the scrollbar.
  - `void search(int value)`: Searches for a value in the graph.

#### `Visualizer`
- **Purpose**: The main class that manages the visualization of different data structures.
- **Attributes**:
  - `window`: The SFML render window.
  - `font`: The font used for text.
  - `titleText`: The title text.
  - `backgroundTexture`, `backgroundSprite`: Background texture and sprite.
  - `titleTexture`, `titleSprite`: Title texture and sprite.
  - `buttons`: A vector of buttons for the main page.
  - `treeButtons`: A vector of buttons for the tree page.
  - `backButton`, `visualizeButton`, `resetButton`, `addNodeButton`, `addEdgeButton`, `searchButton`: Various buttons for interactions.
  - `showMainPage`, `showTreePage`, `showBSTVisualization`, `showAVLVisualization`, `showTrieVisualization`, `showGraphVisualization`: Boolean flags for different pages.
  - `currentAlgorithm`: The current algorithm being visualized.
  - `bstVisualizer`, `avlVisualizer`, `queueVisualizer`, `stackVisualizer`, `trieVisualizer`, `graphVisualizer`: Visualizers for different data structures.
  - `inputBox`, `inputText`, `instructionsText`: Input box and text components.
  - `userInput`: The user input string.
  - `isInputActive`, `isScrolling`: Boolean flags for input and scrolling.
  - `cursorClock`, `cursorBlinkInterval`, `cursorVisible`: Cursor blinking attributes.
  - `dataStructures`: A map of data structure information.
- **Methods**:
  - `Visualizer()`: Constructor to initialize the visualizer.
  - `void resetInputBox()`: Resets the input box.
  - `void run()`: Runs the visualizer.
  - `void handleEvents()`: Handles SFML events.
  - `void update()`: Updates the visualizer.
  - `void render()`: Renders the visualizer.

### Main Function
- **Purpose**: The entry point of the application.
- **Function**: `int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)`
- **Description**: Initializes the visualizer and runs the main loop.

This documentation provides a comprehensive overview of the code, explaining the purpose and functionality of each class and method.