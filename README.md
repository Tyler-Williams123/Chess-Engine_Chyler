A chess engine written from scratch in c

This project was built to help learn c, search algorithms, and performance-oriented programming.

Features:
- Legal move generation
- Make / unmake moves
- Perft testing
- Position evaluation
- Minimax search
- Alpha-beta pruning
- Iterative deepening
- Move ordering
- Zobrist hashing
- Incremental evaluation updates

Building:
This project is written in C and can be compiled with a C compiler
gcc *.c -o chess

Running:
This engine supports a limited set of UCI commands:
- isready
- ucinewgame
- quit
- uci
- position -- supports startpos and FEN strings
- eval
- go -- currently only supports depth

Future improvements:
- Time based search
- Improved evaluation function
- Further search optimizations
- Readability refactoring

Credits:
- Piece-square tabels taken from chess programing wiki Simplified evaluation function. https://www.chessprogramming.org/Simplified_Evaluation_Function
