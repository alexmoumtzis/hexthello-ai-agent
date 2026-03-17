# Hexthello AI Agent

An AI agent for **TUC-Hexthello**, a hexagonal variant of Othello/Reversi played on a hexagonal board. 

---

## About the Game

Hexthello is played on a hexagonal grid (radius 7) where two players — **Black** and **White** — take turns placing pieces. When a piece is placed, it flips all opponent pieces that are sandwiched in any of the **6 hexagonal directions**. The player with the most pieces at the end wins.

The game follows Othello rules adapted for a hexagonal topology:
- Black moves first
- A move must flip at least one opponent piece
- If a player cannot move, their turn is skipped
- The game ends when neither player can move

---

## AI Strategy

The AI agent uses a **Minimax algorithm with Alpha-Beta pruning** and several advanced techniques:

| Technique | Description |
|---|---|
| **Iterative Deepening** | Searches progressively deeper (depth 2, 4, 6, ...) within a 15-second time limit per move |
| **Alpha-Beta Pruning** | Eliminates branches that cannot influence the final decision |
| **Move Ordering** | Sorts moves by heuristic evaluation at shallow depths to improve pruning efficiency |
| **Adaptive Futility Pruning** | Skips moves near leaf nodes that are unlikely to improve the score |
| **Dynamic Evaluation Weights** | Adjusts heuristic weights based on game phase (early / mid / late) |

### Evaluation Heuristics

The evaluation function considers multiple strategic factors:

- **Material** — Piece count difference (weighted heavily in endgame)
- **Mobility** — Number of available moves (weighted heavily in opening)
- **Positional Stability** — Corner, edge, and center control bonuses
- **Frontier Discs** — Penalizes pieces adjacent to empty tiles (exposed to flipping)

Weights shift dynamically based on the board fill ratio:

| Phase | Material | Mobility | Stability | Frontier |
|---|---|---|---|---|
| Opening (<25% filled) | 5 | 15 | 10 | 5 |
| Midgame (25–75%) | 10 | 10 | 15 | 10 |
| Endgame (>75%) | 20 | 5 | 5 | 15 |

---

## Architecture

```
├── client.c        # AI agent (minimax + evaluation logic)
├── minimax.h       # AI function declarations
├── server.c        # Game server (manages matches between two clients)
├── guiServer.c     # GTK+ GUI server with visual board display
├── board.c/h       # Board representation, move execution, legality checks
├── comm.c/h        # Network communication (TCP sockets)
├── gameServer.c/h  # Game management logic (turns, scoring, game loop)
├── move.h          # Move data structure
├── global.h        # Constants and global definitions
├── makefile         # Build configuration
└── images/          # GUI assets for the visual server
```

---

## Requirements

- Linux (tested on Ubuntu/Debian)
- GCC compiler
- GTK+ 2.0 (for GUI server only)

---

## Build

```bash
make            # Build client, server, and GUI server
make client     # Build only the AI client
make server     # Build only the text server
make clean      # Remove compiled files
```

---

## Usage

### 1. Start the Server

**Text-based server:**
```bash
./server [-p port] [-g number_of_games] [-s]
```

| Flag | Description |
|---|---|
| `-p port` | Set server port (default: 6002) |
| `-g N` | Number of games to play |
| `-s` | Swap colors after each game |

**GUI server (requires GTK+ 2.0):**
```bash
./guiServer
```

### 2. Connect AI Clients

```bash
./client [-i ip] [-p port]
```

| Flag | Description |
|---|---|
| `-i ip` | Server IP address (default: 127.0.0.1) |
| `-p port` | Server port (default: 6002) |

### Example: Local Match

```bash
# Terminal 1 — Start server for 5 games with color swapping
./server -g 5 -s

# Terminal 2 — Connect first AI client
./client

# Terminal 3 — Connect second AI client
./client
```



## License

This project is an academic assignment for the Technical University of Crete.
