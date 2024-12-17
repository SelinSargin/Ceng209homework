Dungeon Adventure Game

Dungeon Adventure is a text-based game where you explore a dungeon full of traps, monsters, and treasures. You begin at the dungeon entrance and must navigate through the rooms, fight monsters, collect items, and ultimately defeat all foes to win the game. The game ends either when you have vanquished every monster or when your health drops to zero (in which case, you lose).

Gameplay
Starting the Game
When you run the game, you start in the main menu. You can:

list: View saved games
load <filename>: Load a saved game (e.g., load mysave.save)
start: Begin a new game in the dungeon
exit: Quit the game
After starting or loading a game, you enter the dungeon world.

Controls & Commands
When the game starts, you’ll be placed in the Entrance room. You have various commands at your disposal:

Movement:
move up
move down
move left
move right
Use these to navigate between rooms according to the room connections defined in rooms.txt.

Look:
look
Describes your current room, showing any creatures present and items lying around.

Inventory:
inventory
Lists items you are currently carrying.

Pickup/Drop:
pickup <itemname>
drop <itemname>
Collect items from the room or drop items from your inventory.

Use Items:
use Potion
use Durability Potion
Potions can restore health or grant trap immunity.

Attack:
attack
Engage in combat with any creature in the current room. Combat continues until either you or the creature is defeated.

Health:
health
Shows your current health stats.

Map:
map
Prints a map of the dungeon layout.

Save/Load:
save <filename> to save your current game.
load <filename> to load a previously saved game (only from the main menu).
Exit:

exit
Quit the game at any time.

Winning and Losing
Win Condition: Defeat all monsters in the dungeon. Once all creatures are gone, you see a victory message and the game ends.
Lose Condition: If your health drops to zero (due to combat or traps), you die and the game ends.
Code Structure
The project consists of the following key components:

Main C Source File (main.c or similar):
Contains main() and overall game loop logic.
Handles player input and dispatches commands to the corresponding functions.

Player, Room, Creature, Item Structures:
Defined as typedefed structs for clarity.

Player: Holds player state (health, inventory, current room).
Room: Holds information about a single room (description, trap damage, items, creature presence, and adjacent rooms).
Creature: Stores monster stats (health, strength, description).
Item: Stores item properties (name, description, weight).
Functions for Initialization:

initialize_game(): Reads data from rooms.txt, creatures.txt, and items.txt to build the game world dynamically.
load_rooms(), load_creatures(), load_items(): Parse these files and populate the internal data structures accordingly.
Command Handling:

parse_and_execute_command(): Interprets user input and calls the correct action functions (move_player(), look(), attack(), etc.).
Game Logic Functions:

Movement (move_player()): Changes player->current_room based on direction.
Combat (attack()): Conducts combat rounds between player and creature until one is defeated.
Item Management (pickup(), drop(), use_potion()): Modifies player inventory and room items.
Trap Handling: If trap_damage > 0 in a room, the player may take damage upon entering, unless protected by a Durability Potion.
Health & Status (show_health()): Displays current player health.
Saving and Loading:

save_game(): Writes the current state of the player, rooms, creatures, and items to a .save file.
load_game(): Reads a .save file to restore a previously saved game state.
Memory Management:

free_memory(): Frees all dynamically allocated memory (rooms, items, creatures, player inventory) upon game exit.
Game Logic
Initialization: On start, initialize_game() reads rooms.txt for room layout and traps, creatures.txt for monsters, and items.txt for starting items. It sets the player’s current_room to Entrance and initializes their inventory and stats.

Exploration:
Players move through rooms using move <direction>. The direction corresponds directly to room connections defined in rooms.txt. If a trap is present, it triggers upon entering the room.

Combat:
If a creature is present, the player can attack. Combat is turn-based:

Player hits creature, reducing its health by (player strength + weapon bonus).
If creature still lives, it retaliates, dealing damage reduced by player’s defensive items.
Combat repeats until either the creature dies (incrementing victory condition closer to win) or the player’s health hits zero (losing the game).
Item Interaction:
Players pickup items found in the room to add them to their inventory, use potions to restore health or gain trap immunity, or drop unwanted items.

Win/Lose Condition:
Each creature is tracked with total_monsters. Every time a creature is defeated, creature_defeated() is called, decrementing total_monsters. When total_monsters hits 0, the player wins. If the player’s health reaches 0 due to traps or combat, they lose.

Data Files Format
rooms.txt: Defines rooms and connections. 
Format:
RoomName|Description|TrapDamage|UpRoom|DownRoom|LeftRoom|RightRoom
creatures.txt: Defines monsters. 

Format:
CreatureName|Description|Health|Strength|RoomName
items.txt: Defines items. 

Format:
ItemName|Description|Weight|RoomName

you can easily customize the dungeon layout, the monsters, and the items without changing the C source code.

