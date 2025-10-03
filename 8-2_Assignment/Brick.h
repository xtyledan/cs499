#pragma once

enum class BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum class ONOFF { ON, OFF };

class Brick {
public:
    Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh,
        float rr, float gg, float bb, int hits);

    void drawBrick() const;
    void hit();

    // accessors
    float x() const { return x_; }
    float y() const { return y_; }
    float width()  const { return width_; }
    float height() const { return height_; }
    BRICKTYPE type() const { return brick_type_; }
    ONOFF onoff() const { return onoff_; }

private:
    float red_, green_, blue_;
    float x_, y_, width_, height_;
    BRICKTYPE brick_type_;
    ONOFF onoff_;
    int hitCount_;
};
