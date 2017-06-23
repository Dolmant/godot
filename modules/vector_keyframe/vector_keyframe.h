/* keyframe.h */
#ifndef VECTOR_KEYFRAME_H
#define VECTOR_KEYFRAME_H

#include "reference.h"
#include <vector>
#include "dictionary.h"
#include "math_2d.h"
#include <iterator>

class VectorKeyframe : public Reference {
    OBJ_TYPE(VectorKeyframe,Reference);

    struct frame_struct {
        Vector2 init;
        Vector2 gradient;
        Vector2 gradientgradient;
        float time;
    };

    std::vector<frame_struct> keyframes;
    int internal_type;
    Vector2 get_cache = Vector2(0, 0);
    float get_time_cache = -1;
    
protected:
    static void _bind_methods();

public:
    Dictionary get_frame_number(int frameNo);
    Vector2 internal_get_value_frame(float time, int frameNo, float pulsetime);
    Vector2 integrate_value(float from_time, float to_time);
    Vector2 get_wpulse(float time, float pulsetime);
    Vector2 get(float time);
    void add(Vector2 init, float time, Vector2 gradient, Vector2 gradientgradient);
    void delete_frame_no(int number);
    void delete_frame_time(float target_time);
    void amend_frame_no(int number, Vector2 init, float time, Vector2 gradient, Vector2 gradientgradient);
    void amend_frame_time(float target_time, Vector2 init, float time, Vector2 gradient, Vector2 gradientgradient);
    Dictionary get_frame(float time, float pulsetime);
    void set_type(int input_type);

    VectorKeyframe();
};

#endif