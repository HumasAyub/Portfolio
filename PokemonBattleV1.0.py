#import random #Could implement a random Charizard attack in the future

# Exception for invalid moves (e.g. using the same attack twice in a row)
class InvalidMove(Exception):
    pass

# Exception for battle ended (e.g. when one of the players loses)
class BattleEnded(Exception):
    pass

# Pikachu class representing the player's character
class Pikachu:
    def __init__(self):
        self.hp = 15
        self.attacks = {
            "Volt Tackle": 5,   # Volt Tackle attack does 5 damage to the charizard
            "Electroweb": 4,   # Electroweb attack does 4 damage to the charizard
            "Electro Ball": 3    # Electro Ball attack does 3 damage to the charizard
        }
        self.last_attack = None   # Last attack used by the Pikachu

    # Method to attack the Charizard
    def attack(self, attack_name):
        # Check if the same attack was used twice in a row
        if attack_name == self.last_attack:
            raise InvalidMove("Cannot use the same attack twice in a row.")
        # Check if the attack name is valid
        if attack_name not in self.attacks:
            raise InvalidMove("Invalid attack name.")
        # Get the damage of the attack
        damage = self.attacks[attack_name]
        # Set the last attack used by the Pikachu
        self.last_attack = attack_name
        # Return the damage
        return damage

# Charizard class representing the enemy
class Charizard:
    def __init__(self):
        self.hp = 30
        self.attacks = {
            1: 2,   #1st column is the round number, 2nd column is the charizard attack number which can be used to lookup what damage should be been dealt i.e a charizard attack number of 2 will do the same amount of damage as was done in round 1, since we are looking at the amount of damage attack number 2 caused when it was first used by the charizard
            2: 1,
            3: 3,
            4: 2,
            5: 1,
            6: 3,
            7: 2,
            8: 1,
            9: 3,
            10: 2,
            11: 1,
            12: 3,
            13: 2,
            14: 1,
            15: 3,
            16: 2,
            17: 1,
            18: 3,
            19: 2,
            20: 1,
            21: 3,
            22: 2,
            23: 1,
            24: 3
        }

    # Method for the charizard to attack the Pikachu
    def attack(self, round_num):
        # Check if the round number is valid
        if round_num not in self.attacks:
            raise InvalidMoveException("Invalid round number.")
        # Get the damage of the attack
        damage = self.attacks[round_num]
        # Return the damage
        return damage

# Battle class representing the battle between the Pikachu and the Charizard
class Battle:
    def __init__(self):
        self.pikachu = Pikachu()          # Create a Pikachu instance
        self.charizard = Charizard()      # Create a Charizard instance
        self.round_num = 0                # Round number (starts at 0)
        self.last_pikachu_attack = None   # Last attack used by the Pikachu
        self.last_chairzard_attack = None      # Last attack used by the Charizard

    def advance_round(self, pikachu_attack_name):
        pikachu_damage = self.pikachu.attack(pikachu_attack_name) # Generate a number for damage dealt by Pikachu depending on user input
        charizard_attack_number = self.charizard.attack(self.round_num + 1) # Generate a number depending on the round number (To make Charizard attacks go from one attack to another based on a pre-defined order that loops back ie Attack A, Attack B, Attack C, Attack A ...)
        if charizard_attack_number == 2:
            charizard_attack_name = "Air Slash" # Air Slash is the first Charizard attack in the loop
        if charizard_attack_number == 1:
            charizard_attack_name = "Dragon Breath" # Dragon Breath is the second Charizard attack in the loop
        if charizard_attack_number == 3:
            charizard_attack_name = "Ember" # Ember is the third Charizard attack in the loop
        charizard_damage = self.charizard.attacks[charizard_attack_number] #Using the generated number (charizard_attack_number) to determine damage dealt by Charizard
        self.charizard.hp -= pikachu_damage
        if self.charizard.hp < 0:
            self.charizard.hp = 0
        self.pikachu.hp -= charizard_damage
        if self.pikachu.hp < 0:
            self.pikachu.hp = 0

        if self.charizard.hp <= 0: #Charizard hp is calculated first since the Pikachu attacks first
            print(f"Pikachu used {pikachu_attack_name} and dealt {result[0]} damage.")
            self.pikachu.hp += charizard_damage
            self.last_charizard_attack = "None"
            print(battle.battle_info())
            raise BattleEnded("Charizard has been defeated.")
        if self.pikachu.hp <= 0:
            print(f"Charizard used {result[1]} and dealt {result[2]} damage.")
            self.charizard.hp += pikachu_damage
            self.last_pikachu_attack = "None"
            print(battle.battle_info())
            raise BattleEnded("Pikachu has been defeated.")

        self.round_num += 1
        self.last_pikachu_attack = pikachu_attack_name #Make the last Pikachu attack the one that has just been done
        self.last_charizard_attack = charizard_attack_name #Make the last Charizard attack the that has just been done
        return (pikachu_damage, charizard_attack_name, charizard_damage)

    def battle_info(self):
        return f"Round: {self.round_num}\n"\
               f"Pikachu HP: {self.pikachu.hp}\n"\
               f"Charizard HP: {self.charizard.hp}\n"\
               f"Last Pikachu Attack: {self.last_pikachu_attack}\n"\
               f"Last Charizard Attack: {self.last_charizard_attack}"


battle = Battle()

while True:
    pikachu_attack_name = input("Enter attack name for Pikachu: ")
    try:
        result = battle.advance_round(pikachu_attack_name)
        print(f"Pikachu used {pikachu_attack_name} and dealt {result[0]} damage.")
        print(f"Charizard used {result[1]} and dealt {result[2]} damage.")
        print(battle.battle_info())
    except InvalidMove as e: #When the user makes an invalid move it will prompt the user to make a valid one and continue from where it left off
        print(str(e))
    except BattleEnded as e: #When Pikachu or Charizard has been defeated the loop will break and the battle will end
        print(str(e))
        break
