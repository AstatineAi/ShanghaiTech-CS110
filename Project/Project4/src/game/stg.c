#include "game/stg.h"
#include <string.h>
#include <stdio.h>

// Button debounce times
static int last_button_times[7] = {0}; // One for each button
static int last_frame_time = 0;        // Track last frame time

// Find closest active enemy to a position
static int find_closest_enemy(GameState *game, int x, int y) {
    int closest = -1;
    int min_dist = LCD_W * LCD_W + LCD_H * LCD_H; // Max possible distance

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            int dx = game->enemies[i].x - x;
            int dy = game->enemies[i].y - y;
            int dist = dx * dx + dy * dy;
            if (dist < min_dist) {
                min_dist = dist;
                closest = i;
            }
        }
    }
    return closest;
}

// Check collision between bullet and enemy
static int check_bullet_enemy_collision(Bullet *bullet, Enemy *enemy) {
    return (bullet->x < enemy->x + ENEMY_WIDTH &&
            bullet->x + BULLET_WIDTH > enemy->x &&
            bullet->y < enemy->y + ENEMY_HEIGHT &&
            bullet->y + BULLET_HEIGHT > enemy->y);
}

void init_game(GameState *game, int scene_number) {
    // Initialize player
    game->player.x = LCD_W / 2 - PLAYER_WIDTH / 2;
    game->player.y = LCD_H - PLAYER_HEIGHT - 10;
    game->player.prev_x = game->player.x;
    game->player.prev_y = game->player.y;
    game->player.active = 1;

    // Initialize bullets
    memset(game->bullets, 0, sizeof(game->bullets));
    game->active_bullets = 0;

    // Initialize enemies with different types
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].x = (i % 5) * (LCD_W / 5);
        game->enemies[i].y = 20 + (i / 5) * 30;
        game->enemies[i].prev_x = game->enemies[i].x;
        game->enemies[i].prev_y = game->enemies[i].y;
        game->enemies[i].active = 1;
        game->enemies[i].move_direction = i % 2;
        game->enemies[i].type = i % 3;  // Distribute enemy types
        game->enemies[i].pattern_step = 0;
    }

    // Initialize game state
    game->last_shot_time = 0;
    game->frame_count = 0;
    game->last_fps_update = 0;
    game->current_fps = 0;
    last_frame_time = 0;
}

int is_button_debounced(int button, int current_time, int *last_press_time) {
    int debounce_time = (button == BUTTON_1) ? SHOOT_DEBOUNCE : DEBOUNCE_TIME;
    if (current_time - last_press_time[button] >= debounce_time) {
        last_press_time[button] = current_time;
        return 1;
    }
    return 0;
}

// Draw a cross-shaped bullet
static void draw_cross_bullet(int x, int y) {
    LCD_DrawPoint(x, y, PURPLE);
    LCD_DrawPoint(x-1, y, PURPLE);
    LCD_DrawPoint(x+1, y, PURPLE);
    LCD_DrawPoint(x, y-1, PURPLE);
    LCD_DrawPoint(x, y+1, PURPLE);
}

// Draw a circular bullet
static void draw_circle_bullet(int x, int y) {
    // Draw circle outline
    LCD_DrawPoint(x - 1, y, YELLOW);
    LCD_DrawPoint(x + 1, y, YELLOW);
    LCD_DrawPoint(x, y - 1, YELLOW);
    LCD_DrawPoint(x, y + 1, YELLOW);
}

