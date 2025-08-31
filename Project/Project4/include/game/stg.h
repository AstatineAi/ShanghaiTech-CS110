#ifndef __STG_H
#define __STG_H

#include "lcd/lcd.h"
#include "utils.h"

// Game constants
#define PLAYER_WIDTH 2
#define PLAYER_HEIGHT 2
#define BULLET_WIDTH 1
#define BULLET_HEIGHT 1
#define ENEMY_WIDTH 3
#define ENEMY_HEIGHT 3
#define MAX_BULLETS 300      // more than 256 bullets
#define MAX_ENEMIES 10
#define DEBOUNCE_TIME 300    // 300ms debounce time for movement
#define SHOOT_DEBOUNCE 50    // 50ms debounce time for shooting
#define FPS_UPDATE_INTERVAL 120  // Update FPS every 120ms
#define MIN_FPS 30  // Minimum target FPS
#define HOMING_SPEED 1  // Speed of homing adjustment
#define MAX_CROSS_BOUNCES 4  // Maximum number of bounces for cross bullets

// Colors
#define PURPLE 0xF81F  // Purple color for cross bullets

// Bullet types
#define BULLET_TYPE_HOMING 0    // Player's homing bullet
#define BULLET_TYPE_NORMAL 1    // Enemy's normal bullet
#define BULLET_TYPE_CROSS 2     // Enemy's cross bullet
#define BULLET_TYPE_CIRCLE 3    // Enemy's circular bullet

// Enemy types
#define ENEMY_TYPE_NORMAL 0     // Normal shooting enemy
#define ENEMY_TYPE_CROSS 1      // Cross pattern enemy
#define ENEMY_TYPE_CIRCLE 2     // Circular pattern enemy

// Game states
typedef struct {
    int x;
    int y;
    int prev_x;  // Previous x position
    int prev_y;  // Previous y position
    int active;
} Player;

typedef struct {
    int x;
    int y;
    int prev_x;  // Previous x position
    int prev_y;  // Previous y position
    int active;
    int is_player_bullet;  // 1 for player bullets, 0 for enemy bullets
    int target_enemy;      // Index of target enemy for homing bullets (-1 if not homing)
    int dx;               // X direction for homing
    int dy;               // Y direction for homing
    int type;             // Bullet type
    int pattern_step;     // Step in pattern for special bullets
    int pattern_dir;      // Direction for pattern movement
    int bounce_count;     // Number of bounces for cross bullets
} Bullet;

typedef struct {
    int x;
    int y;
    int prev_x;  // Previous x position
    int prev_y;  // Previous y position
    int active;
    int move_direction;  // 0: left, 1: right
    int type;           // Enemy type
    int pattern_step;   // Step in shooting pattern
} Enemy;

typedef struct {
    Player player;
    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];
    int last_shot_time;
    int frame_count;
    int last_fps_update;
    int current_fps;
    int active_bullets;  // Counter for active bullets
} GameState;

// Function declarations
void init_game(GameState *game, int scene_number);
void update_game(GameState *game);
void draw_game(GameState *game);
int is_button_debounced(int button, int current_time, int *last_press_time);

#endif /* __STG_H */ 