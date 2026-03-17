#include "global.h"
#include "board.h"
#include "move.h"
#include "comm.h"
#include "minimax.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

/**********************************************************/
Position gamePosition;		// Position we are going to use

Move moveReceived;			// temporary move to retrieve opponent's choice
Move myMove;				// move to save our choice and send it to the server

char myColor;				// to store our color
int mySocket;				// our socket
char msg;					// used to store the received message

char * agentName = "Agent 47";		//default name.. change it! keep in mind MAX_NAME_LENGTH

char * ip = "127.0.0.1";	// default ip (local machine)
/**********************************************************/


int main( int argc, char ** argv )
{
	int c;
	opterr = 0;


	while( ( c = getopt ( argc, argv, "i:p:h" ) ) != -1 )
		switch( c )
		{
			case 'h':
				printf( "[-i ip] [-p port]\n" );
				return 0;
			case 'i':
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case '?':
				if( optopt == 'i' || optopt == 'p' )
					printf( "Option -%c requires an argument.\n", ( char ) optopt );
				else if( isprint( optopt ) )
					printf( "Unknown option -%c\n", ( char ) optopt );
				else
					printf( "Unknown option character -%c\n", ( char ) optopt );
				return 1;
			default:
			return 1;
		}


	connectToTarget( port, ip, &mySocket );

/**********************************************************/
// used in random
	//srand( time( NULL ) );
	//int i, j;
/**********************************************************/

	while( 1 )
	{

		msg = recvMsg( mySocket );

		switch ( msg )
		{
			case NM_REQUEST_NAME:		//server asks for our name
				sendName( agentName, mySocket );
				break;

			case NM_NEW_POSITION:		//server is trying to send us a new position
				getPosition( &gamePosition, mySocket );
				printPosition( &gamePosition );
				break;

			case NM_COLOR_W:			//server informs us that we have WHITE color
				myColor = WHITE;
				break;

			case NM_COLOR_B:			//server informs us that we have BLACK color
				myColor = BLACK;
				break;

			case NM_PREPARE_TO_RECEIVE_MOVE:	//server informs us that he will now send us opponent's move
				getMove( &moveReceived, mySocket );
				moveReceived.color = getOtherSide( myColor );
				doMove( &gamePosition, &moveReceived );		//play opponent's move on our position
				printPosition( &gamePosition );
				break;

			case NM_REQUEST_MOVE:		//server requests our move
				myMove.color = myColor;


				if( !canMove( &gamePosition, myColor ) )
				{
					myMove.tile[ 0 ] = NULL_MOVE;		// we have no move ..so send null move
				}
				else
				{


/**********************************************************/
// Agent 
                // In main(), before calling alphaBetaMove
            clock_t start = clock();                
          

            myMove = alphaBetaMove(gamePosition, myColor);

            // Stop the timer
            clock_t end = clock();
            double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;

            printf("Move: (%d, %d)\n", myMove.tile[0], myMove.tile[1]);
            printf("Execution time: %.6f seconds\n", elapsed_time);

// end of Agent
/**********************************************************/

				}

				sendMove( &myMove, mySocket );			//send our move
				doMove( &gamePosition, &myMove );		//play our move on our position
				printPosition( &gamePosition );
				break;

			case NM_QUIT:			//server wants us to quit...we shall obey
				close( mySocket );
				return 0;
		}

	} 

	return 0;
}



Move alphaBetaMove(Position gamePosition, char playerColor) {
    Move bestMove;
    bestMove.tile[0] = NULL_MOVE;  // Default move in case time runs out

    clock_t startTime = clock();  // Start timing

    for (int depth = 2; depth <= 100; depth+=2) {
        Move currentBestMove;
        int bestValue = maxValue(gamePosition, 0, depth, playerColor, &currentBestMove, INT_MIN, INT_MAX, startTime);

        // Check elapsed time
        double elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
        if (elapsedTime >= TIME_LIMIT) {
            printf("Time limit reached. Using best move found at depth %d\n", depth - 1);
            break;  // Stop searching deeper
        }

        // Store the best move found at this depth
        bestMove = currentBestMove;

        printf("Completed depth %d in %.6f seconds\n", depth, elapsedTime);
    }

    return bestMove;
}

// Function to compare moves for sorting (Descending for MAX player)
int compareMovesDesc(const void *a, const void *b) {
    return ((ScoredMove *)b)->evaluation - ((ScoredMove *)a)->evaluation;
}

// Function to compare moves for sorting (Ascending for MIN player)
int compareMovesAsc(const void *a, const void *b) {
    return ((ScoredMove *)a)->evaluation - ((ScoredMove *)b)->evaluation;
}



