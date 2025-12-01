class Game {
    public:
        Game();
        ~Game();
        bool running;
        void run();
        void render();
        void user_input_listener();
        void update();
        void stop();
};