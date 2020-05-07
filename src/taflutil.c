#include "taflutil.h"

uint8_t type_to_side(uint8_t type) {
    switch(type) {
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 3:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}

uint8_t opposite_side(uint8_t type) {
    switch(type) {
        case 1:
            return 2;
            break;
        case 2:
            return 1;
            break;
        case 3:
            return 2;
            break;
        default:
            return 0;
            break;
    }
}
