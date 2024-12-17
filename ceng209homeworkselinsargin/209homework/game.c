#include "game.h"

Room **all_rooms = NULL;
int room_count = 0;
int total_monsters = 0;

Player player_global;

Room *create_room_dynamic(const char *name, const char *description, int trap_damage) {
    Room *room = malloc(sizeof(Room));
    room->name = strdup(name);
    room->description = strdup(description);
    room->up = room->down = room->left = room->right = NULL;
    room->creature = NULL;
    room->items = NULL;
    room->item_count = 0;
    room->trap_damage = trap_damage;
    return room;
}

Item *create_item(const char *name, const char *description, int weight) {
    Item *item = malloc(sizeof(Item));
    item->name = strdup(name);
    item->description = strdup(description);
    item->weight = weight;
    return item;
}

Creature *create_creature(const char *name, const char *description, int health, int strength) {
    Creature *creature = malloc(sizeof(Creature));
    creature->name = strdup(name);
    creature->description = strdup(description);
    creature->health = health;
    creature->strength = strength;
    total_monsters++;
    return creature;
}

void add_item_to_room(Room *room, Item *item) {
    room->items = realloc(room->items, (room->item_count + 1)*sizeof(Item*));
    room->items[room->item_count++] = item;
}

Room *find_room_by_name(const char *name) {
    for (int i = 0; i < room_count; i++) {
        if (strcmp(all_rooms[i]->name, name)==0) {
            return all_rooms[i];
        }
    }
    return NULL;
}

int player_has_item(Player *player, const char *item_name) {
    for (int i = 0; i < player->inventory_count; i++) {
        if (strcmp(player->inventory[i]->name, item_name) == 0) {
            return 1;
        }
    }
    return 0;
}

int player_weapon_bonus(Player *player) {
    if (player_has_item(player, "Katana")) {
        return 2;
    } else if (player_has_item(player, "Sword")) {
        return 1;
    }
    return 0;
}

int player_damage_reduction(Player *player) {
    int reduction = 0;
    if (player_has_item(player, "Shield")) reduction += 3;
    if (player_has_item(player, "Helmet")) reduction += 2;
    return reduction;
}

void creature_defeated() {
    total_monsters--;
    if (total_monsters <= 0) {
        printf("You have defeated all the monsters! You win the game!\n");
        exit(0);
    }
}

void load_rooms() {
    FILE *f = fopen("rooms.txt","r");
    if(!f) {
        printf("rooms.txt could not be opened.\n");
        exit(1);
    }
    char line[1024];
    int count=0;
    while(fgets(line,sizeof(line),f)) {
        if(line[0]=='\n'||line[0]=='#') continue;
        count++;
    }
    rewind(f);

    all_rooms = malloc(sizeof(Room*)*count);
    room_count = 0;

    while(fgets(line,sizeof(line),f)) {
        if(line[0]=='\n'||line[0]=='#') continue;
        line[strcspn(line,"\n")]=0;
        char *name=strtok(line,"|");
        char *desc=strtok(NULL,"|");
        char *trap=strtok(NULL,"|");
        char *up=strtok(NULL,"|");
        char *down=strtok(NULL,"|");
        char *left=strtok(NULL,"|");
        char *right=strtok(NULL,"|");
        int trapd=atoi(trap);
        Room *r = create_room_dynamic(name,desc,trapd);
        all_rooms[room_count++] = r;
    }
    fclose(f);

    f = fopen("rooms.txt","r");
    int idx=0;
    while(fgets(line,sizeof(line),f)) {
        if(line[0]=='\n'||line[0]=='#') continue;
        line[strcspn(line,"\n")]=0;
        char *name=strtok(line,"|");
        strtok(NULL,"|"); 
        strtok(NULL,"|"); 
        char *up=strtok(NULL,"|");
        char *down=strtok(NULL,"|");
        char *left=strtok(NULL,"|");
        char *right=strtok(NULL,"|");

        Room *r = find_room_by_name(name);
        if(up && strcmp(up,"NULL")!=0) r->up = find_room_by_name(up);
        if(down && strcmp(down,"NULL")!=0) r->down = find_room_by_name(down);
        if(left && strcmp(left,"NULL")!=0) r->left = find_room_by_name(left);
        if(right && strcmp(right,"NULL")!=0) r->right = find_room_by_name(right);

        idx++;
    }
    fclose(f);
}

