enum Driving_State {
    standstill = 0,
    acceleration,
    strait,
    breaking,
    left_corner,
    right_corner
};

enum Object_State {
    nothing = 0,
    front,
    left,
    right,
    back
};

enum Line_State {
    not_recognized = 0,
    recognized
};