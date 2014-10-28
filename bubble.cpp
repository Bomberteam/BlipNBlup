#include "bubble.h"
#include <wasp.h>
#include <QPainter>
#include <cmath>

#include <qdebug.h>

Bubble::Bubble() :
    GameObject(),
    m_age(0)
{
    SetWidth(32);
    SetHeight(32);
    int sprite_height = 64;
    SetHitdX(GetWidth()/2);
    SetHitdY(GetHeight()/2);
    QImage bubble_image = QImage(":/graphics/bubbles.png");
    for (int i = 0; i < 2; i++){
        AddSprite(bubble_image.copy(
                        i*sprite_height, 0,
                        sprite_height, sprite_height)
        );
    }
}

//AGE BUBBLE AND RETURN TRUE ON OLD
bool Bubble::Age() noexcept{
    ++m_age;
    if (m_age > 176-(Randomize() / 128) ) {
        SetCurrentFrame(1);
        SetWidth(GetWidth()+2);
        SetHeight(GetHeight()+2);
    }
    if (m_age > 180-(Randomize() / 128) ) {

        return true;
    }
    else return false;
}

//MOVE BUBBLE
void Bubble::ApplyMovement(long ticks, const std::vector<double>& sine) noexcept
{
    if ( GetYSpeed() > -2.5 ){ SetYSpeed( GetYSpeed() - 0.05 );}
    SetXSpeed(GetXSpeed() * 0.96);

    int phase = floor( 3 * ticks + Randomize() );
    SetX( GetX() + GetXSpeed() + sine[phase%512] );
    SetY( GetY() + GetYSpeed() );
}

//DRAW BUBBLE
void Bubble::Draw(QPainter * const painter, const int ticks, const std::vector<double>& sine) noexcept
{
    int phase = 3 * ticks + Randomize();
    QImage sprite = GetSprite(GetCurrentFrame());
    const double width  = GetWidth()  - abs(GetXSpeed())/2 + (2*sine[phase%512]);
    const double height = GetHeight() + abs(GetXSpeed())/4 + (2*sine[phase%512]);
    const double dx = (GetWidth() - width)  / 2.0;
    const double dy = (GetHeight() - height) / 2.0;

    QPixmap flip = QPixmap::fromImage(sprite.mirrored(!IsFacingRight(), false));
    painter->drawPixmap( GetX()+dx, GetY()+dy, width, height, flip);
    //WRAPPING
    if( GetX() <   0)          painter->drawPixmap( GetX() + dx + 400, GetY() + dy      , width, height, flip);
    if( GetX() > 400 - width)  painter->drawPixmap( GetX() + dx - 400, GetY() + dy      , width, height, flip);
    if( GetY() <   0)          painter->drawPixmap( GetX() + dx      , GetY() + dy + 300, width, height, flip);
    if( GetY() > 300 - height) painter->drawPixmap( GetX() + dx      , GetY() + dy - 300, width, height, flip);

    if( GetX() <   0         && GetY() <   0)          painter->drawPixmap( GetX() + dx + 400, GetY() + dy + 300, width, height, flip);
    if( GetX() > 400 - width && GetY() <   0)          painter->drawPixmap( GetX() + dx - 400, GetY() + dy + 300, width, height, flip);
    if( GetX() > 400 - width && GetY() > 300 - height) painter->drawPixmap( GetX() + dx - 400, GetY() + dy - 300, width, height, flip);
    if( GetX() <   0         && GetY() > 300 - height) painter->drawPixmap( GetX() + dx + 400, GetY() + dy - 300, width, height, flip);
}