void load_creatures() {
    FILE *f = fopen("creatures.txt","r");
    if(!f) {

        return;
    }
    char line[1024];
    while(fgets(line,sizeof(line),f)) {
        if(line[0]=='\n'||line[0]=='#') continue;
        line[strcspn(line,"\n")]=0;
        char *cname=strtok(line,"|");
        char *cdesc=strtok(NULL,"|");
        char *chp=strtok(NULL,"|");
        char *cstr=strtok(NULL,"|");
        char *rname=strtok(NULL,"|");
        int hp=atoi(chp);
        int st=atoi(cstr);
        Creature *cr = create_creature(cname,cdesc,hp,st);
        Room *r = find_room_by_name(rname);
        if(r) r->creature = cr;
    }
    fclose(f);
}

void load_items() {
    FILE *f = fopen("items.txt","r");
    if(!f) {
        return;
    }
    char line[1024];
    while(fgets(line,sizeof(line),f)) {
        if(line[0]=='\n'||line[0]=='#') continue;
        line[strcspn(line,"\n")]=0;
        char *iname=strtok(line,"|");
        char *idesc=strtok(NULL,"|");
        char *iw=strtok(NULL,"|");
        char *rname=strtok(NULL,"|");
        int w=atoi(iw);
        Item *it = create_item(iname, idesc, w);
        if(rname && strcmp(rname,"NULL")!=0) {
            Room *r = find_room_by_name(rname);
            if(r) add_item_to_room(r,it);
        }
    }
    fclose(f);
}

void initialize_game(Player *player) {
    load_rooms();
    load_creatures();
    load_items();

    Room *entrance = find_room_by_name("Entrance");
    if(!entrance) {
        printf("Entrance room not found.\n");
        exit(1);
    }

    player->current_room = entrance;
    player->inventory = malloc(player->max_inventory_capacity * sizeof(Item *));
    if (!player->inventory) {
        printf("Memory allocation error for player inventory.\n");
        exit(1);
    }

    player->inventory_count = 0;
    player->durability_potion_active = 0;

    player->move = move_player;
    player->look = look;
    player->attack = attack;
    player->pickup = pickup;
    player->drop = drop;
    player->use_potion = use_potion;
}

void look(Player *player, const char *argument) {
    Room *room = player->current_room;
    printf("You are in %s.\n%s\n", room->name, room->description);
    if (room->creature) {
        printf("You see a %s: %s\n", room->creature->name, room->creature->description);
    }
    if (room->item_count > 0) {
        printf("Items in the room:\n");
        for (int i = 0; i < room->item_count; i++) {
            printf("- %s: %s\n", room->items[i]->name, room->items[i]->description);
        }
    }
}

void show_inventory(Player *player) {
    if (player->inventory_count == 0) {
        printf("Your inventory is empty.\n");
    } else {
        printf("Your inventory contains:\n");
        for (int i = 0; i < player->inventory_count; i++) {
            printf("- %s: %s\n", player->inventory[i]->name, player->inventory[i]->description);
        }
    }
}

static void add_item_to_inventory(Player *player, Item *item) {
    if (player->inventory_count >= player->max_inventory_capacity) {
        printf("Inventory is full! Cannot carry more items.\n");
    } else {
        player->inventory[player->inventory_count++] = item;
        printf("You picked up %s.\n", item->name);
    }
}

void pickup(Player *player, const char *item_name) {
    Room *room = player->current_room;
    for (int i = 0; i < room->item_count; i++) {
        if (strcmp(room->items[i]->name, item_name) == 0) {
            add_item_to_inventory(player, room->items[i]);
            for (int j = i; j < room->item_count - 1; j++) {
                room->items[j] = room->items[j + 1];
            }
            room->item_count--;
            return;
        }
    }
    printf("Item not found in the room.\n");
}

