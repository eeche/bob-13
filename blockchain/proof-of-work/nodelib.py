import hashlib
import json

class Node:
    def __init__(self) -> None:
        self.address = 0
        self.balance = 0
        self.isMiner = False
        self.chain = []
        self.transactions = []

    def proof(self, block):
        difficulty = block['difficulty']
        hash = hashlib.sha256(json.dumps(block).encode()).hexdigest()
        if int(hash, 16) < int(difficulty, 16):
            return True
        
        return False
    
    def recv_block(self, block):
        res = self.proof(block)
        if res:
            self.chain.append(block)

        self.transactions = []
        return res

    def recv_transaction(self, transaction):
        sender = transaction['from']
        amount = transaction['amount']
        to = transaction['to']

        if sender == self.address:
            if amount > self.balance:
                return False
            self.balance -= amount
        if to == self.address:
            self.balance += amount
        
        self.transactions.append(transaction)
        return True
    
    def send_to(self, to, amount):
        transaction = {
            'from': self.address,
            'to': to,
            'amount': amount
        }
        return transaction
    
class Miner(Node):
    def __init__(self):
        super().__init__()
        self.isMiner = True
    
    def get_leafs(self, transactions):
        res = []
        for i in range(0, len(transactions)):
            hash = hashlib.sha256(json.dumps(transactions[i]).encode()).hexdigest()
            res.append(hash)
        return res
    
    def get_mrklroot(self, transactions):
        leafs = self.get_leafs(transactions)

        if len(leafs) <= 0:
            return ''
        elif len(leafs) == 1:
            return leafs[0]
        
        while len(leafs) > 1:
            hashA = leafs.pop(0)
            hashB = leafs.pop(0)
            hashC = hashlib.sha256(''.join([hashA, hashB]).encode()).hexdigest()
            leafs.append(hashC)
        return leafs[0]
    
    def do_mining(self, difficulty):
        time = self.chain[len(self.chain) - 1]['time'] + 600

        block = {
            'ver': 1,
            'prev_hash': '',
            'mrkl_root': self.get_mrklroot(self.transactions),
            'time': time,
            'difficulty': difficulty,
            'nonce': 0,
            'transactions': []
        }

        prev_hash = ''
        if len(self.chain) > 0:
            prev_block = self.chain[len(self.chain) - 1]
            prev_hash = hashlib.sha256(json.dumps(prev_block).encode()).hexdigest()
        
        self.recv_transaction({
            'from': 'none',
            'to': self.address,
            'amount': 50
        })
        block['transactions'] = self.transactions
        block['prev_hash'] = prev_hash
        difficulty = block['difficulty']

        while True:
            hash = hashlib.sha256(json.dumps(block).encode()).hexdigest()
            if int(hash, 16) < int(difficulty, 16):
                break
            block['nonce'] += 1
        return block