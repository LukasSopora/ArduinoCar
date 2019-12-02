enum Driving_State {
    standing = 0,
    accelerating,
    backwarding, /*muss aus irgendwinem grund drin bleiben --> Arduino IDE ist kacke*/
    straight,
    left_cornering, /*same here...*/
    right_cornering /*same here...*/
};

enum Dodge_State {
    dodge_NONE = 0,
    dodge_right_step_1,
    dodge_right_step_2,
    dodge_right_step_3,
    dodge_left_step_1,
    dodge_left_step_2,
    dodge_left_step_3
};

enum Object_State {
    nothing = 0,
    front_left,
    front_center,
    front_right,
    left,
    right,
    back
};

enum Colors {
    color_NONE = 0,
    color_red,
    color_blue
};
