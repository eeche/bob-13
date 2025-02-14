import sys
from web3 import Web3
import time
import pygame
from pygame.locals import QUIT, KEYDOWN

screen_size = {
    'width': 800,
    'height': 600
}

pygame.init()
screen = pygame.display.set_mode((screen_size['width'], screen_size['height']))
pygame.display.set_caption('Rock-Paper-Scissors')
clock = pygame.time.Clock()

ganache_url = 'http://127.0.0.1:8545'
web3 = Web3(Web3.HTTPProvider(ganache_url))

if not web3.is_connected():
    print('Could not connect to Ganache')
    exit()

web3.eth.default_account = web3.eth.accounts[0]
# abi from RockPaperScissors.sol
abi = [
	{
		"anonymous": False,
		"inputs": [
			{
				"indexed": False,
				"internalType": "string",
				"name": "result",
				"type": "string"
			}
		],
		"name": "GameResult",
		"type": "event"
	},
	{
		"inputs": [],
		"name": "betAmount",
		"outputs": [
			{
				"internalType": "uint256",
				"name": "",
				"type": "uint256"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [],
		"name": "computerMove",
		"outputs": [
			{
				"internalType": "enum RockPaperScissors.Move",
				"name": "",
				"type": "uint8"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "enum RockPaperScissors.Move",
				"name": "move",
				"type": "uint8"
			}
		],
		"name": "play",
		"outputs": [
			{
				"internalType": "string",
				"name": "",
				"type": "string"
			}
		],
		"stateMutability": "nonpayable",
		"type": "function"
	},
	{
		"inputs": [],
		"name": "playerMove",
		"outputs": [
			{
				"internalType": "enum RockPaperScissors.Move",
				"name": "",
				"type": "uint8"
			}
		],
		"stateMutability": "view",
		"type": "function"
	}
]
# contract_address from RockPaperScissors.sol
contract_address = '0xc1Bcc92b9A6244867695A483623f83b5FB6cEa70'
contract = web3.eth.contract(address=contract_address, abi=abi)

def move_to_string(move):
    if move == 1:
        return 'rock'
    elif move == 2:
        return 'paper'
    elif move == 3:
        return 'scissors'
    else:
        return 'none'
    
def draw_text(text, x, y):
    font = pygame.font.Font(None, 30)
    text = font.render(text, False, (0, 0, 0))
    width = text.get_width()
    height = text.get_height()
    screen.blit(text, (x - width / 2, y - height / 2))

    return True

def play_game(move):
    global sel
    screen.fill((255, 255, 255))

    moves = {"rock": 1, "paper": 2, "scissors": 3}
    if move not in moves:
        draw_text('Please select(1~3)', screen.get_width() / 2, screen.get_height() / 2)
        pygame.display.update()
        return False
    
    tx_hash = contract.functions.play(moves[move]).transact()
    tx_receipt = web3.eth.wait_for_transaction_receipt(tx_hash)

    logs = contract.events.GameResult().process_receipt(tx_receipt)
    for log in logs:
        print(f"Game result: {log['args']['result']}")

    player_move = contract.functions.playerMove().call()
    computer_move = contract.functions.computerMove().call()
    print(f'Player move: {move_to_string(player_move)}',
          f'Computer move: {move_to_string(computer_move)}')

    sel = 0
    player_image = pygame.image.load(f'images/{move_to_string(player_move)}.png')
    computer_image = pygame.image.load(f'images/{move_to_string(computer_move)}.png')
    player_image = pygame.transform.scale(player_image, (100, 100))
    x = (screen.get_width() / 3) - (player_image.get_width() / 2)
    y = (screen.get_height() / 2) - (player_image.get_height() / 2)
    screen.blit(player_image, (x, y, 0, 0))

    computer_image = pygame.transform.scale(computer_image, (100, 100))
    x = (screen.get_width() / 3 * 2) - (computer_image.get_width() / 2)
    y = (screen.get_height() / 2) - (computer_image.get_height() / 2)
    screen.blit(computer_image, (x, y, 0, 0))

    draw_text(log['args']['result'], screen.get_width() / 2, screen.get_height() / 3 * 2)

    pygame.display.update()
    time.sleep(3)
    return True

fps = 300
count = 60
index = 0
sel = 0
selects = ['none', 'rock', 'paper', 'scissors']

while True:
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()
        elif event.type == KEYDOWN:
            if event.key == pygame.K_1:
                sel = 1
            elif event.key == pygame.K_2:
                sel = 2
            elif event.key == pygame.K_3:
                sel = 3

    if index > count:
        index = 0
        play_game(selects[sel])
    else:
        index = index + 1
    
    clock.tick(fps)