#include <aabbtree.hpp>

#include <raylib.h>

#include <iterator>
#include <unordered_set>
#include <vector>

using namespace mia;

void debug_boxes_helper_recursive(std::vector<std::pair<AABB2f, usize>> *boxes, AABBTreeNode *cur_node, usize depth) {
    if (cur_node == nullptr) return;
    boxes->emplace_back(std::make_pair(cur_node->bound, depth));

    if (!cur_node->childs[0] || !cur_node->childs[1]) return;
    debug_boxes_helper_recursive(boxes, cur_node->childs[0], depth + 1);
    debug_boxes_helper_recursive(boxes, cur_node->childs[1], depth + 1);
}

void handle_aabb_hit_bounds(AABB2f *aabb, Vec2f *velocity, float screen_width, float screen_height) {
    float speed = vector::length(*velocity);
    bool hit_edge = false;

    if (aabb->min.x < 0) {
        aabb->max.x += -aabb->min.x;
        aabb->min.x = 0;
        hit_edge = true;
    } else if (aabb->max.x > screen_width) {
        aabb->min.x -= (aabb->max.x - screen_width);
        aabb->max.x = screen_width;
        hit_edge = true;
    }

    if (aabb->min.y < 0) {
        aabb->max.y += -aabb->min.y;
        aabb->min.y = 0;
        hit_edge = true;
    } else if (aabb->max.y > screen_height) {
        aabb->min.y -= (aabb->max.y - screen_height);
        aabb->max.y = screen_height;
        hit_edge = true;
    }

    if (hit_edge) {
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        velocity->x = cos(angle) * speed;
        velocity->y = sin(angle) * speed;
    }
}