void update_game(GameState *game) {
    int current_time = last_frame_time + 40;
    last_frame_time = current_time;
    game->player.prev_x = game->player.x;
    game->player.prev_y = game->player.y;

    // Update player position based on joystick
    if (Get_Button(JOY_LEFT) && game->player.x > 0) {
        game->player.x -= 2;
    }
    if (Get_Button(JOY_RIGHT) && game->player.x < LCD_W - PLAYER_WIDTH) {
        game->player.x += 2;
    }
    if (Get_Button(JOY_UP) && game->player.y > 0) {
        game->player.y -= 2;
    }
    if (Get_Button(JOY_DOWN) && game->player.y < LCD_H - PLAYER_HEIGHT) {
        game->player.y += 2;
    }

    if (Get_Button(BUTTON_1) &&
        is_button_debounced(BUTTON_1, current_time, last_button_times)) {
        // Try to fire multiple bullets
        for (int attempt = 0; attempt < 3; attempt++) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!game->bullets[i].active) {
                    game->bullets[i].x = game->player.x + PLAYER_WIDTH / 2 - BULLET_WIDTH / 2;
                    game->bullets[i].y = game->player.y;
                    game->bullets[i].prev_x = game->bullets[i].x;
                    game->bullets[i].prev_y = game->bullets[i].y;
                    game->bullets[i].active = 1;
                    game->bullets[i].is_player_bullet = 1;
                    game->bullets[i].type = BULLET_TYPE_HOMING;
                    game->bullets[i].target_enemy = find_closest_enemy(game, game->bullets[i].x, game->bullets[i].y);
                    game->bullets[i].dx = 0;
                    game->bullets[i].dy = -2;
                    game->active_bullets++;
                    break;
                }
            }
        }
    }

    // Update bullets
    game->active_bullets = 0;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            game->bullets[i].prev_x = game->bullets[i].x;
            game->bullets[i].prev_y = game->bullets[i].y;

            if (game->bullets[i].is_player_bullet) {
                // Update homing behavior for player bullets
                if (game->bullets[i].target_enemy >= 0 && 
                    game->enemies[game->bullets[i].target_enemy].active) {
                    int target_x = game->enemies[game->bullets[i].target_enemy].x + ENEMY_WIDTH/2;
                    int target_y = game->enemies[game->bullets[i].target_enemy].y + ENEMY_HEIGHT/2;
                    
                    if (game->bullets[i].x < target_x) game->bullets[i].dx += HOMING_SPEED;
                    if (game->bullets[i].x > target_x) game->bullets[i].dx -= HOMING_SPEED;
                    if (game->bullets[i].y < target_y) game->bullets[i].dy += HOMING_SPEED;
                    if (game->bullets[i].y > target_y) game->bullets[i].dy -= HOMING_SPEED;

                    if (game->bullets[i].dx > 2) game->bullets[i].dx = 2;
                    if (game->bullets[i].dx < -2) game->bullets[i].dx = -2;
                    if (game->bullets[i].dy > 2) game->bullets[i].dy = 2;
                    if (game->bullets[i].dy < -2) game->bullets[i].dy = -2;
                }

                // Move bullet
                game->bullets[i].x += game->bullets[i].dx;
                game->bullets[i].y += game->bullets[i].dy;

                // Check collision with enemies for player bullets only
                int hit_enemy = 0;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (game->enemies[j].active && 
                        check_bullet_enemy_collision(&game->bullets[i], &game->enemies[j])) {
                        // Clear the bullet's position before deactivating
                        LCD_Fill(game->bullets[i].x - 1, game->bullets[i].y - 1,
                                game->bullets[i].x + BULLET_WIDTH + 1,
                                game->bullets[i].y + BULLET_HEIGHT + 1, BLACK);
                        game->bullets[i].active = 0;
                        hit_enemy = 1;
                        break;
                    }
                }
                if (hit_enemy) continue;
            } else {
                // Update enemy bullets based on type
                switch (game->bullets[i].type) {
                    case BULLET_TYPE_NORMAL:
                        game->bullets[i].y += 1;
                        break;
                    case BULLET_TYPE_CROSS:
                        // Bounce around in a pattern
                        game->bullets[i].pattern_step++;
                        if (game->bullets[i].pattern_step >= 20) {
                            game->bullets[i].pattern_step = 0;
                            game->bullets[i].pattern_dir = (game->bullets[i].pattern_dir + 1) % 4;
                        }
                        switch (game->bullets[i].pattern_dir) {
                            case 0: game->bullets[i].x += 1; break;
                            case 1: game->bullets[i].y += 1; break;
                            case 2: game->bullets[i].x -= 1; break;
                            case 3: game->bullets[i].y -= 1; break;
                        }
                        break;
                    case BULLET_TYPE_CIRCLE:
                        // Move in a square pattern
                        game->bullets[i].pattern_step++;
                        if (game->bullets[i].pattern_step >= 40) {
                            game->bullets[i].active = 0;  // Deactivate after completing pattern
                            continue;
                        }
                        switch (game->bullets[i].pattern_step / 10) {
                            case 0: game->bullets[i].x += 1; break;
                            case 1: game->bullets[i].y += 1; break;
                            case 2: game->bullets[i].x -= 1; break;
                            case 3: game->bullets[i].y -= 1; break;
                        }
                        break;
                }
            }

            // Deactivate bullets that go off screen
            if (game->bullets[i].y < 0 || game->bullets[i].y > LCD_H ||
                game->bullets[i].x < 0 || game->bullets[i].x > LCD_W) {
                LCD_Fill(game->bullets[i].x - 2, game->bullets[i].y - 2,
                        game->bullets[i].x + BULLET_WIDTH + 2,
                        game->bullets[i].y + BULLET_HEIGHT + 2, BLACK);
                game->bullets[i].active = 0;
            } else {
                game->active_bullets++;
            }
        }
    }

    // Update enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            game->enemies[i].prev_x = game->enemies[i].x;
            game->enemies[i].prev_y = game->enemies[i].y;

            // Move enemies
            if (game->enemies[i].move_direction) {
                game->enemies[i].x += 1;
                if (game->enemies[i].x > LCD_W - ENEMY_WIDTH) {
                    game->enemies[i].move_direction = 0;
                }
            } else {
                game->enemies[i].x -= 1;
                if (game->enemies[i].x < 0) {
                    game->enemies[i].move_direction = 1;
                }
            }

            // Enemy shooting based on type
            if (rand() % 100 < 25 && game->active_bullets < MAX_BULLETS) {
                switch (game->enemies[i].type) {
                    case ENEMY_TYPE_NORMAL:
                        // Normal shooting
                        for (int j = 0; j < MAX_BULLETS; j++) {
                            if (!game->bullets[j].active) {
                                game->bullets[j].x = game->enemies[i].x + ENEMY_WIDTH / 2 - BULLET_WIDTH / 2;
                                game->bullets[j].y = game->enemies[i].y + ENEMY_HEIGHT;
                                game->bullets[j].prev_x = game->bullets[j].x;
                                game->bullets[j].prev_y = game->bullets[j].y;
                                game->bullets[j].active = 1;
                                game->bullets[j].is_player_bullet = 0;
                                game->bullets[j].type = BULLET_TYPE_NORMAL;
                                game->active_bullets++;
                                break;
                            }
                        }
                        break;
                    case ENEMY_TYPE_CROSS:
                        // Shoot cross pattern
                        for (int j = 0; j < MAX_BULLETS; j++) {
                            if (!game->bullets[j].active) {
                                game->bullets[j].x = game->enemies[i].x + ENEMY_WIDTH / 2;
                                game->bullets[j].y = game->enemies[i].y + ENEMY_HEIGHT;
                                game->bullets[j].prev_x = game->bullets[j].x;
                                game->bullets[j].prev_y = game->bullets[j].y;
                                game->bullets[j].active = 1;
                                game->bullets[j].is_player_bullet = 0;
                                game->bullets[j].type = BULLET_TYPE_CROSS;
                                game->bullets[j].pattern_step = 0;
                                game->bullets[j].pattern_dir = 0;
                                game->active_bullets++;
                                break;
                            }
                        }
                        break;
                    case ENEMY_TYPE_CIRCLE:
                        // Shoot circular pattern
                        for (int j = 0; j < MAX_BULLETS; j++) {
                            if (!game->bullets[j].active) {
                                game->bullets[j].x = game->enemies[i].x + ENEMY_WIDTH / 2;
                                game->bullets[j].y = game->enemies[i].y + ENEMY_HEIGHT;
                                game->bullets[j].prev_x = game->bullets[j].x;
                                game->bullets[j].prev_y = game->bullets[j].y;
                                game->bullets[j].active = 1;
                                game->bullets[j].is_player_bullet = 0;
                                game->bullets[j].type = BULLET_TYPE_CIRCLE;
                                game->bullets[j].pattern_step = 0;
                                game->active_bullets++;
                                break;
                            }
                        }
                        break;
                }
            }
        }
    }

    // Update FPS counter
    game->frame_count++;
    if (current_time - game->last_fps_update >= FPS_UPDATE_INTERVAL) {
        int time_diff = current_time - game->last_fps_update;
        if (time_diff > 0) {
            game->current_fps = (game->frame_count * 1000) / time_diff;
        }
        game->frame_count = 0;
        game->last_fps_update = current_time;
    }
}

