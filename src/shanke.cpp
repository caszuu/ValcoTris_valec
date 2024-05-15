#include <Logic.hpp>
#include <cstdint>
#include <random>
#include <vector>

// #include "esp_now.h"

extern int brightness;
namespace shanke {
struct Vec2 {
    int32_t x, y;

    constexpr bool operator==(const Vec2& o) const noexcept { return x == o.x && y == o.y; }
};

constexpr static Vec2 default_head = { .x = 6, .y = 5 };

class ShankeState {
public:
    ShankeState() noexcept
        : shanke_parts { { default_head } }
        , shanke_head { default_head } {
        shanke_state = this;

        shanke_parts.resize(1, default_head);
        gen_food();
    }

    ~ShankeState() noexcept = default;

    void loop() {
        while (true) {
            // apply inputs

            shanke_dir = next_shanke_dir;

            // step and check

            switch (shanke_dir) {
            case 0:
                shanke_head.y++;
                break;

            case 1:
                shanke_head.x++;
                break;

            case 2:
                shanke_head.y--;
                break;

            case 3:
                shanke_head.x--;
                break;
            };

            if (shanke_head.x < 0) {
                shanke_head.x = map_size.x;
            } else if (shanke_head.x > map_size.x) {
                shanke_head.x = 0;
            }

            if (shanke_head.y < 0 || shanke_head.y > map_size.y) {
                return;
            }

            // check for shanke

            for (Vec2 part : shanke_parts) {
                if (part == shanke_head) {
                    return; // fail
                }
            }

            if (food_pos == shanke_head) {
                // extend forward
                shanke_parts.insert(shanke_parts.begin(), shanke_head);
                score++;

                gen_food();
            } else {
                // move snake parts
                std::copy(shanke_parts.begin(), shanke_parts.end() - 1, shanke_parts.begin() + 1);
                shanke_parts[0] = shanke_head;
            }
            draw();
            // TODO: delay
        }
    }

    void gen_food() noexcept {
        while (true) {
            food_pos = Vec2 { rand() % map_size.x, rand() % map_size.y };

            for (Vec2 part : shanke_parts) {
                if (part == shanke_head) {
                    continue; // food_pos is in shanke, reroll
                }
            }

            return;
        }
    }

    void right() noexcept {
        if (++next_shanke_dir % 4 == (shanke_dir + 2) % 4)
            --next_shanke_dir;
        next_shanke_dir %= 4;
    }

    void left() noexcept {
        if (--next_shanke_dir % 4 == (shanke_dir + 2) % 4)
            --next_shanke_dir;
        next_shanke_dir %= 4;
    }

    void draw() const noexcept {
        display.clear();

        for (Vec2 part : shanke_parts) {
            display.setColor(part.x, part.y, Rgb(0, 255, 50));
        }
        display.setColor(food_pos.x, food_pos.y, Rgb(255, 0, 0));

        display.show(brightness);
    }

    static ShankeState& getShankeState() noexcept { return *shanke_state; }

private:
    static ShankeState* shanke_state;

    std::vector<Vec2> shanke_parts;

    Vec2 shanke_head;
    Vec2 food_pos;
    constexpr static Vec2 map_size = Vec2 { .x = 12, .y = 10 };

    int32_t shanke_dir = 1;
    int32_t next_shanke_dir = 1;
    uint32_t score = 0;
};

ShankeState* ShankeState::shanke_state = nullptr;
}

int shanke_main() {
        shanke::ShankeState game_state;

        game_state.loop();
        return 1; // score
}

void shanke_left() noexcept {
    shanke::ShankeState::getShankeState().left();
}

void shanke_right() noexcept {
    shanke::ShankeState::getShankeState().right();
}