void drop(Player *player, const char *item_name) {
    for (int i = 0; i < player->inventory_count; i++) {
        if (strcmp(player->inventory[i]->name, item_name) == 0) {
            printf("You dropped %s.\n", player->inventory[i]->name);
            add_item_to_room(player->current_room, player->inventory[i]);
            for (int j = i; j < player->inventory_count - 1; j++) {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory_count--;
            return;
        }
    }
    printf("You don't have that item in your inventory.\n");
}

void attack(Player *player, const char *argument) {
    Creature *creature = player->current_room->creature;
    if (!creature) {
        printf("There's nothing to attack here.\n");
        return;
    }

    int weapon_bonus = player_weapon_bonus(player);
    int reduction = player_damage_reduction(player);

    while (creature->health > 0 && player->health > 0) {
        printf("You attack the %s.\n", creature->name);
        int player_damage = player->strength + weapon_bonus;
        creature->health -= player_damage;
        if (creature->health <= 0) {
            printf("You defeated the %s!\n", creature->name);
            free(creature->name);
            free(creature->description);
            free(creature);
            player->current_room->creature = NULL;
            creature_defeated();
            return;
        }

        printf("The %s attacks you.\n", creature->name);
        int damage = creature->strength - reduction;
        if (damage < 0) damage = 0;

        player->health -= damage;
        if (player->health <= 0) {
            printf("You were defeated! Game Over.\n");
            exit(0);
        }

        printf("Your health: %d\n", player->health);
        printf("%s's health: %d\n", creature->name, creature->health);
    }
}

void use_potion(Player *player, const char *argument) {
    for (int i = 0; i < player->inventory_count; i++) {
        if (strcmp(player->inventory[i]->name, "Potion") == 0 && strcmp(argument, "Potion") == 0) {
            printf("You used a Potion! Your health is restored by 50.\n");
            player->health += 50;
            if (player->health > 100) player->health = 100;
            Item *p = player->inventory[i];
            for (int j = i; j < player->inventory_count - 1; j++) {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory_count--;
            free(p->name);
            free(p->description);
            free(p);
            return;
        } else if (strcmp(player->inventory[i]->name, "Durability Potion") == 0 && strcmp(argument, "Durability Potion") == 0) {
            printf("You used a Durability Potion! The next trap you step on will not affect you.\n");
            player->durability_potion_active = 1;
            Item *p = player->inventory[i];
            for (int j = i; j < player->inventory_count - 1; j++) {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory_count--;
            free(p->name);
            free(p->description);
            free(p);
            return;
        }
    }
    printf("You don't have that item to use.\n");
}

void show_health(Player *player) {
    printf("Your current health: %d\n", player->health);
}

void move_player(Player *player, const char *direction) {
    Room *next_room = NULL;
    if (strcmp(direction, "up") == 0) next_room = player->current_room->up;
    else if (strcmp(direction, "down") == 0) next_room = player->current_room->down;
    else if (strcmp(direction, "left") == 0) next_room = player->current_room->left;
    else if (strcmp(direction, "right") == 0) next_room = player->current_room->right;

    if (next_room) {
        player->current_room = next_room;
        printf("You moved to %s.\n", next_room->name);

        if (player->current_room->trap_damage > 0) {
            if (player->durability_potion_active == 1) {
                printf("Your Durability Potion protects you from the trap!\n");
                player->durability_potion_active = 0;
            } else {
                int damage = player->current_room->trap_damage;
                int reduction = player_damage_reduction(player);
                damage -= reduction;
                if (damage < 0) damage = 0;
                if (damage > 0) {
                    player->health -= damage;
                    printf("A trap hits you for %d damage!\n", damage);
                    if (player->health <= 0) {
                        printf("You died from the trap. Game Over.\n");
                        exit(0);
                    }
                    printf("Your health: %d\n", player->health);
                } else {
                    printf("A trap was triggered but your defenses prevent any damage.\n");
                }
            }
        }

    } else {
        printf("You can't go that way.\n");
    }
}

void free_memory(Player *player) {
    if(player->inventory) {
        for (int i = 0; i < player->inventory_count; i++) {
            free(player->inventory[i]->name);
            free(player->inventory[i]->description);
            free(player->inventory[i]);
        }
        free(player->inventory);
        player->inventory = NULL;
    }

    if (all_rooms) {
        for (int i = 0; i < room_count; i++) {
            Room *r = all_rooms[i];
            for (int j = 0; j < r->item_count; j++) {
                free(r->items[j]->name);
                free(r->items[j]->description);
                free(r->items[j]);
            }
            free(r->items);
            if (r->creature) {
                free(r->creature->name);
                free(r->creature->description);
                free(r->creature);
            }
            free(r->name);
            free(r->description);
            free(r);
        }
        free(all_rooms);
        all_rooms = NULL;
    }

    room_count = 0;
    total_monsters = 0;
}

void list_saved_games() {
    DIR *d;
    struct dirent *dir;
    d = opendir("./saves");
    if (!d) {
        printf("No 'saves' directory found or could not open it.\n");
        return;
    }

    printf("Saved games:\n");
    int found = 0;
    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".save")) {
            printf("- %s\n", dir->d_name);
            found = 1;
        }
    }
    if (!found) {
        printf("No saved games found.\n");
    }
    closedir(d);
}

