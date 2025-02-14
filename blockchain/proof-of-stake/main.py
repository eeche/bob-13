import random

class Transaction:
    def __init__(self, sender, receiver, amount):
        self.sender = sender
        self.receiver = receiver
        self.amount = amount

    def __repr__(self):
        return f"{self.sender.name} -> {self.receiver.name} : {self.amount}"

class Blockchain:
    def __init__(self):
        self.nodes = []
        self.chain = []
        self.pending_transactions = []

    def add_node(self, node):
        self.nodes.append(node)

    def total_stake(self):
        return sum(node.stake for node in self.nodes)

    def select_validator(self):
        total_stake = self.total_stake()
        pick = random.uniform(0, total_stake)
        current = 0
        for node in self.nodes:
            current += node.stake
            if current > pick:
                return node

    def add_transaction(self, transaction):
        self.pending_transactions.append(transaction)

    def create_block(self, validator):
        block = {
            'validator': validator.name,
            'transactions': self.pending_transactions.copy()
        }
        self.chain.append(block)
        self.pending_transactions.clear()
        return block

class Node:
    def __init__(self, name, balance):
        self.name = name
        self.balance = balance
        self.stake = 0
        self.transactions = []

    def create_transaction(self, receiver, amount):
        transaction = Transaction(self, receiver, amount)
        self.transactions.append(transaction)
        return transaction

    def stake_coins(self, amount):
        if amount <= self.balance:
            self.balance -= amount
            self.stake += amount
            print(f"{self.name} staked {amount} coins. "
                  f"/ balance is {self.balance}")
        else:
            print(f"{self.name} does not have enough "
                  f"balance to stake {amount} coins.")

    def release(self):
        self.transactions.clear()
        self.balance += self.stake
        self.stake = 0

    def rewarding(self):
        self.balance += 50

users = ['Alice', 'Bob', 'Charlie', 'Dave', 'Eve']
limit = 2000
nodes = []
network = Blockchain()

for user in users:
    balance = random.randint(5, 100)
    node = Node(user, balance)
    network.add_node(node)
    nodes.append(node)

for node in nodes:
    print(f"{node.name}'s balance - {node.balance}")

for i in range(0, limit):
    print(f"\nSlot {i + 1}")

    for node in nodes:
        if node.balance < 1:
            continue

        stake = 1 if node.balance == 1 else random.randint(1, node.balance)
        node.stake_coins(stake)

    for sender_index, sender in enumerate(nodes):
        receiver_index = sender_index
        while sender_index == receiver_index:
            receiver_index = random.randint(0, len(nodes) - 1)

        receiver = nodes[receiver_index]
        amount = random.randint(0, sender.balance)

        if amount > 0:
            transaction = sender.create_transaction(receiver, amount)
            network.add_transaction(transaction)

    validator = network.select_validator()
    new_block = network.create_block(validator)
    validator.rewarding()

    for node in nodes:
        transactions = node.transactions
        for transaction in transactions:
            transaction.sender.balance -= transaction.amount
            transaction.receiver.balance += transaction.amount

        node.release()

print("\nStatus of Blockchain:")
for i, block in enumerate(network.chain, start=1):
    print(f"Block {i}")
    print(f" Validator - {block['validator']}")
    print(f" Transactions - {block['transactions']}\n")

for node in nodes:
    print(f"{node.name}'s balance - {node.balance}")
