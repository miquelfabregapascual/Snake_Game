#include "Snake.hpp"
#include "StringHelpers.hpp"
#include <SFML/Graphics.hpp>
#include <deque> 

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
    : mWindow(sf::VideoMode(640, 480), "Snake Game", sf::Style::Close)
    , mTexture()
    , mPlayer()
    , mApple()
    , mAppleTexture()
    , mFont()
    , mStatisticsText()
    , mStatisticsUpdateTime()
    , mStatisticsNumFrames(0)
    , mIsMovingUp(false)
    , mIsMovingDown(false)
    , mIsMovingRight(false)
    , mIsMovingLeft(false)
    , mCurrentDirection(1.f, 0.f) // Direcció inicial: dreta
    , mSnakeLength(1) //Longitud inicial de la serp: 1
    , mScore(0) // Inicialitzar la puntuació a zero
  

{
    if (!mTexture.loadFromFile("Media/Textures/Snake.png") ||
        !mAppleTexture.loadFromFile("Media/Textures/apple.png")) {
        // Gestiona l'error de càrrega
    }

    // posició pome random
    sf::Vector2u windowSize = mWindow.getSize();
    sf::Vector2f randomPosition(
        static_cast<float>(std::rand() % windowSize.x),
        static_cast<float>(std::rand() % windowSize.y)
    );
    mApple.setTexture(mAppleTexture);
    mApple.setScale(0.07f, 0.07f); //Reduir tamany
    mApple.setPosition(randomPosition);

    mTexture.setSmooth(true);
    mPlayer.setTexture(mTexture);
    mPlayer.setScale(0.01f, 0.01f); // reduir tamany 
    mPlayer.setPosition(350.f, 250.f); // psició inicial

    mFont.loadFromFile("Media/Sansation.ttf");
    mStatisticsText.setFont(mFont);
    mStatisticsText.setPosition(5.f, 5.f);
    mStatisticsText.setCharacterSize(10);
    mPlayer.setTexture(mTexture);
    mPlayer.setPosition(100.f, 100.f);
    mPlayer.setOrigin(16.f, 16.f); // Origen al centre del personatge
}

void Game::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen()) {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;

            processEvents();
            update(TimePerFrame);
        }

        updateStatistics(elapsedTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::KeyPressed:
            handlePlayerInput(event.key.code, true);
            break;

        case sf::Event::KeyReleased:
            handlePlayerInput(event.key.code, false);
            break;

        case sf::Event::Closed:
            mWindow.close();
            break;
        }
    }
}





void Game::update(sf::Time elapsedTime) {
    sf::Vector2f movement(0.f, 0.f);
    if (mIsMovingUp && mCurrentDirection != sf::Vector2f(0.f, 1.f)) {
        mCurrentDirection = sf::Vector2f(0.f, -1.f); // gira 90 graus amunt
    }
    if (mIsMovingDown && mCurrentDirection != sf::Vector2f(0.f, -1.f)) {
        mCurrentDirection = sf::Vector2f(0.f, 1.f); // gira 90 graus abaix
    }
    if (mIsMovingLeft && mCurrentDirection != sf::Vector2f(1.f, 0.f)) {
        mCurrentDirection = sf::Vector2f(-1.f, 0.f); // gira 90 graus a l'esquerra
    }
    if (mIsMovingRight && mCurrentDirection != sf::Vector2f(-1.f, 0.f)) {
        mCurrentDirection = sf::Vector2f(1.f, 0.f); // gira 90 graus a la dreta
    }

    movement = PlayerSpeed * mCurrentDirection;

    sf::Vector2f newPosition = mPlayer.getPosition() + (movement * elapsedTime.asSeconds());

    // limits del mapa
    float minX = 0.f;
    float minY = 0.f;
    float maxX = static_cast<float>(mWindow.getSize().x) - mPlayer.getGlobalBounds().width;
    float maxY = static_cast<float>(mWindow.getSize().y) - mPlayer.getGlobalBounds().height;

    // El jugador no pot moure's fora del mapa
    newPosition.x = std::max(minX, std::min(newPosition.x, maxX));
    newPosition.y = std::max(minY, std::min(newPosition.y, maxY));

    // Comprova si el jugador ha sortit del mapa
    if (newPosition.x == minX || newPosition.x == maxX || newPosition.y == minY || newPosition.y == maxY) {
        // Restablir la posició del jugador si està fora del mapa
        mPlayer.setPosition(100.f, 100.f);
        mCurrentDirection = sf::Vector2f(1.f, 0.f); // direcció inicial dreta
        mSnakeSegments.clear(); // Netegeu els segments del cos de la serp
    }
    else {
        // Comproveu si hi ha col·lisió amb el cos de la serp
        for (const sf::Vector2f& segmentPos : mSnakeSegments) {
            if (newPosition == segmentPos) {
                // Col·lisió amb el segment del cos, reinicia el joc
                mPlayer.setPosition(100.f, 100.f);
                mCurrentDirection = sf::Vector2f(1.f, 0.f); // Direcció inicial: dreta
                mSnakeSegments.clear(); //Netegeu els segments del cos de la serp
                return; //Sortiu de la funció per evitar més actualitzacions
            }
        }

        // actualitzar els segments del cos de la serp
        mSnakeSegments.push_front(mPlayer.getPosition());
        while (mSnakeSegments.size() > mSnakeLength) {
            mSnakeSegments.pop_back();
        }

        // Aplica la nova posició de jugador
        mPlayer.setPosition(newPosition);
    }

    //Comproveu si hi ha col·lisió amb la poma
    if (mPlayer.getGlobalBounds().intersects(mApple.getGlobalBounds())) {
        // Torneu a col·locar la poma en un lloc diferent
        sf::Vector2u windowSize = mWindow.getSize();
        sf::Vector2f randomPosition(
            static_cast<float>(std::rand() % windowSize.x),
            static_cast<float>(std::rand() % windowSize.y)
        );
        mApple.setPosition(randomPosition);

        //Augmenta la longitud de la serp
        mSnakeLength++;

        // Actualitzar la puntuació
        mScore += 1; // puntuació donada per pome 
    }

}


void Game::render() {
    mWindow.clear();

    mWindow.draw(mApple); 

    // Dibuixa els segments del cos de la serp
    for (const sf::Vector2f& segmentPos : mSnakeSegments) {
        sf::Sprite snakeSegment = mPlayer;
        snakeSegment.setPosition(segmentPos);
        mWindow.draw(snakeSegment);
    }

    // Dibuixa el cap de la serp
    mWindow.draw(mPlayer);

    // Mostra la puntuació a la pantalla
    sf::Text scoreText;
    scoreText.setFont(mFont);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.f, 10.f);
    scoreText.setString("Score: " + std::to_string(mScore));
    mWindow.draw(scoreText);

    mWindow.draw(mStatisticsText);
    mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime) {
    mStatisticsUpdateTime += elapsedTime;
    mStatisticsNumFrames += 1;

    if (mStatisticsUpdateTime >= sf::seconds(1.0f)) {
        mStatisticsText.setString(
            "Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
            "Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

        mStatisticsUpdateTime -= sf::seconds(1.0f);
        mStatisticsNumFrames = 0;
    }
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed) {
    if (key == sf::Keyboard::W)
        mIsMovingUp = isPressed;
    else if (key == sf::Keyboard::S)
        mIsMovingDown = isPressed;
    else if (key == sf::Keyboard::A)
        mIsMovingLeft = isPressed;
    else if (key == sf::Keyboard::D)
        mIsMovingRight = isPressed;
}
