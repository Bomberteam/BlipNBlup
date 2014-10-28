#ifndef PLAYER_H
#define PLAYER_H

#include <gameobject.h>
#include <bubble.h>

class Player : public GameObject
{
    Q_OBJECT
public:
    explicit Player(bool blup);
private:
    bool m_pressed_up;
    bool m_pressed_down;
    bool m_pressed_left;
    bool m_pressed_right;
    bool m_pressed_fire;

    int  m_jump;
    double m_walk_speed;

    enum Animation{Stand, Walk, Jump, Fire, Dead};
    Animation current_animation;

    //QSound jumpSound;
    QSound shootSound;

public slots:
    bool GetUp()    const noexcept;
    bool GetDown()  const noexcept;
    bool GetLeft()  const noexcept;
    bool GetRight() const noexcept;
    bool GetFire()  const noexcept;

    void SetUp(bool state)      noexcept;
    void SetDown(bool state)    noexcept;
    void SetLeft(bool state)    noexcept;
    void SetRight(bool state)   noexcept;
    void SetFire(bool state)    noexcept;

    void ApplyKeys(std::vector<Bubble *> &bubbles) noexcept;
    void ApplyMovement(const QPixmap& background);
    Bubble * Shoot();


    void Animate() noexcept;
};

#endif // PLAYER_H
