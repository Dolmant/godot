/* keyframe.h */
#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "reference.h"
#include <vector>
#include "dictionary.h"
#include <iterator>

class Keyframe : public Reference {
    OBJ_TYPE(Keyframe,Reference);

    struct frame_struct {
        float init;
        float gradient;
        float gradientgradient;
        float time;
    };

    std::vector<frame_struct> keyframes;
    int internal_type;
    float get_cache = Vector2(0, 0);
    float get_time_cache = -1;
    
protected:
    static void _bind_methods();

public:
    Dictionary get_frame_number(int frameNo);
    float internal_get_value_frame(float time, int frameNo, float pulsetime);
    float integrate_value(float from_time, float to_time);
    float get_wpulse(float time, float pulsetime);
    float get(float time);
    void add(float init, float time, float gradient, float gradientgradient);
    void delete_frame_no(int number);
    void delete_frame_time(float target_time);
    void amend_frame_no(int number, float init, float time, float gradient, float gradientgradient);
    void amend_frame_time(float target_time, float init, float time, float gradient, float gradientgradient);
    Dictionary get_frame(float time, float pulsetime);
    void set_type(int input_type);

    Keyframe();
};

#endif