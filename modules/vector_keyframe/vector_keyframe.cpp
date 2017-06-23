/* keyframe.cpp */
#include "vector_keyframe.h"
#include <vector>
#include <iterator>
#include "dictionary.h"
#include "math_2d.h"

Dictionary VectorKeyframe::get_frame_number(int frameNo) {
    frame_struct frame = keyframes[frameNo];
    Dictionary result = memnew(Dictionary);
    result["init"] = frame.init;
    result["time"] = frame.time;
    result["gradient"] = frame.gradient;
    result["gradientgradient"] = frame.gradientgradient;
	return result;
}

Vector2 VectorKeyframe::internal_get_value_frame(float time, int frameNo, float pulsetime) {
    frame_struct frame = keyframes[frameNo];
    Vector2 local_time = Vector2(time, time);
    Vector2 frame_time = Vector2(frame.time, frame.time);
	if (internal_type == 0 || internal_type == 1) {
        return (frame.init + frame.gradient * (local_time - frame_time) + frame.gradientgradient * (local_time - frame_time) * (local_time - frame_time));
    }
    if (time - frame.time < pulsetime) {
        return frame.gradient;
    } else {
        return frame.init;
    }
}

Vector2 VectorKeyframe::integrate_value(float from_time, float to_time) {
    float time_box = 0;
    Vector2 v_time_box;
    Vector2 total_area = Vector2(0, 0);
    for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
		if (from_time <= keyframes[frameNo].time && keyframes[frameNo].time < to_time) {
			if (frameNo == 0) {
                time_box = to_time - keyframes[0].time;
                v_time_box = Vector2(time_box, time_box);
				total_area = total_area + v_time_box*(keyframes[0].init + v_time_box * keyframes[0].gradient / 2 + v_time_box * v_time_box * keyframes[0].gradientgradient / 3);
            } else if (from_time <= keyframes[frameNo - 1].time && keyframes[frameNo - 1].time < to_time) {
                time_box = keyframes[frameNo - 1].time - keyframes[frameNo].time;
                v_time_box = Vector2(time_box, time_box);
				total_area = total_area + v_time_box*(keyframes[0].init + v_time_box * keyframes[0].gradient / 2 + v_time_box * v_time_box * keyframes[0].gradientgradient / 3);
            } else {
                time_box = to_time - keyframes[frameNo].time;
                v_time_box = Vector2(time_box, time_box);
                total_area = total_area + v_time_box*(keyframes[0].init + v_time_box * keyframes[0].gradient / 2 + v_time_box * v_time_box * keyframes[0].gradientgradient / 3);
            }
        } else if (from_time <= keyframes[frameNo - 1].time && keyframes[frameNo - 1].time < to_time) {
            time_box = keyframes[frameNo-1].time - keyframes[frameNo].time;
            v_time_box = Vector2(time_box, time_box);
			total_area = total_area + v_time_box*(keyframes[0].init + v_time_box * keyframes[0].gradient / 2 + v_time_box * v_time_box * keyframes[0].gradientgradient / 3);
            time_box = from_time - keyframes[frameNo].time;
            v_time_box = Vector2(time_box, time_box);
            total_area = total_area - v_time_box*(keyframes[0].init + v_time_box * keyframes[0].gradient / 2 + v_time_box * v_time_box * keyframes[0].gradientgradient / 3);
        }
    }
	return total_area;
}

Vector2 VectorKeyframe::get_wpulse(float time, float pulsetime) {
	if (keyframes[0].time <= time) {
        return internal_get_value_frame(time, 0, pulsetime);
    } else {
        for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
            if (keyframes[frameNo].time <= time) {
                return internal_get_value_frame(time, frameNo, pulsetime);
            }
        }
    }
    return Vector2(0, 0);
}

Vector2 VectorKeyframe::get(float time) {
    if (time == get_time_cache) {
        return get_cache;
    }
    get_time_cache = time;
    get_cache = get_wpulse(time, 0);
    return get_cache;
}

