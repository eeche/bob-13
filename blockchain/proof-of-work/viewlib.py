import pygame
from pygame.locals import QUIT
import math

class View:
    def __init__(self, width, height):
        screenSize = {
            'width': width,
            'height': height
        }

        pygame.init()
        self.screen = pygame.display.set_mode((screenSize['width'], screenSize['height']))
        pygame.display.set_caption("Proof of Work")
        self.clock = pygame.time.Clock()
        self.fps = 30

    def draw(self, nodes, current_block, block_info, transactions):
        self.screen.fill((255, 255, 255))

        font = pygame.font.Font(None, 15)
        step = int(360 / len(nodes))
        positions = []

        for i, node in enumerate(nodes):
            degree = step * i
            theta = float(degree) * math.pi / 180

            x = int(self.screen.get_width() / 2) + int(self.screen.get_width() / 3) * math.cos(theta)
            y = int(self.screen.get_height() / 2) + int(self.screen.get_height() / 3) * math.sin(theta)

            positions.append((x, y))

        # Draw connections between the node creating the transaction and other nodes
        for i, pos in enumerate(positions):
            if i == current_block['miner_index'] or i in [tx['from'] for tx in transactions if 'from' in tx]:
                for end_pos in positions:
                    if pos != end_pos:
                        pygame.draw.line(self.screen, (200, 200, 200), pos, end_pos, 1)

        # Draw nodes above connections
        for i, node in enumerate(nodes):
            if i == current_block['miner_index']:
                node_color = (173, 255, 47)  # Light green for miner
            elif i in [tx['from'] for tx in transactions if 'from' in tx]:
                node_color = (255, 182, 193)  # Light pink for sender
            elif i in [tx['to'] for tx in transactions if 'to' in tx]:
                node_color = (255, 165, 0)  # Orange for recipient
            else:
                node_color = (255, 255, 255)  # White for others

            text = font.render(f'Node {i}', False, (0, 0, 0))
            balance_text = font.render(f"Balance: {node.balance}", False, (0, 0, 0))
            width = max(text.get_width(), balance_text.get_width()) + 20
            height = text.get_height() + balance_text.get_height() + 25

            x, y = positions[i]
            x -= width / 2
            y -= height / 2

            pygame.draw.rect(self.screen, node_color, [x, y, width, height])  # Fill the node with the determined color
            pygame.draw.rect(self.screen, (0, 0, 0), [x, y, width, height], 1, 5)  # Draw the node border
            self.screen.blit(text, (x + 10, y + 10))
            self.screen.blit(balance_text, (x + 10, y + 25))

        # Display block information on the top left corner
        info_font = pygame.font.Font(None, 20)
        info_y = 10
        for line in block_info:
            info_text = info_font.render(line, False, (0, 0, 0))
            self.screen.blit(info_text, (10, info_y))
            info_y += 20

        # Display transactions on the bottom right corner
        tx_font = pygame.font.Font(None, 20)
        tx_y = self.screen.get_height() - 20 * len(transactions) - 10
        for idx, tx in enumerate(transactions):
            if 'from' in tx and 'to' in tx:
                tx_text = tx_font.render(f'Transaction {idx + 1}: Node {tx["from"]} -> Node {tx["to"]}: {tx["amount"]}', False, (0, 0, 0))
            else:
                tx_text = tx_font.render(f'Transaction {idx + 1}: Node {tx["miner"]} receives reward: {tx["amount"]}', False, (0, 0, 0))
            self.screen.blit(tx_text, (self.screen.get_width() - tx_text.get_width() - 10, tx_y))
            tx_y += 20

        pygame.display.update()
        self.clock.tick(self.fps)

        # Animate transactions
        for idx, tx in enumerate(transactions):
            if 'from' in tx and 'to' in tx:
                self.animate_transaction_transfer(positions[tx['from']], positions, f"Transaction {idx + 1}", nodes, current_block, block_info, transactions, positions)

        # Animate the block moving to each recipient node simultaneously
        miner_index = current_block['miner_index']
        miner_pos = positions[miner_index]
        self.animate_block_transfer(miner_pos, positions, f"Block {current_block['block_number']}", block_info, nodes, current_block, transactions, positions)

        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                quit()
        return True

    def animate_transaction_transfer(self, start_pos, end_positions, label, nodes, current_block, block_info, transactions, positions):
        steps = 30
        delta_positions = [( (end_pos[0] - start_pos[0]) / steps, (end_pos[1] - start_pos[1]) / steps ) for end_pos in end_positions]

        for step in range(steps):
            self.screen.fill((255, 255, 255))
            self.redraw(nodes, current_block, block_info, transactions, positions)

            for delta_pos, end_pos in zip(delta_positions, end_positions):
                current_x = start_pos[0] + step * delta_pos[0]
                current_y = start_pos[1] + step * delta_pos[1]
                pygame.draw.rect(self.screen, (255, 255, 255), [current_x - 50, current_y - 15, 100, 30], 0, 5)  # Draw transaction with rounded corners
                pygame.draw.rect(self.screen, (0, 0, 0), [current_x - 50, current_y - 15, 100, 30], 1, 5)  # Draw border
                label_text = pygame.font.Font(None, 20).render(label, False, (0, 0, 0))
                self.screen.blit(label_text, (current_x - 45, current_y - 10))

            pygame.display.update()
            self.clock.tick(self.fps)

    def animate_block_transfer(self, miner_pos, end_positions, label, block_info, nodes, current_block, transactions, positions):
        steps = 30
        delta_positions = [( (end_pos[0] - miner_pos[0]) / steps, (end_pos[1] - miner_pos[1]) / steps ) for end_pos in end_positions]

        for step in range(steps):
            self.screen.fill((255, 255, 255))
            self.redraw(nodes, current_block, block_info, transactions, positions)

            for delta_pos in delta_positions:
                current_x = miner_pos[0] + step * delta_pos[0]
                current_y = miner_pos[1] + step * delta_pos[1]
                pygame.draw.rect(self.screen, (210, 210, 210), [current_x - 50, current_y - 15, 100, 30], 0, 5)  # Draw block with rounded corners
                pygame.draw.rect(self.screen, (0, 0, 0), [current_x - 50, current_y - 15, 100, 30], 1, 5)  # Draw border
                label_text = pygame.font.Font(None, 20).render(label, False, (0, 0, 0))
                self.screen.blit(label_text, (current_x - 45, current_y - 10))

            pygame.display.update()
            self.clock.tick(self.fps)

    def redraw(self, nodes, current_block, block_info, transactions, positions):
        font = pygame.font.Font(None, 15)

        # Draw connections between the node creating the transaction and other nodes
        for i, pos in enumerate(positions):
            if i == current_block['miner_index'] or i in [tx['from'] for tx in transactions if 'from' in tx]:
                for end_pos in positions:
                    if pos != end_pos:
                        pygame.draw.line(self.screen, (200, 200, 200), pos, end_pos, 1)

        for i, node in enumerate(nodes):
            x, y = positions[i][0] - 20, positions[i][1] - 20
            width, height = 80, 40

            # Determine the node color based on its role in the current block
            if i == current_block['miner_index']:
                node_color = (173, 255, 47)  # Light green for miner
            elif i in [tx['from'] for tx in transactions if 'from' in tx]:
                node_color = (255, 182, 193)  # Light pink for sender
            elif i in [tx['to'] for tx in transactions if 'to' in tx]:
                node_color = (255, 165, 0)  # Orange for recipient
            else:
                node_color = (255, 255, 255)  # White for others

            pygame.draw.rect(self.screen, node_color, [x, y, width, height])  # Fill the node with the determined color
            pygame.draw.rect(self.screen, (0, 0, 0), [x, y, width, height], 1, 5)  # Draw the node border
            text = font.render(f'Node {i}', False, (0, 0, 0))
            balance_text = font.render(f"Balance: {node.balance}", False, (0, 0, 0))
            self.screen.blit(text, (x + 10, y + 10))
            self.screen.blit(balance_text, (x + 10, y + 25))

        # Display block information on the top left corner
        info_font = pygame.font.Font(None, 20)
        info_y = 10
        for line in block_info:
            info_text = info_font.render(line, False, (0, 0, 0))
            self.screen.blit(info_text, (10, info_y))
            info_y += 20

        # Display transactions on the bottom right corner
        tx_font = pygame.font.Font(None, 20)
        tx_y = self.screen.get_height() - 20 * len(transactions) - 10
        for idx, tx in enumerate(transactions):
            if 'from' in tx and 'to' in tx:
                tx_text = tx_font.render(f'Transaction {idx + 1}: Node {tx["from"]} -> Node {tx["to"]}: {tx["amount"]}', False, (0, 0, 0))
            else:
                tx_text = tx_font.render(f'Transaction {idx + 1}: Node {tx["miner"]} receives reward: {tx["amount"]}', False, (0, 0, 0))
            self.screen.blit(tx_text, (self.screen.get_width() - tx_text.get_width() - 10, tx_y))
            tx_y += 20