void save_game(Player *player, const char *filepath) {
    mkdir("saves", 0777);

    char fullpath[256];
    snprintf(fullpath, sizeof(fullpath), "saves/%s", filepath);
    FILE *f = fopen(fullpath, "w");
    if (!f) {
        printf("Could not open file for saving.\n");
        return;
    }

    
    fprintf(f, "PLAYER %d %d %d %d %d\n", player->health, player->strength, player->inventory_count, player->durability_potion_active, total_monsters);

   
    for (int i = 0; i < player->inventory_count; i++) {
        fprintf(f, "ITEM %s|%s|%d\n", player->inventory[i]->name, player->inventory[i]->description, player->inventory[i]->weight);
    }

   
    fprintf(f, "CURRENT_ROOM %s\n", player->current_room->name);

    for (int i=0; i<room_count; i++) {
        Room *r = all_rooms[i];
        fprintf(f, "ROOM %s\n", r->name);
        if (r->creature) {
            fprintf(f, "CREATURE %s|%s|%d|%d\n", r->creature->name, r->creature->description, r->creature->health, r->creature->strength);
        } else {
            fprintf(f, "CREATURE NONE\n");
        }
        fprintf(f, "RITEMCOUNT %d\n", r->item_count);
        for (int j=0; j<r->item_count; j++) {
            fprintf(f, "RITEM %s|%s|%d\n", r->items[j]->name, r->items[j]->description, r->items[j]->weight);
        }
        fprintf(f, "TRAP %d\n", r->trap_damage);
    }

    fclose(f);
    printf("Game saved to %s.\n", fullpath);
}

