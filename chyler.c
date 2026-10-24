#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "defs.h"
#include "board.h"
#include "moveGeneration.h"
#include "search.h"
#include "debugtools.h"
#include "Timer.h"

#define MAX_SEARCH_DEPTH 64

Board board;

int main(){ // add options, pondering, and time, figure out why bestmove is always b1b2
    char line[4000];
    initBoard(&board);

    while(fgets(line, sizeof(line), stdin)){
        if(strncmp("quit", line, 4) == 0){
            exit(0);
        }
        if(strncmp("ucinewgame", line, 10) == 0){
            initBoard(&board);
        }
        else if(strncmp("uci", line, 3) == 0){
            printf("id name Chyler\n");
            printf("id author Tyler Williams\n");
            printf("uciok\n");
            fflush(stdout);
        }
        else if(strncmp("isready", line, 7) == 0){
            printf("readyok\n");
            fflush(stdout);
        }
        else if(strncmp("position", line, 8) == 0){
            char* token = strtok(line, " ");
            token = strtok(NULL, " ");
            smol setup = 0;
            
            if(token == NULL)
                continue;

            if(strncmp("startpos", token, 8) == 0){
                setup = 1;
                initBoard(&board);
            }
            else if(strncmp("fen", token, 3) == 0){
                setup = 1;
                char fen[100] = "";
                for(int i = 0; i < 6; i++){
                    token = strtok(NULL, " ");
                    if(token == NULL)
                        continue;

                    strcat(fen, token);
                    strcat(fen, " ");
                }
                FENInit(&board, fen);
            }
            
            token = strtok(NULL, " ");
            if(setup && token != NULL && strncmp(token, "moves", 5) == 0){
                token = strtok(NULL, " ");
                while(token != NULL){
                    makeMove(&board, parseMove(token, &board));
                    token = strtok(NULL, " ");
                }
            }
        }
        else if(strncmp("go", line, 2) == 0){ // adding movetime
            char* token = strtok(line, " ");
            token = strtok(NULL, " ");
            
            if(token != NULL && strncmp("depth", token, 5) == 0){
                token = strtok(NULL, " ");
                int depth = token != NULL ? atoi(token) : 1;
                if(depth < 1) depth = 1;
                if(depth > MAX_SEARCH_DEPTH) depth = MAX_SEARCH_DEPTH;
                setTimer(-1);
                move bestMove = search(&board, depth);
                if(bestMove == NO_MOVE){
                    printf("bestmove 0000\n");
                }
                else{
                    char move[6];
                    moveToString(bestMove, move);
                    printf("bestmove %s\n", move);
                }
                fflush(stdout);
            }
            if(token != NULL && strncmp("movetime", token, 8) == 0){
                token = strtok(NULL, " ");
                if(token != NULL){
                    setTimer((u64)atoll(token)); // smol would truncate to 8 bits
                }
                else{
                    printf("invalid movetime command\n");
                    fflush(stdout);
                    continue;
                }
                
                move bestMove = search(&board, MAX_SEARCH_DEPTH); // the timer ends the search, not the depth
                setTimer(-1);
                if(bestMove == NO_MOVE){
                    printf("bestmove 0000\n");
                }
                else{
                    char move[6];
                    moveToString(bestMove, move);
                    printf("bestmove %s\n", move);
                }
                fflush(stdout);
            }
        }
        else if(strncmp(line, "eval", 4) == 0){
            int eval = evaluate(&board) / 100;
            printf("%d\n", eval);
            fflush(stdout);
        }
    }

    return 0;
}