void VectorKeyframe::add(Vector2 init, float time, Vector2 gradient, Vector2 gradientgradient) {
    frame_struct frame;
	frame.init = init;
	frame.time = time;
	frame.gradient = gradient;
    frame.gradientgradient = gradientgradient;
	std::vector<frame_struct>::iterator it = keyframes.begin();
    size_t size = keyframes.size();
	if (size == 0) {
		keyframes.insert(it, frame);
    } else if (keyframes[0].time > time) {
        for (size_t frameNo = 0; frameNo < size; ++frameNo) {
			if (keyframes[frameNo].time < time) {
                std::vector<frame_struct>::iterator it2 = keyframes.begin() + frameNo;
				keyframes.insert(it2, frame);
                break;
            }
        }
        if (keyframes.size() == size) {
            keyframes.push_back(frame);
        }
    } else {
        keyframes.insert(it, frame);
    }
}

void VectorKeyframe::delete_frame_no(int number) {
    std::vector<frame_struct>::iterator it = keyframes.begin() + number;
	keyframes.erase(it);
}

void VectorKeyframe::delete_frame_time(float target_time) {
    bool found = 0;
	for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
        if (keyframes[frameNo].time <= target_time) {
            std::vector<frame_struct>::iterator it = keyframes.begin() + frameNo;
			keyframes.erase(it);
			found = 1;
            break;
        }
    }
	if (!found && keyframes.size() > 0) {
        std::vector<frame_struct>::iterator it2 = keyframes.begin() + keyframes.size() - 1;
        keyframes.erase(it2);
    }
}

void VectorKeyframe::amend_frame_no(int number, Vector2 init, float time, Vector2 gradient, Vector2 gradientgradient) {
    frame_struct new_frame;
    new_frame.init = init;
    new_frame.time = time;
    new_frame.gradient = gradient;
    new_frame.gradientgradient = gradientgradient;
    keyframes[number] = new_frame;
}

void VectorKeyframe::amend_frame_time(float target_time, Vector2 init, float time, Vector2 gradient, Vector2 gradientgradient) {
    bool found = 0;
    frame_struct new_frame;
    for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
        if (keyframes[frameNo].time <= target_time) {
            new_frame.init = init;
            new_frame.time = time;
            new_frame.gradient = gradient;
            new_frame.gradientgradient = gradientgradient;
            keyframes[frameNo] = new_frame;
            found = 1;
            break;
        }
    }
    if (!found && keyframes.size() > 0) {
        new_frame.init = init;
        new_frame.time = time;
        new_frame.gradient = gradient;
        new_frame.gradientgradient = gradientgradient;
        keyframes[keyframes.size() - 1] = new_frame;
    }
}

Dictionary VectorKeyframe::get_frame(float time1, float pulsetime) {
    Dictionary frame = memnew(Dictionary);
	if (keyframes[0].time <= time1) {
         frame = get_frame_number(0);
    } else {
        for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
            if (keyframes[frameNo].time <= time1) {
                frame = get_frame_number(frameNo);
                break;
            }
        }
    }
    return frame;
}

void VectorKeyframe::set_type(int input_type) {
    internal_type = input_type;
}

void VectorKeyframe::_bind_methods() {
    ObjectTypeDB::bind_method("get_frame_number",&VectorKeyframe::get_frame_number);
    ObjectTypeDB::bind_method("internal_get_value_frame",&VectorKeyframe::internal_get_value_frame);
    ObjectTypeDB::bind_method("integrate_value",&VectorKeyframe::integrate_value);
    ObjectTypeDB::bind_method("get_wpulse",&VectorKeyframe::get_wpulse);
    ObjectTypeDB::bind_method("get",&VectorKeyframe::get);
    ObjectTypeDB::bind_method("add",&VectorKeyframe::add);
    ObjectTypeDB::bind_method("delete_frame_no",&VectorKeyframe::delete_frame_no);
    ObjectTypeDB::bind_method("delete_frame_time",&VectorKeyframe::delete_frame_time);
    ObjectTypeDB::bind_method("amend_frame_no",&VectorKeyframe::amend_frame_no);
    ObjectTypeDB::bind_method("amend_frame_time",&VectorKeyframe::amend_frame_time);
    ObjectTypeDB::bind_method("get_frame",&VectorKeyframe::get_frame);
    ObjectTypeDB::bind_method("set_type",&VectorKeyframe::set_type);
}

VectorKeyframe::VectorKeyframe() {
    internal_type = 0;
    keyframes.reserve(100);
}