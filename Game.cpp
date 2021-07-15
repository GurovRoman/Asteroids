#define _USE_MATH_DEFINES
#include "Engine.h"
#include "Actor.h"
#include "Util.h"
#include <cstdlib>
#include <list>
#include <algorithm>


//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()


Actor player;
std::list<Actor> bullets;
std::list<std::pair<size_t, Actor>> asteroids;  // Size, Actor

bool shot = false;
float invul_time = 0.f;
size_t lives = 3;
float flash_timer = 0.f;

const std::vector<Vector> bullet_model = { {-1, 0}, {1, 0} };
const std::vector<Vector> player_model = { {5, 0}, {-4, -3}, {-4, 3} };

const std::vector<float> asteroid_radiuses = { 20, 40, 60 };

Actor& spawn_asteroid(size_t size) {
    asteroids.emplace_back(size, Actor{});
    auto& asteroid = asteroids.back().second;
    asteroid.radius = asteroid_radiuses[size];

    asteroid.pos = { RandFloat(0, SCREEN_WIDTH), RandFloat(0, SCREEN_HEIGHT) };

    auto speed = RandFloat(50, 200);
    auto direction = RandFloat(0, M_PI * 2);

    asteroid.velocity.x = cos(direction) * speed;
    asteroid.velocity.y = -sin(direction) * speed;

    asteroid.angle = direction * 180 / M_PI;

    std::vector<Vector> model;

    const size_t vertex_count = 10;

    for (size_t i = 0; i < vertex_count; ++i) {
        float angle = i * 2 * M_PI / vertex_count + RandFloat(0, 2 * M_PI / vertex_count);
        float dist_mult = RandFloat(0.9, 1.1);
        model.push_back({ cosf(angle) * asteroid.radius * dist_mult, -sinf(angle) * asteroid.radius * dist_mult });
    }

    asteroid.SetModel(model);

    return asteroid;
}

void split_asteroid(const Actor& asteroid, size_t debris_size) {
    const size_t split_count = 3;
    const float debris_speed = 100;
    for (size_t i = 0; i < split_count; ++i) {
        auto& debris = spawn_asteroid(debris_size);

        auto direction = asteroid.angle * M_PI / 180 + (i * 2 * M_PI / split_count);

        debris.pos = asteroid.pos;
        debris.pos.x += cos(direction) * asteroid.radius / 2;
        debris.pos.y -= sin(direction) * asteroid.radius / 2;

        debris.velocity = asteroid.velocity;
        debris.velocity.x += cos(direction) * debris_speed;
        debris.velocity.y -= sin(direction) * debris_speed;
    }
}


void initialize() {
    player = {};
    bullets.clear();
    asteroids.clear();
    lives = 3;
    invul_time = 3;

    player.pos = { SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 };
    player.acceleration = 100;
    player.max_velocity = 400;
    player.SetModel(player_model);
    
    for (size_t i = 0; i < 10; ++i) {
        size_t size;
        if (i < 3) {
            size = 0;
        } else if (i < 6) {
            size = 1;
        } else {
            size = 2;
        }
        spawn_asteroid(size);
    }
}


void act(float dt) {
    // advance flash timer
    flash_timer += dt;

    // Key presses
    if (is_key_pressed(VK_ESCAPE)) {
        schedule_quit_game();
    }
    if (is_key_pressed(VK_UP)) {
        player.AccelerateStep(dt);
    }
    if (is_key_pressed(VK_LEFT)) {
        player.angle += 90 * dt;
    }
    if (is_key_pressed(VK_RIGHT)) {
        player.angle -= 90 * dt;
    }
    if (is_key_pressed(VK_SPACE)) {
        if (lives == 0 || asteroids.size() == 0) {
            initialize();
            shot = true;
            return;
        }
        if (!shot && bullets.size() < 10) {
            // spawn bullet
            bullets.emplace_back();
            auto& bullet = bullets.back();

            bullet.SetModel(bullet_model);
            bullet.pos = player.pos;
            bullet.angle = player.angle;

            auto speed = player.velocity.Length();

            float angle_ = bullet.angle * M_PI / 180;
            bullet.velocity.x = cos(angle_) * 200 + player.velocity.x;
            bullet.velocity.y = -sin(angle_) * 200 + player.velocity.y;

            // make bullets spawn at ship's nose
            bullet.pos.x += cos(angle_) * 6;
            bullet.pos.y -= sin(angle_) * 6;
        }
        shot = true;
    } else {
        shot = false;
    }

    // collide bullets
    bullets.remove_if([](const Actor& bullet) {
        for (auto as_it = asteroids.begin(); as_it != asteroids.end(); ++as_it) {
            if (PointInSphere(bullet.pos, as_it->second.pos, as_it->second.radius)) {
                if (as_it->first > 0) {
                    split_asteroid(as_it->second, as_it->first - 1);
                }
                asteroids.erase(as_it);
                return true;
            }
        }
        return false;
    });


    // handle invulnerability timer
    invul_time = std::max(0.f, invul_time - dt);

    // collide player if alive
    if (lives > 0 && invul_time == 0.f) {
        for (auto as_it = asteroids.begin(); as_it != asteroids.end(); ++as_it) {
            if (PointInSphere(player.pos, as_it->second.pos, as_it->second.radius)) {
                if (as_it->first > 0) {
                    split_asteroid(as_it->second, as_it->first - 1);
                }
                asteroids.erase(as_it);

                // kill and respawn player
                --lives;

                player.pos = { SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 };
                player.velocity = {};
                invul_time = 5.f;

                break;
            }
        }
    }


    // Move every actor
    player.MoveStep(dt);
    for (auto& bullet : bullets) {
        bullet.MoveStep(dt);
    }
    for (auto& asteroid : asteroids) {
        asteroid.second.MoveStep(dt);
    }
}


void draw() {
    // clear backbuffer
    clear_buffer();

    if (lives > 0 && fmodf(invul_time, 0.5f) < 0.25f) {
        player.Draw();
    }

    for (const auto& bullet : bullets) {
        bullet.Draw();
    }
    for (const auto& asteroid : asteroids) {
        asteroid.second.Draw();
    }

    // draw interface
    Actor live;
    live.SetModel(player_model);
    live.angle = 60;
    live.scale = 3;
    live.pos.y = 30;
    for (size_t i = 0; i < lives; ++i) {
        live.pos.x = 30 + 30 * i;
        live.Draw();
    }

    // draw death or win screen
    if (fmodf(flash_timer, 1.f) < 0.5f) return;

    if (lives == 0) {
        float size = 200;
        Vector center = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
        DrawLine(center.x - size, center.y - size, center.x + size, center.y + size, 0xFF0000);
        DrawLine(center.x - size, center.y + size, center.x + size, center.y - size, 0xFF0000);
    }
    else if (asteroids.size() == 0) {
        float size = 200;
        Vector center = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
        DrawLine(center.x - size / 1.5f, center.y, center.x, center.y + size, 0xFF00);
        DrawLine(center.x, center.y + size, center.x + size, center.y - size, 0xFF00);

    }
}


void finalize() {
}

