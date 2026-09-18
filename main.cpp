#include <SFML/Graphics.hpp>
#include <string>

const sf::Keyboard::Key controls[5] = {
    sf::Keyboard::A,   // Player1 UP
    sf::Keyboard::Z,   // Player1 Down
    sf::Keyboard::Up,  // Player2 UP
    sf::Keyboard::Down, // Player2 Down
    sf::Keyboard::Space // 2 Player Mode Toggle
};

//Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const sf::Time time_step = sf::seconds(0.017f); //60 fps
sf::Vector2f ball_velocity;
bool is_player1_serving = true;
const float initial_velocity_x = 100.f; //horizontal velocity
const float initial_velocity_y = 60.f; //vertical velocity
const float velocity_multiplier = 1.1f; //how much the ball will speed up everytime it hits a paddle. Here, 10% every time.
sf::Font font;
sf::Text scoreText;
int p1score = 0;
int p2score = 0;
bool twoPMode = true;

//Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];

void reset() {
    // reset paddle position
    paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - (paddleOffsetWall + paddleSize.x) / 2.f, gameHeight / 2.f);
    // reset Ball Position
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
    ball_velocity = { (is_player1_serving ? initial_velocity_x : -initial_velocity_x), initial_velocity_y };

    scoreText.setString(std::to_string(p1score) + " - " + std::to_string(p2score));
    scoreText.setPosition((gameWidth * .5f) - (scoreText.getLocalBounds().width * .5f), 0);
}

void init() {
    font.loadFromFile("resources/fonts/RobotoSlab-Regular.ttf");
    scoreText.setFont(font);
    scoreText.setCharacterSize(48);
    scoreText.setFillColor(sf::Color::White);

    // Set size and origin of paddles
    for (sf::RectangleShape& p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); //Should be half the ball width and height
    reset(); // reset paddles and ball position
}

void update(float dt) {
    // handle paddle movement
    float direction = 0.0f;
    float direction2 = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[4])) {
        twoPMode ? twoPMode = false : twoPMode = true;
    }
    if (sf::Keyboard::isKeyPressed(controls[0])) {
        direction--;
    }
    if (sf::Keyboard::isKeyPressed(controls[1])) {
        direction++;
    }
    if (twoPMode) {
        if (sf::Keyboard::isKeyPressed(controls[2])) {
            direction2--;
        }
        if (sf::Keyboard::isKeyPressed(controls[3])) {
            direction2++;
        }
    }
    else if (!twoPMode) {
        if (ball.getPosition().y > paddles[1].getPosition().y) {
            direction2++;
        }
        if (ball.getPosition().y < paddles[1].getPosition().y) {
            direction2--;
        }
    }
    

    paddles[0].move(sf::Vector2f(0.f, direction * paddleSpeed * dt));
    paddles[1].move(sf::Vector2f(0.f, direction2 * paddleSpeed * dt));
    ball.move(ball_velocity * dt);

    // check paddle edge collision
    const float p1y = paddles[0].getPosition().y;
    if ((p1y + (paddleSize.y * 0.5)) > gameHeight) { //bottom wall
        paddles[0].move(sf::Vector2f(0.f, -7.5f));
    }
    else if (p1y - (paddleSize.y * 0.5) < 0) { //top wall
        paddles[0].move(sf::Vector2f(0.f, 7.5f));
    }
    const float p2y = paddles[1].getPosition().y;
    if (p2y + (paddleSize.y * 0.5) > gameHeight) { //bottom wall
        paddles[1].move(sf::Vector2f(0.f, -7.5f));
    }
    else if (p2y - (paddleSize.y * 0.5) < 0) { //top wall
        paddles[1].move(sf::Vector2f(0.f, 7.5f));
    }

    // check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;
    if (by > gameHeight) { //bottom wall
        // bottom wall
        ball_velocity.x *= velocity_multiplier;
        ball_velocity.y *= -velocity_multiplier;
        ball.move(sf::Vector2f(0.f, -10.f));
    }
    else if (by < 0) { //top wall
        // top wall
        ball_velocity.x *= velocity_multiplier;
        ball_velocity.y *= -velocity_multiplier;
        ball.move(sf::Vector2f(0.f, 10.f));
    }
    else if (bx > gameWidth) {
        // right wall
        is_player1_serving = false;
        p2score++;
        reset();
    }
    else if (bx < 0) {
        // left wall
        is_player1_serving = true;
        p1score++;
        reset();
    }
    else if (
        //ball is inline or behind paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5)) {
            // bounce off left paddle
            ball_velocity.x *= -velocity_multiplier;
            ball_velocity.y *= velocity_multiplier;
            ball.move(sf::Vector2f(10.f, 0.f));
    }
    else if (
        //ball is inline or behind paddle AND
        bx > gameWidth - (paddleSize.x + paddleOffsetWall) &&
        //ball is below top edge of paddle AND
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5)) {
            // bounce off right paddle
            ball_velocity.x *= -velocity_multiplier;
            ball_velocity.y *= velocity_multiplier;
            ball.move(sf::Vector2f(-10.f, 0.f));
    }
}

void render(sf::RenderWindow& window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(scoreText);
}

int main() {
    //create the window
    sf::RenderWindow window(sf::VideoMode({ gameWidth, gameHeight }), "PONG");
    //initialise and load
    init();
    while (window.isOpen()) {
        //Calculate dt
        static sf::Clock clock;
        const float dt = clock.restart().asSeconds();

        window.clear();
        update(dt);
        render(window);
        //wait for the time_step to finish before displaying the next frame.
        sf::sleep(time_step);
        //wait for Vsync
        window.display();
    }

}