void draw_game(GameState *game) {
    // Clear screen borders with extra padding
    LCD_Fill(0, LCD_H - 2, LCD_W, LCD_H, BLACK); // Bottom border
    LCD_Fill(LCD_W - 2, 0, LCD_W, LCD_H, BLACK); // Right border

    // Clear and draw bullets first
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            // Clear previous position with extra padding
            int clear_x1 = game->bullets[i].prev_x - 2;
            int clear_y1 = game->bullets[i].prev_y - 2;
            int clear_x2 = game->bullets[i].prev_x + BULLET_WIDTH + 2;
            int clear_y2 = game->bullets[i].prev_y + BULLET_HEIGHT + 2;

            // Ensure we don't clear the text area
            if (clear_y1 < 16) clear_y1 = 16;
            if (clear_y2 < 16) clear_y2 = 16;

            if (clear_x1 < 0) clear_x1 = 0;
            if (clear_x2 > LCD_W) clear_x2 = LCD_W;
            if (clear_y2 > LCD_H) clear_y2 = LCD_H;

            LCD_Fill(clear_x1, clear_y1, clear_x2, clear_y2, BLACK);

            // Draw bullet based on type
            if (game->bullets[i].is_player_bullet) {
                LCD_Fill(game->bullets[i].x, game->bullets[i].y,
                        game->bullets[i].x + BULLET_WIDTH,
                        game->bullets[i].y + BULLET_HEIGHT, BLUE);
            } else {
                switch (game->bullets[i].type) {
                    case BULLET_TYPE_NORMAL:
                        LCD_Fill(game->bullets[i].x, game->bullets[i].y,
                                game->bullets[i].x + BULLET_WIDTH,
                                game->bullets[i].y + BULLET_HEIGHT, RED);
                        break;
                    case BULLET_TYPE_CROSS:
                        draw_cross_bullet(game->bullets[i].x, game->bullets[i].y);
                        break;
                    case BULLET_TYPE_CIRCLE:
                        draw_circle_bullet(game->bullets[i].x, game->bullets[i].y);
                        break;
                }
            }
        }
    }

    // Clear and draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            // Ensure we don't clear the text area
            int clear_y1 = game->enemies[i].prev_y;
            if (clear_y1 < 16) clear_y1 = 16;

            LCD_Fill(game->enemies[i].prev_x, clear_y1,
                    game->enemies[i].prev_x + ENEMY_WIDTH,
                    game->enemies[i].prev_y + ENEMY_HEIGHT, BLACK);

            LCD_Fill(game->enemies[i].x, game->enemies[i].y,
                    game->enemies[i].x + ENEMY_WIDTH,
                    game->enemies[i].y + ENEMY_HEIGHT, GREEN);
        }
    }

    // Clear and draw player last
    LCD_Fill(game->player.prev_x, game->player.prev_y,
            game->player.prev_x + PLAYER_WIDTH,
            game->player.prev_y + PLAYER_HEIGHT, BLACK);

    LCD_Fill(game->player.x, game->player.y,
            game->player.x + PLAYER_WIDTH,
            game->player.y + PLAYER_HEIGHT, WHITE);

    // Draw FPS counter and entity counter
    static int last_fps = -1;
    static int last_bullets = -1;
    char info_str[40];
    sprintf(info_str, "F: %d B: %3d", game->current_fps, game->active_bullets);
    
    // Only clear and redraw text if values have changed
    if (last_fps != game->current_fps || last_bullets != game->active_bullets) {
        LCD_Fill(0, 0, 80, 16, BLACK);  // Clear text area
        LCD_ShowString(0, 0, (u8 *)info_str, WHITE);
        last_fps = game->current_fps;
        last_bullets = game->active_bullets;
    }
}