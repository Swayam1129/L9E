#ifndef DYNAMIC_CAMERA_H
#define DYNAMIC_CAMERA_H

#include <iostream>
#include <limits>

#include "camera.h"

constexpr float INF = std::numeric_limits<float>::infinity();

// AABB in 1D
struct Interval {
    float min;
    float max;

    constexpr bool contains(const Interval& other) const {
        return (min <= other.min) && (max >= other.max);
    }

    constexpr bool contains(float x) const { return (min <= x) && (x <= max); }

    // signedDistance returns a positive smallest distance of x from the interval boundary
    // if it is not contained, else it returns a smallest (in magnitude) negative distance
    // from the interval boundary.
    constexpr float signedDistance(float x) const {
        if (contains(x)) {
            // 0 or negative
            return std::max(x - max, min - x);
        }
        // positive
        return std::max(x - max, min - x);
    }
};

struct DynamicCamera {
    void update(Camera& camera,                           // the camera to update
                const sf::Vector2f& trackedPosWorld,      // world coords of player to track
                const sf::Vector2f& trackedVelocityWorld  // world velocity of player to track
    ) {
        // 1. Measure the player relative to the UNADJUSTED camera (no lookahead),
        //    so the zones behave exactly like the prep.
        camera.setCameraCenter(cameraUnadjustedCenter);
        float x = camera.worldToWindowPercent(trackedPosWorld).x;  // player x in [0,1] of window
        float vx = trackedVelocityWorld.x;

        // 2. Target lookahead in WORLD units: +lookAhead% of width if moving right,
        //    -lookAhead% if moving left, 0 if stopped.
        float lookAheadWorld = camera.windowPercentToWorldDist({lookAhead, 0.f}).x;
        float targetLookahead = 0.f;
        if (vx > 0) {
            targetLookahead = lookAheadWorld;
        } else if (vx < 0) {
            targetLookahead = -lookAheadWorld;
        }

        // 3. EMA so the lookahead slides in/out smoothly instead of snapping.
        lookaheadState = alpha * targetLookahead + (1.f - alpha) * lookaheadState;

        // 4. Dead / soft / hard zone -> how far to move the unadjusted camera (window percent).
        float deltaPercent = 0.f;
        if (deadZoneX.contains(x)) {
            // dead zone: camera doesn't move
            deltaPercent = 0.f;
        } else {
            // signed distance past the dead-zone edge (+ = right of it, - = left of it)
            float overDead = (x > deadZoneX.max) ? x - deadZoneX.max : x - deadZoneX.min;

            if (softZoneX.contains(x)) {
                // soft zone: ease toward putting the player back on the dead-zone edge
                deltaPercent = alpha * overDead;
            } else {
                // hard zone: immediately push back to the hard line (player may never pass it),
                // then ease the remaining soft-zone width like above
                float hardEdge = (x > softZoneX.max) ? softZoneX.max : softZoneX.min;
                float deadEdge = (x > deadZoneX.max) ? deadZoneX.max : deadZoneX.min;
                deltaPercent = (x - hardEdge) + alpha * (hardEdge - deadEdge);
            }
        }
        float deltaWorld = camera.windowPercentToWorldDist({deltaPercent, 0.f}).x;

        // 5. Displayed camera = unadjusted (moved) + smoothed lookahead.
        sf::Vector2f newUnadjusted = cameraUnadjustedCenter + sf::Vector2f{deltaWorld, 0.f};
        camera.setCameraCenter(newUnadjusted + sf::Vector2f{lookaheadState, 0.f});

        // 6. Remember the center WITHOUT lookahead for next frame.
        cameraUnadjustedCenter = newUnadjusted;
    }

    float alpha;  // alpha to use for damping of camera position, *as well as* for smoothing of
                  // lookahead offset.
    Interval deadZoneX;  // percentage interval along x-axis to use for dead zone in [0, 1]
    // percentage interval along x-axis to use for soft rejection zone in [0, 1]
    Interval softZoneX = {-INF, INF};
    // (everything beyond softZone is considered hard zone).
    float lookAhead;  // screen-percentage lookahead
    mutable sf::Vector2f
        cameraUnadjustedCenter;      // the camera world center from last frame, ignoring lookahead
    mutable float lookaheadState{};  // the EMA of the accumulated lookahead offset in world coords
};

#endif // DYNAMIC_CAMERA_H