void load_game(Player *player, const char *filepath) {
    char fullpath[256];
    snprintf(fullpath, sizeof(fullpath), "saves/%s", filepath);
    FILE *f = fopen(fullpath, "r");
    if (!f) {
        printf("Could not open file for loading.\n");
        return;
    }

    free_memory(player);

    int phealth, pstr, pinvcount, pdur, pmonsters;
    char line[1024];
    char current_room_name[256] = {0};

    Item **player_items_temp = NULL;
    int player_items_loaded = 0;

    while (fgets(line,sizeof(line),f)) {
        if (line[0]=='\n') continue;
        line[strcspn(line,"\n")]=0;

        if(strncmp(line,"PLAYER ",7)==0) {
            sscanf(line,"PLAYER %d %d %d %d %d",&phealth,&pstr,&pinvcount,&pdur,&pmonsters);
            player->health = phealth;
            player->strength = pstr;
            player->max_inventory_capacity = 3; 
            player->durability_potion_active = pdur;
            total_monsters = pmonsters;
            player->inventory = malloc(player->max_inventory_capacity * sizeof(Item*));
            player->inventory_count = 0;
        }
        else if(strncmp(line,"ITEM ",5)==0) {
            char *data = line+5;
            char *iname=strtok(data,"|");
            char *idesc=strtok(NULL,"|");
            char *iw=strtok(NULL,"|");
            int w=atoi(iw);
            Item *it = create_item(iname, idesc, w);
            if (player->inventory_count < player->max_inventory_capacity) {
                player->inventory[player->inventory_count++] = it;
            }
        }
        else if(strncmp(line,"CURRENT_ROOM ",13)==0) {
            strcpy(current_room_name, line+13);
            break;
        }
    }

    int filepos = ftell(f);
    int temp_room_count = 0;
    rewind(f);
    while(fgets(line,sizeof(line),f)) {
        if(strncmp(line,"ROOM ",5)==0) {
            temp_room_count++;
        }
    }
    fseek(f,filepos,SEEK_SET);

    all_rooms = malloc(sizeof(Room*)*temp_room_count);
    room_count = 0;

    int after_current_room_line = ftell(f);

    while(fgets(line,sizeof(line),f)) {
        if(strncmp(line,"ROOM ",5)==0) {
            line[strcspn(line,"\n")]=0;
            char *rname = line+5;

            char *room_name = strdup(rname);
            char *room_desc = strdup("No description");
            int trap_damage = 0;
            Creature *cr = NULL;
            Item **ritems = NULL;
            int ritem_count = 0;

            while(1) {
                long pos = ftell(f);
                if(!fgets(line,sizeof(line),f)) break;
                line[strcspn(line,"\n")]=0;
                if(strncmp(line,"ROOM ",5)==0) {
                    fseek(f,pos,SEEK_SET);
                    break;
                }
                if(strncmp(line,"CREATURE ",9)==0) {
                    if(strcmp(line,"CREATURE NONE")==0) {
                        cr = NULL;
                    } else {
                        char *data = line+9;
                        char *cname=strtok(data,"|");
                        char *cdesc=strtok(NULL,"|");
                        char *chp=strtok(NULL,"|");
                        char *cstr=strtok(NULL,"|");
                        int hp=atoi(chp);
                        int st=atoi(cstr);
                        cr = create_creature(cname,cdesc,hp,st);
                    }
                } else if(strncmp(line,"RITEMCOUNT ",11)==0) {
                } else if(strncmp(line,"RITEM ",6)==0) {
                    char *data = line+6;
                    char *iname=strtok(data,"|");
                    char *idesc=strtok(NULL,"|");
                    char *iw=strtok(NULL,"|");
                    int w=atoi(iw);
                    Item *it = create_item(iname,idesc,w);
                    ritems = realloc(ritems,(ritem_count+1)*sizeof(Item*));
                    ritems[ritem_count++] = it;
                } else if(strncmp(line,"TRAP ",5)==0) {
                    trap_damage=atoi(line+5);
                }
            }

            Room *R = create_room_dynamic(room_name,room_desc,trap_damage);
            R->creature = cr;
            R->items = ritems;
            R->item_count = ritem_count;
            free(room_name);
            free(room_desc);

            all_rooms[room_count++] = R;
        }
    }

    fclose(f);

    free_memory(player);
    load_rooms();
    load_creatures();
    load_items();

    player->health = phealth;
    player->strength = pstr;
    player->max_inventory_capacity = 3;
    player->durability_potion_active = pdur;
    total_monsters = pmonsters;

    player->inventory = malloc(player->max_inventory_capacity * sizeof(Item *));
    player->inventory_count = 0;

    f = fopen(fullpath, "r");
    while(fgets(line,sizeof(line),f)) {
        line[strcspn(line,"\n")]=0;
        if(strncmp(line,"PLAYER ",7)==0) {
        } else if(strncmp(line,"ITEM ",5)==0) {
            char *data = line+5;
            char *iname=strtok(data,"|");
            char *idesc=strtok(NULL,"|");
            char *iw=strtok(NULL,"|");
            int w=atoi(iw);
            Item *it = create_item(iname,idesc,w);
            if (player->inventory_count < player->max_inventory_capacity) {
                player->inventory[player->inventory_count++] = it;
            }
        } else if(strncmp(line,"CURRENT_ROOM ",13)==0) {
            strcpy(current_room_name, line+13);
            break;
        }
    }
    fclose(f);

    Room *crr = find_room_by_name(current_room_name);
    if(!crr) {
        crr = find_room_by_name("Entrance");
    }
    player->current_room = crr;

    player->move = move_player;
    player->look = look;
    player->attack = attack;
    player->pickup = pickup;
    player->drop = drop;
    player->use_potion = use_potion;

    printf("Game loaded from %s.\n", fullpath);
}

