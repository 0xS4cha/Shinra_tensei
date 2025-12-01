class Grid {
    public:
        std::vector<bool> cells;
        countNeighbors(int x, int y);
        getCell(int x, int y);
        setCell(int x, int y, bool alive);
        update();
};