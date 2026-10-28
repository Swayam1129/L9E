#ifndef CAMERA_H
#define CAMERA_H

#include "vector_util.h"
#include <SFML/System.hpp>

class Camera {
public:
    Camera(const sf::Vector2f& windowSize, const sf::Vector2f& viewCenterWorld,
           float worldUnitsPerPixel = 1.f)
        : mWindowSize{windowSize},
          mViewCenterWorld{viewCenterWorld},
          mWorldUnitsPerPixel{worldUnitsPerPixel} {}

    sf::Vector2f windowToWorld(const sf::Vector2f& pWindow) {
        // Could pull out the multiplication by mWorldUnitsPerPixel on the last two terms,
        // but left it like this for clarity
        //
        // Derivation:
        // Take center of view in world units, subtract half window size in world units
        // to get top-left in world units,
        // then add pWindow converted to world units.
        return mViewCenterWorld - ((mWindowSize * 0.5f) * mWorldUnitsPerPixel) +
               (pWindow * mWorldUnitsPerPixel);
    }

    sf::Vector2f worldToWindow(const sf::Vector2f& pWorld) {
        // Derivation:
        // Take center of view in world units, subtract half window size in world units
        // to get top-left in world units,
        // then subtract that from pWorld to get delta from top-left in world units,
        // finally convert to screen coords.
        return (pWorld - (mViewCenterWorld - ((mWindowSize * 0.5f) * mWorldUnitsPerPixel))) /
               mWorldUnitsPerPixel;
    }

    sf::Vector2f windowPercentToWorld(const sf::Vector2f& pWindowPercent) {
        return windowToWorld(pWindowPercent * mWindowSize);
    }

    sf::Vector2f worldToWindowPercent(const sf::Vector2f& pWorld) {
        return worldToWindow(pWorld) / mWindowSize;
    }

    void translateWorld(const sf::Vector2f& deltaWorld) { mViewCenterWorld += deltaWorld; }

    void translateWindow(const sf::Vector2f& deltaScreen) {
        mViewCenterWorld += deltaScreen * mWorldUnitsPerPixel;
    }

    void translateWindowPercent(const sf::Vector2f& deltaScreenPercent) {
        mViewCenterWorld += deltaScreenPercent * mWindowSize * mWorldUnitsPerPixel;
    }

    void setCamera(const sf::Vector2f& pWorld, const sf::Vector2f& pWindow) {
        // we just need to set a new view center such that
        //  (pWorld - (mViewCenterWorld - mWindowSize * 0.5 * mWorldUnitsPerPixel)) /
        //      mWorldUnitsPerPixel
        //   == pWindow
        // Solve for mViewCenterWorld:
        //  <=> pWindow * mWorldUnitsPerPixel = pWorld - (mViewCenterWorld - mWindowSize * 0.5 *
        //                                      mWorldUnitsPerPixel)
        //  <=> (mViewCenterWorld - mWindowSize * 0.5 * mWorldUnitsPerPixel) = pWorld - pWindow *
        //                                                                     mWorldUnitsPerPixel
        //  <=> mViewCenterWorld = pWorld - pWindow * mWorldUnitsPerPixel + mWindowSize * 0.5 *
        //                          mWorldUnitsPerPixel
        mViewCenterWorld =
            pWorld - pWindow * mWorldUnitsPerPixel + mWindowSize * 0.5f * mWorldUnitsPerPixel;
    }

    void setCameraPercent(const sf::Vector2f& pWorld, const sf::Vector2f& pWindowPercent) {
        return setCamera(pWorld, pWindowPercent * mWindowSize);
    }

    void setCameraCenter(const sf::Vector2f& pWorld) {
        // trick question -- they should not be calling the other setCamera* methods.
        mViewCenterWorld = pWorld;
    }

    sf::Vector2f getCameraCenter() const { return mViewCenterWorld; }

    // TODO NEW STUFF: Equivalent to above, but for vectors (distances for 1D) instead of points
    sf::Vector2f windowToWorldDist(const sf::Vector2f& vWindow) { return {}; };
    sf::Vector2f worldToWindowDist(const sf::Vector2f& vWorld) { return {}; };
    sf::Vector2f windowPercentToWorldDist(const sf::Vector2f& vWindowPercent) { return {}; };
    sf::Vector2f worldToWindowPercentDist(const sf::Vector2f& vWorld) { return {}; };
    float windowToWorldDist(float vWindow) { return 0.f; };
    float worldToWindowDist(float vWorld) { return 0.f; };

private:
    sf::Vector2f mWindowSize;       // size of the window in screen pixels (i.e. window coords)
    sf::Vector2f mViewCenterWorld;  // world coords point corresponding to the center of our window.
    float mWorldUnitsPerPixel;      // distance in world coords covered by one pixel
};

#endif  // CAMERA_H