void show_map() {
    printf("\n------ Dungeon Map (6 rooms) ------\n");
    printf("                        Treasure Room → Armory\n");
    printf("                        ↑\n");
    printf("Library ← Entrance → Hallway\n");
    printf("                         ↓\n");
    printf("                        Deep Cave\n");
    printf("-----------------------------------\n");
}

void parse_and_execute_command(Player *player, const char *input, int *game_started) {
    char command[50], argument[200];
    argument[0] = '\0';

    int argc = sscanf(input, "%s %[^\n]", command, argument);

    if (argc >= 1) {
        if (strcmp(command, "list") == 0) {
            list_saved_games();
            return;
        }
        else if (strcmp(command, "save") == 0 && argc == 2) {
            if (*game_started) {
                save_game(player, argument);
            } else {
                printf("You have not started a game yet. Start a new game or load one first.\n");
            }
            return;
        }
        else if (strcmp(command, "load") == 0 && argc == 2) {
            load_game(player, argument);
            *game_started = 1; 
            return;
        }
        else if (strcmp(command, "exit") == 0) {
            printf("Exiting the game.\n");
            exit(0);
        }
    }

    if (!*game_started) {
        if (strcmp(command, "start") == 0) {
            player->max_inventory_capacity = 3;
            total_monsters = 0;
            initialize_game(player);
            *game_started = 1;
            printf("Game started! You are now in the dungeon.\n");
            return;
        } else {
            printf("You are currently in the main menu. Available commands: list, load <file>, start, exit.\n");
            return;
        }
    }

    if (argc == 1) {
        if (strcmp(command, "look") == 0) player->look(player, argument);
        else if (strcmp(command, "inventory") == 0) show_inventory(player);
        else if (strcmp(command, "attack") == 0) player->attack(player, argument);
        else if (strcmp(command, "health") == 0) show_health(player);
        else if (strcmp(command, "map") == 0) show_map();
        else if (strcmp(command, "start") == 0) {
            printf("A game is already running.\n");
        } else {
            printf("Unknown command: %s\n", command);
        }
    } else if (argc == 2) {
        if (strcmp(command, "move") == 0) player->move(player, argument);
        else if (strcmp(command, "pickup") == 0) player->pickup(player, argument);
        else if (strcmp(command, "drop") == 0) player->drop(player, argument);
        else if (strcmp(command, "use") == 0) player->use_potion(player, argument);
        else printf("Unknown command: %s\n", command);
    } else {
        printf("Invalid input.\n");
    }
}

int main() {
    Player player = {100, 10, 3, NULL, 0, NULL, 0};
    int game_started = 0;

    printf("Welcome to the Dungeon Adventure Game!\n");
    printf("You are currently in the main menu. Commands:\n");
    printf(" - list: List saved games\n");
    printf(" - load <filepath>: Load a saved game\n");
    printf(" - start: Start a new game\n");
    printf(" - map: Show the dungeon map (in game)\n");
    printf(" - exit: Exit the game\n");

    char command[256];
    while (1) {
        printf(">> ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }
        parse_and_execute_command(&player, command, &game_started);
    }

    free_memory(&player);
    return 0;
}