int maxValue(Position gamePosition, int depth,int depthMax, char playerColor, Move *bestMove, int alpha, int beta, clock_t startTime) {

    // Check if time is up
    double elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
    if (elapsedTime >= TIME_LIMIT) {
        return 0;  // Return a neutral evaluation when time runs out
    }

    if (depth >= depthMax || !canMove(&gamePosition, playerColor)) {
        *bestMove = (Move){.tile = {NULL_MOVE, NULL_MOVE}, .color = playerColor};
        int eval = evaluate(gamePosition, playerColor);
        //storeTransposition(&gamePosition, hashKey, eval, depth, *bestMove, 0);
        return eval;
    }

    int v = INT_MIN;
    Move optimalMove;
    optimalMove.tile[0] = NULL_MOVE;

    
    // Adaptive Futility Pruning (If move is unlikely to improve score, cut it)
    if (depth >= depthMax - 2) {  
        int staticEval = evaluate(gamePosition, playerColor);
        int futilityMargin = abs(staticEval) / 4;  // Scale with evaluation magnitude
        if (staticEval + futilityMargin <= alpha) return alpha;
}
    

    // Generate all possible moves
    ScoredMove moves[MAX_MOVES];
    int moveCount = 0;
    for (int i = 0; i < ARRAY_BOARD_SIZE; i++) {
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++) {
            if (gamePosition.board[i][j] == EMPTY) {
                Move tempMove = {.tile = {i, j}, .color = playerColor};

                if (isLegalMove(&gamePosition, &tempMove)) {
                    Position tempPos = gamePosition;
                    doMove(&tempPos, &tempMove);
                    moves[moveCount].move = tempMove;
                    moves[moveCount].evaluation = (depth == 0) ? evaluate(tempPos, playerColor) : 0;
                    moveCount++;
                }
            }
        }
    }

    // Sort moves only at root level
    if (depth <= 3) {
        qsort(moves, moveCount, sizeof(ScoredMove), compareMovesDesc);
    }

    for (int i = 0; i < moveCount; i++) {
        Position newPosition = gamePosition;
        doMove(&newPosition, &moves[i].move);

        Move responseMove;
        int v2 = minValue(newPosition, depth + 1, depthMax, getOtherSide(playerColor), &responseMove, alpha, beta, startTime);

        if (v2 > v) {
            v = v2;
            optimalMove = moves[i].move;
        }

        alpha = (alpha > v) ? alpha : v;
        if (v >= beta) {  // Cutoff
            *bestMove = optimalMove;
            return v;
        }
    }

    *bestMove = optimalMove;

    return v;
}

int minValue(Position gamePosition, int depth,int depthMax, char playerColor, Move *bestMove, int alpha, int beta, clock_t startTime) {

    // Check if time is up
    double elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
    if (elapsedTime >= TIME_LIMIT) {
        return 0;  // Return neutral evaluation when time runs out
    }

    if (depth >= depthMax || !canMove(&gamePosition, playerColor)) {
        *bestMove = (Move){.tile = {NULL_MOVE, NULL_MOVE}, .color = playerColor};
        int eval = evaluate(gamePosition, playerColor);
        //storeTransposition(&gamePosition, hashKey, eval, depth, *bestMove, 0);
        return eval;
    }

    int v = INT_MAX;
    Move optimalMove;
    optimalMove.tile[0] = NULL_MOVE;

    
    // Adaptive Futility Pruning (If move is unlikely to improve score, cut it)
    if (depth >= depthMax - 2) {  
        int staticEval = evaluate(gamePosition, playerColor);
        int futilityMargin = abs(staticEval) / 4;  // Scale dynamically
        if (staticEval - futilityMargin >= beta) return beta;
    }
    

    // Generate all possible moves
    ScoredMove moves[MAX_MOVES];
    int moveCount = 0;
    for (int i = 0; i < ARRAY_BOARD_SIZE; i++) {
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++) {
            if (gamePosition.board[i][j] == EMPTY) {
                Move tempMove = {.tile = {i, j}, .color = playerColor};

                if (isLegalMove(&gamePosition, &tempMove)) {
                    Position tempPos = gamePosition;
                    doMove(&tempPos, &tempMove);
                    moves[moveCount].move = tempMove;
                    moves[moveCount].evaluation = (depth == 1) ? evaluate(tempPos, playerColor) : 0;
                    moveCount++;
                }
            }
        }
    }

    // Sort moves only at root level
    if (depth<=3) {
        qsort(moves, moveCount, sizeof(ScoredMove), compareMovesAsc);  // Sorting in ascending order for Min player
    }

    for (int i = 0; i < moveCount; i++) {
        Position newPosition = gamePosition;
        doMove(&newPosition, &moves[i].move);

        Move responseMove;

        int v2 = maxValue(newPosition, depth + 1 , depthMax, getOtherSide(playerColor), &responseMove, alpha, beta, startTime);

        if (v2 < v) {
            v = v2;
            optimalMove = moves[i].move;
        }

        beta = (beta < v) ? beta : v;
        if (v <= alpha){  // Cutoff
            *bestMove = optimalMove;
            return v;
        }
    }

    
    *bestMove = optimalMove;

    return v;
}

