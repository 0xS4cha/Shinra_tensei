Game::Game()
{
    running = true;
}

/*Game::~Game()
{
    Nettoyage lorsque finit
}*/

void Game::run()
{
    while (running)
    {
        user_input_listener();
        update();
        render();
    }
}

void Game::render()
{

}

void Game::update()
{

}

void Game::user_input_listener()
{

}

void Game::stop()
{
    running = false;
}