#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H
#include <ostream>

#include <SFML/System.hpp>
template <typename T>
sf::Vector2<T> operator*(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
    return {a.x * b.x, a.y * b.y};
}

template <typename T>
sf::Vector2<T> operator/(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
    assert(b.x != 0);
    assert(b.y != 0);
    return {a.x / b.x, a.y / b.y};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
    return os << "Vector2{ " << v.x << ", " << v.y << " }";
}

#endif  // VECTOR_UTIL_H
