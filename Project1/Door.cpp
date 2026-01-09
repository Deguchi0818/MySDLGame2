#include "Door.h"

Door::Door(float x, float y, float w, float h, DoorColor color)
    : m_collider(x, y, w, h), m_color(color), m_state(DoorState::Closed)
{
    m_originalHeight = h;
    m_currentHeight = h;
}

Door::~Door()
{
    // 何もしなくてOK
}

void Door::onHit() {
    // 閉じている時だけ、開き始める
    if (m_state == DoorState::Closed) {
        m_state = DoorState::Opening;
        m_timer = 0.0f;
    }
}

void Door::update(float dt) 
{
    const float openSpeed = 0.3f;
    const float waitTime = 2.0f;

    switch (m_state) {
    case DoorState::Opening:
        m_timer += dt;
        // 徐々に高さを 0 に近づける
        m_currentHeight = m_originalHeight * (1.0f - (m_timer / openSpeed));

        if (m_timer >= openSpeed) {
            m_state = DoorState::Open;
            m_currentHeight = 0.0f;
            m_timer = 0.0f;
        }
        break;
    case DoorState::Open:
        m_timer += dt;
        if (m_timer >= waitTime) {
            m_state = DoorState::Closing;
            m_timer = 0.0f;
        }
        break;

    case DoorState::Closing:
        m_timer += dt;
        // 徐々に高さを元に戻す
        m_currentHeight = m_originalHeight * (m_timer / openSpeed);

        if (m_timer >= openSpeed) {
            m_state = DoorState::Closed;
            m_currentHeight = m_originalHeight;
            m_timer = 0.0f;
        }
        break;

    case DoorState::Closed:
        break;
    }

    // 当たり判定のサイズも見た目に合わせる
    // (これによって、開ききると当たり判定が消える)
    m_collider.setSize(m_collider.rect().w, m_currentHeight);
}