enum Driving_State {
    standing = 0,
    accelerating,
    straight,
    breaking,
    left_turning,
    right_turning,
    left_rotating_90,
    right_rotating_90
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

enum Line_State {
    not_recognized = 0,
    recognized
};