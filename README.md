# Chess-Engine
A bot capable of playing 3-check-chess, using the XBoard interface. Can perform all the classic chess moves like castling, promoting, en-passant.
Done by a team of 4 students. We used the Negamax algorithm, with Alpha-Beta pruning optimisation and Transposition tables to find the next optimal
move. The program can reach a depth of 6-7, but we consider improving it in the future.

To decide the best move we used an heuristic, based on the positional score for each piece, alongside material cost. To provide the integration with
XBoard, we read from stdin all the commands and made our bot act accordingly. For example, if XBoards tells us "B1C1", we update the internal table,
then we check if the bot is in check. After that, the best move is decided using the algorithms and heuristics explained earlier. 

To run our bot, simply run the command 'xboard -fcp "make run" -debug'.
