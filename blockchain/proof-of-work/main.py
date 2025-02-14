import hashlib
import datetime
import json
import random
import nodelib
import viewlib

nodes = []
nodeCount = 10
limit = 20
view = viewlib.View(1024, 768)
reward = 50

difficulty = 256 * 'f'

def get_gen_block(difficulty):
    block = {
        'ver': 1,
        'prev_hash': '',
        'mrkl_root': '',
        'time': datetime.datetime.now().timestamp(),
        'difficulty': difficulty,
        'nonce': 0,
        'transactions': []
    }

    difficulty = int(block['difficulty'], 16)

    while True:
        block['nonce'] += 1
        block_hash = hashlib.sha256(json.dumps(block).encode()).hexdigest()
        if int(block_hash, 16) < difficulty:
            break

    return block

def init():
    gen_block = get_gen_block(difficulty)

    for i in range(nodeCount):
        node = nodelib.Miner()
        node.address = hashlib.sha256(str(random.random()).encode()).hexdigest()
        node.recv_block(gen_block)
        nodes.append(node)

    return True

def broadcast_transaction(transaction):
    for node in nodes:
        node.recv_transaction(transaction)
        print(f"Transaction broadcasted to node: {node.address}")
    return True

def broadcast_block(block):
    recipients = []
    for i, node in enumerate(nodes):
        if node.recv_block(block):
            recipients.append(i)
    return recipients

def create_transaction():
    transactions = []
    for i in range(nodeCount):
        if random.randint(0, 1) > 0:
            sender = nodes[i]
            if sender.balance < 1:
                continue
            sel = i
            while sel == i:
                sel = random.randint(0, nodeCount - 1)
            recipient = nodes[sel]
            amount = random.randint(1, sender.balance)
            transaction = sender.send_to(recipient.address, amount)
            broadcast_transaction(transaction)
            transactions.append({
                'from': i,
                'to': sel,
                'amount': amount
            })
            print(f"Transaction created: Node {i} -> Node {sel}", f"Amount: {amount}")
    return transactions

def mining_process(difficulty):
    for repeat in range(limit):
        transactions = create_transaction()
        miner_index = random.randint(0, nodeCount - 1)
        miner = nodes[miner_index]
        candidate_block = miner.do_mining(difficulty)
        reward_tx = {
            'miner': miner_index,
            'amount': reward
        }
        transactions.append(reward_tx)
        recipients = broadcast_block(candidate_block)
        current_block = {
            'miner_index': miner_index,
            'recipients': recipients,
            'block_number': repeat + 1
        }

        block_info = [
            f' + Mining block {repeat + 1} -----------------',
            f'nounce: {candidate_block["nonce"]}'
        ]

        view.draw(nodes, current_block, block_info, transactions)

if __name__ == '__main__':
    init()
    mining_process(difficulty)