int main(void) {
    AABBTree tree{};
    tree.margin = 8.0;

    constexpr usize cnt = 10000;
    std::unordered_set<usize> removed_indices;
    AABB2f aabbs[cnt];
    Vec2f aabb_vecs[cnt];

    InitWindow(1600, 900, "aabbtree demo");
    SetTargetFPS(60);

    for (auto i = 0U; i < cnt; i++) {
        float minX = GetRandomValue(0 + 1, 1600 - 20 - 1);
        float minY = GetRandomValue(0 + 1, 900 - 20 - 1);
        float maxX = minX + GetRandomValue(3, 20);
        float maxY = minY + GetRandomValue(3, 20);

        aabbs[i].min = {minX, minY};
        aabbs[i].max = {maxX, maxY};

        aabbtree::insert(&tree, &aabbs[i]);
    }

    for (auto i = 0U; i < cnt; i++) {
        float speed = GetRandomValue(0, 30);
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        aabb_vecs[i].x = cos(angle) * speed;
        aabb_vecs[i].y = sin(angle) * speed;
    }

    // Color debug_box_color[7] = {PURPLE, PINK, RED, ORANGE, YELLOW, LIME, GREEN};

    SetTargetFPS(144);

    usize frame_count = 0;
    bool remove_mode = true; // true: remove each frame, insert each 6 frames
                             // false: remove each 6 frames, insert each frame

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        frame_count++;

        std::vector<usize> active_indices;
        for (usize i = 0; i < cnt; i++) {
            if (removed_indices.find(i) == removed_indices.end()) {
                active_indices.push_back(i);
            }
        }

        // Reverse mode if no active AABBs or all are active
        if (active_indices.empty() && remove_mode) {
            remove_mode = false;
        } else if (removed_indices.empty() && !remove_mode) {
            remove_mode = true;
        }

        // Remove or insert based on mode and frame count
        if (remove_mode) {
            // Remove each frame, insert each 6 frames
            if (!active_indices.empty() && frame_count % 1 == 0) {
                for (auto _ = 0; _ < 12; _++) {
                    usize random_idx = active_indices[GetRandomValue(0, active_indices.size() - 1)];
                    aabbtree::remove(&tree, &aabbs[random_idx]);
                    removed_indices.insert(random_idx);
                }
            }
            if (!removed_indices.empty() && frame_count % 2 == 0) {
                auto it = removed_indices.begin();
                if (it == nullptr) continue;
                std::advance(it, GetRandomValue(0, removed_indices.size() - 1));
                if (it == nullptr) continue;
                usize idx = *it;
                aabbtree::insert(&tree, &aabbs[idx]);
                removed_indices.erase(it);
            }
        } else {
            // Remove each 6 frames, insert each frame
            if (!active_indices.empty() && frame_count % 2 == 0) {
                usize random_idx = active_indices[GetRandomValue(0, active_indices.size() - 1)];
                aabbtree::remove(&tree, &aabbs[random_idx]);
                removed_indices.insert(random_idx);
            }
            if (!removed_indices.empty() && frame_count % 1 == 0) {
                for (auto _ = 0; _ < 12; _++) {
                    auto it = removed_indices.begin();
                    if (it == nullptr) continue;
                    std::advance(it, GetRandomValue(0, removed_indices.size() - 1));
                    if (it == nullptr) continue;
                    usize idx = *it;
                    aabbtree::insert(&tree, &aabbs[idx]);
                    removed_indices.erase(it);
                }
            }
        }

        for (auto i = 0U; i < cnt; i++) {
            Vec2f displacement = aabb_vecs[i] * dt;
            aabbs[i].min += displacement;
            aabbs[i].max += displacement;

            handle_aabb_hit_bounds(&aabbs[i], &aabb_vecs[i], 1600, 900);
        }

        aabbtree::update(&tree);
        auto pair_list = aabbtree::get_collided_pairs(&tree);
        // auto pair_list = AABBPairList{};

        // std::vector<std::pair<AABB2f, usize>> debug_boxes;
        // debug_boxes_helper_recursive(&debug_boxes, tree.root, 0);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // for (const auto &[aabb, depth] : debug_boxes) {
        //     Color border_color = debug_box_color[depth % 7];
        //     Color fill_color = border_color;
        //     fill_color.a = 10;
        //
        //     f32 offset_scale = 0;
        //     Rectangle rect = {aabb.min.x - depth * offset_scale, aabb.min.y - depth * offset_scale,
        //                       aabb.max.x - aabb.min.x + depth * offset_scale * 2, aabb.max.y - aabb.min.y + depth * offset_scale *
        //                       2};
        //
        //     DrawRectangle(rect.x, rect.y, rect.width, rect.height, fill_color);
        //     DrawRectangleLinesEx(rect, 1.0, border_color);
        // }

        for (auto i : active_indices) {
            DrawRectangle(aabbs[i].min.x, aabbs[i].min.y, aabbs[i].max.x - aabbs[i].min.x, aabbs[i].max.y - aabbs[i].min.y, BLUE);
            DrawRectangleLines(aabbs[i].min.x, aabbs[i].min.y, aabbs[i].max.x - aabbs[i].min.x, aabbs[i].max.y - aabbs[i].min.y, BLACK);
        }

        for (cauto &pair : pair_list) {
            Color color = RED;
            color.a = 200;
            DrawRectangle(pair.first->min.x, pair.first->min.y, pair.first->max.x - pair.first->min.x,
                          pair.first->max.y - pair.first->min.y, color);
            DrawRectangle(pair.second->min.x, pair.second->min.y, pair.second->max.x - pair.second->min.x,
                          pair.second->max.y - pair.second->min.y, color);
        }
        arrlist::free(&pair_list);

        // Query AABB around cursor position and highlight detected AABBs
        Vector2 mouse_pos = GetMousePosition();
        constexpr float query_half_size = 50.0f;
        AABB2f query_region = {{mouse_pos.x - query_half_size, mouse_pos.y - query_half_size},
                               {mouse_pos.x + query_half_size, mouse_pos.y + query_half_size}};
        auto queried_aabbs = aabbtree::query_aabb(&tree, query_region);
        for (usize i = 0; i < queried_aabbs.count; i++) {
            AABB2f *aabb = queried_aabbs[i];
            DrawRectangle(aabb->min.x, aabb->min.y, aabb->max.x - aabb->min.x, aabb->max.y - aabb->min.y, PURPLE);
            DrawRectangleLines(aabb->min.x, aabb->min.y, aabb->max.x - aabb->min.x, aabb->max.y - aabb->min.y, WHITE);
        }
        arrlist::free(&queried_aabbs);

        // Draw the query region outline
        DrawRectangleLinesEx(
            {query_region.min.x, query_region.min.y, query_region.max.x - query_region.min.x, query_region.max.y - query_region.min.y},
            2.0f, DARKGREEN);

        DrawRectangle(0, 0, 120, 40, WHITE);
        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
