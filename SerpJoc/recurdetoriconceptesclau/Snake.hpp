#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <deque> 

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time elapsedTime);
    void render();
    void updateStatistics(sf::Time elapsedTime);
    void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

private:
    static const float PlayerSpeed;
    static const sf::Time TimePerFrame;

    sf::RenderWindow mWindow;
    sf::Texture mTexture;
    sf::Texture mAppleTexture;
    sf::Sprite mPlayer;
    sf::Sprite mApple;
    sf::Font mFont;
    sf::Text mStatisticsText;
    sf::Time mStatisticsUpdateTime;

    std::vector<sf::Sprite> mSnakeBody;
    std::deque<sf::Vector2f> mSnakeSegments; 
    std::size_t mStatisticsNumFrames;

    bool mIsMovingUp;
    bool mIsMovingDown;
    bool mIsMovingRight;
    bool mIsMovingLeft;

    sf::Vector2f mCurrentDirection;
    int mSnakeLength;
    int mScore;
  
};

#endif // GAME_HPP
