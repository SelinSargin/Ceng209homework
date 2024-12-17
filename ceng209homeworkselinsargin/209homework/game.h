#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct Player Player;
typedef struct Room Room;
typedef struct Creature Creature;
typedef struct Item Item;
typedef void (*GameAction)(Player *player, const char *argument);

struct Item {
    char *name;
    char *description;
    int weight;
};

struct Creature {
    char *name;
    char *description;
    int health;
    int strength;
};

struct Room {
    char *name;
    char *description;
    Room *up;
    Room *down;
    Room *left;
    Room *right;
    Creature *creature;
    Item **items;
    int item_count;
    int trap_damage;
};

struct Player {
    int health;
    int strength;
    int max_inventory_capacity;
    Item **inventory;
    int inventory_count;
    Room *current_room;
    int durability_potion_active;
    GameAction move;
    GameAction look;
    GameAction attack;
    GameAction pickup;
    GameAction drop;
    GameAction use_potion;
};

extern Room **all_rooms;
extern int room_count;
extern int total_monsters;
extern Player player_global;

Room *find_room_by_name(const char *name);
Item *create_item(const char *name, const char *description, int weight);
Creature *create_creature(const char *name, const char *description, int health, int strength);
void add_item_to_room(Room *room, Item *item);
int player_has_item(Player *player, const char *item_name);
int player_weapon_bonus(Player *player);
int player_damage_reduction(Player *player);
void move_player(Player *player, const char *direction);
void look(Player *player, const char *argument);
void show_inventory(Player *player);
void pickup(Player *player, const char *item_name);
void drop(Player *player, const char *item_name);
void attack(Player *player, const char *argument);
void use_potion(Player *player, const char *argument);
void show_health(Player *player);
void creature_defeated();
void free_memory(Player *player);
void parse_and_execute_command(Player *player, const char *input, int *game_started);
void list_saved_games();
void save_game(Player *player, const char *filepath);
void load_game(Player *player, const char *filepath);
void show_map();
Room *create_room_dynamic(const char *name, const char *description, int trap_damage);
void load_rooms();
void load_creatures();
void load_items();
void initialize_game(Player *player);

#endif
