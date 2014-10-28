#include "player.h"
#include "bubble.h"
#include <stdio.h>

Player::Player(bool blup) :
    GameObject(),
    m_pressed_up(false), m_pressed_down(false),
    m_pressed_left(false), m_pressed_right(false),
    m_pressed_fire(false),
    m_jump(10), m_walk_speed(1.8),

    current_animation(Stand),
    //jumpSound("://audio/jump.wav"),
    shootSound("://audio/shootbubble.wav")
{
    int sprite_size = 64;
    SetHitdX(GetWidth()/2);
    SetHitdY(GetHeight()/2);
    QImage playerImage = QImage(":/graphics/blipnblup.png");
    for (int i = 0; i < (playerImage.width()/sprite_size); i++){
        AddSprite(playerImage.copy(
                      i*sprite_size, blup*sprite_size,
                      sprite_size, sprite_size));
    }
}

bool Player::GetUp()    const noexcept  {return m_pressed_up;}
bool Player::GetDown()  const noexcept  {return m_pressed_down;}
bool Player::GetLeft()  const noexcept  {return m_pressed_left;}
bool Player::GetRight() const noexcept  {return m_pressed_right;}
bool Player::GetFire()  const noexcept  {return m_pressed_fire;}

void Player::SetUp(bool state) noexcept     {m_pressed_up    = state;}
void Player::SetDown(bool state) noexcept   {m_pressed_down  = state;}
void Player::SetLeft(bool state) noexcept   {m_pressed_left  = state;}
void Player::SetRight(bool state) noexcept  {m_pressed_right = state;}
void Player::SetFire(bool state) noexcept   {m_pressed_fire  = state;}

//SHOOT BUBBLE
Bubble * Player::Shoot()
{
    shootSound.play();
    ResetCharge();
    Bubble * bubble = new Bubble();
    bubble->SetX(GetX());
    bubble->SetY(GetY());
    bubble->SetYSpeed(0);
    bubble->SetXSpeed( (10*(-1+2*IsFacingRight() ))+(3*GetRight())-(3*GetLeft()) );
    return bubble;
}

//APPLY INPUT EFFECTS
void Player::ApplyKeys(std::vector<Bubble*> &bubbles) noexcept{
    if (IsAlive()){
        //JUMP
        if (GetUp() && IsOnGround()){
            //jumpSound.play(); //Too square
            current_animation = Jump;
            SetOnGround(false);
            SetYSpeed(-m_jump);
        }
        //FORCE LEFT
        if (GetLeft()){
            FaceLeft();
            if (GetXSpeed() > -m_walk_speed){ SetXSpeed( GetXSpeed() - (0.25 + (0.75*IsOnGround())) );}
        }
        //FORCE RIGHT
        if (GetRight()){
            FaceRight();
            if (GetXSpeed() <  m_walk_speed){ SetXSpeed( GetXSpeed() + (0.25 + (0.75*IsOnGround())) );}
        }
        //SHOOT
        if (GetFire() && GetChargeAmount() > 1.0){
            current_animation = Fire;
            bubbles.push_back(Shoot());
        }
    }
    else current_animation = Dead;
}

//MOVE PLAYER
void Player::ApplyMovement(const QPixmap& background)
{
    int check_x = (GetX()+(GetWidth()/2));
    int check_y = (GetY()+GetHeight()-4);
    //COLLISION DETECTION TO THE LEFT
    if (GetXSpeed() < 0){
        for(int i = 0; i < -GetXSpeed(); ++i){
            if (--check_x < 0) check_x += background.width();
            if (check_x > background.width()-1) check_x -= background.width();
            if (--check_y < 0) check_y += background.height();
            if (check_y > background.height()-1) check_y -= background.height();
            const QRgb sidepixel = background.toImage().pixel(check_x, check_y);
            const int red = qRed(sidepixel);
            if ((!red % 2)){
                SetX(GetX()-i);
                SetXSpeed(0);
            }
        }
    }

    //COLLISION DETECTION TO THE RIGHT
    if (GetXSpeed() > 0) {
        for(int i = 0; i < GetXSpeed(); ++i){
            if (++check_x < 0) check_x += background.width();
            if (check_x > background.width()-1) check_x -= background.width();
            if (--check_y < 0) check_y += background.height();
            if (check_y > background.height()-1) check_y -= background.height();
            const QRgb sidepixel = background.toImage().pixel(check_x, check_y);
            const int red = qRed(sidepixel);
            if ((!red % 2)){
                SetX(GetX()+i);
                SetXSpeed(0);
            }
        }
    }

    SetX(GetX()+GetXSpeed());
    SetY(GetY()+GetYSpeed());

    //SLOW DOWN
    if (IsOnGround()){ SetXSpeed(GetXSpeed() * 0.7); }
}

//PLAY ANIMATION
void Player::Animate() noexcept{
    if (IsAlive()){
        double abs_x_speed = GetXSpeed();
        if (abs_x_speed < 0) abs_x_speed = -abs_x_speed;
        m_animframe += (abs_x_speed/8.0);

        if (current_animation == Fire){
            SetCurrentFrame(Fire);
            if (GetChargeAmount() > 0.3) current_animation = Stand;
        }
        else if (IsOnGround()){
            if (abs_x_speed < 0.5) current_animation = Stand;
            else current_animation = Walk;
        }

        if (current_animation == Jump)  SetCurrentFrame(2);
        if (current_animation == Stand) SetCurrentFrame(Stand);
        if (current_animation == Walk){
            if (m_animframe >= 4) m_animframe = 0;
            switch ((int)floor((double)m_animframe)){
                case 0 : SetCurrentFrame(0); break;
                case 1 : SetCurrentFrame(1); break;
                case 2 : SetCurrentFrame(0); break;
                case 3 : SetCurrentFrame(2); break;
                default: SetCurrentFrame(0); break;
            }
        }
    }
    else SetCurrentFrame(Dead);
}
