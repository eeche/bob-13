// SPDX-License-Identifier: MIT
pragma solidity >= 0.7;

contract RockPaperScissors {
    enum Move { None, Rock, Paper, Scissors }
    Move public playerMove;
    Move public computerMove;
    uint256 public betAmount;

    event GameResult(string result);

    function play(Move move) public returns (string memory) {
        require(move == Move.Rock || move == Move.Paper || move == Move.Scissors, "Invalid move");
        resetGame();

        betAmount = 1;
        playerMove = move;
        computerMove = randomMove();

        return determineWinner();
    }

    function randomMove() private view returns (Move) {
        uint256 rand = uint256(keccak256(abi.encodePacked(block.timestamp))) % 3 + 1;
        return Move(rand);
    }

    function determineWinner() private returns (string memory) {
        if(playerMove == computerMove) {
            emit GameResult("Draw");
            return "It's a draw!";
        } else if (
            (playerMove == Move.Rock && computerMove == Move.Scissors) ||
            (playerMove == Move.Paper && computerMove == Move.Rock) ||
            (playerMove == Move.Scissors && computerMove == Move.Paper)
        ) {
            emit GameResult("Players wins");
            return "You win!";
        } else {
            emit GameResult("Computer wins");
            return "You lose!";
        }
    }

    function resetGame() private {
        playerMove = Move.None;
        computerMove = Move.None;
        betAmount = 0;
    }
}