int evaluate(Position gamePosition, char playerColor)
{
    int myScore = 0, opponentScore = 0;
    int myMobility = 0, opponentMobility = 0;
    int stabilityScore = 0, opponentStability = 0;
    int myFrontier = 0, opponentFrontier = 0;

    char opponentColor = getOtherSide(playerColor);

    int centerDistance = HEX_BOARD_RADIUS/2;
    int occupiedTiles = 0, totalTiles = 0;

    // Iterate over the board
    for (int i = 0; i < ARRAY_BOARD_SIZE; i++)
    {
        for (int j = 0; j < ARRAY_BOARD_SIZE; j++)
        {
            if (gamePosition.board[i][j] == OUT_OF_BOUND) 
                continue; // Skip out-of-bounds tiles

            totalTiles ++;

            char tile = gamePosition.board[i][j];

            // Count occupied tiles (for game phase detection)
            if (tile == playerColor || tile == opponentColor)
                occupiedTiles++;

            // Material Advantage (Count pieces)
            if (tile == playerColor)
                myScore++;
            else if (tile == opponentColor)
                opponentScore++;

            // Mobility (Count possible moves)
            if (tile == EMPTY)
            {
                Move move;
                move.tile[0] = i;
                move.tile[1] = j;

                move.color = playerColor;
                if (isLegalMove(&gamePosition, &move))
                    myMobility++;

                move.color = opponentColor;
                if (isLegalMove(&gamePosition, &move))
                    opponentMobility++;
            }


            // Frontier Disc Calculation
            if (tile == playerColor || tile == opponentColor) {
                int isFrontier = 0;

                // Check adjacent hexagons (HexThello has 6 neighbors)
                int dx[] = {-1, -1, 0, 0, 1, 1}; 
                int dy[] = {-1, 0, -1, 1, 0, 1};

                for (int d = 0; d < 6; d++) {
                    int ni = i + dx[d];
                    int nj = j + dy[d];

                    if (ni >= 0 && ni < ARRAY_BOARD_SIZE && nj >= 0 && nj < ARRAY_BOARD_SIZE &&
                        gamePosition.board[ni][nj] == EMPTY) {
                        isFrontier = 1;
                        break;
                    }
                }

                if (isFrontier) {
                    if (tile == playerColor) myFrontier++;
                    else opponentFrontier++;
                }
            }

            // Positional Strength (Edges, Corners, Stability)
            if (tile == playerColor || tile == opponentColor){
                int stabilityValue = 0;

                // Central stability
                if (abs(i - HEX_BOARD_RADIUS) + abs(j - HEX_BOARD_RADIUS) <= centerDistance)
                {
                    stabilityValue += 1;  // Center control bonus
                }

                // Edge bonus
                if (i == 0 || j == 0 || i == ARRAY_BOARD_SIZE - 1 || j == ARRAY_BOARD_SIZE - 1 ||
                    (i + j == HEX_BOARD_RADIUS) || (i + j == ARRAY_BOARD_SIZE + HEX_BOARD_RADIUS - 1))
                {
                    stabilityValue += 3;  
                }

                // Corner bonus (Most stable positions)
                if ((i == 0 && j == HEX_BOARD_RADIUS) || (i == HEX_BOARD_RADIUS && j == 0) ||
                    (i == ARRAY_BOARD_SIZE - 1 && j == HEX_BOARD_RADIUS) ||
                    (i == ARRAY_BOARD_SIZE - 1 - HEX_BOARD_RADIUS && j == ARRAY_BOARD_SIZE - 1) ||
                    (i == HEX_BOARD_RADIUS && j == ARRAY_BOARD_SIZE - 1) ||
                    (i == 0 && j == HEX_BOARD_RADIUS * 2))
                {
                    stabilityValue += 4;
                }

                // Store stability for both players
                if (tile == playerColor) stabilityScore += stabilityValue;
                else opponentStability += stabilityValue;
            }
            
        }
    }

    // Determine Game Phase
    double filledRatio = (double)occupiedTiles / totalTiles;

    int materialWeight, mobilityWeight, stabilityWeight, frontierWeight;
   
    // Adjust Weights Dynamically Based on Game Phase
    materialWeight  = (filledRatio < 0.25) ?  5  : (filledRatio < 0.75) ? 10 : 20;
    mobilityWeight  = (filledRatio < 0.25) ? 15  : (filledRatio < 0.75) ? 10 :  5;
    stabilityWeight = (filledRatio < 0.25) ? 10  : (filledRatio < 0.75) ? 15 :  5;
    frontierWeight  = (filledRatio < 0.25) ?  5  : (filledRatio < 0.75) ? 10 : 15;



    // Compute Final Score
    int score = (materialWeight * (myScore - opponentScore)) +
                (mobilityWeight * (myMobility - opponentMobility)) +
                (stabilityWeight * stabilityScore - opponentStability) -
                (frontierWeight * (myFrontier - opponentFrontier)); // Subtract frontier exposure

    return score;
}






