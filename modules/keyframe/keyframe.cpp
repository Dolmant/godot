/* keyframe.cpp */
#include "keyframe.h"
#include <vector>
#include <iterator>
#include "dictionary.h"

Dictionary Keyframe::get_frame_number(int frameNo) {
    frame_struct frame = keyframes[frameNo];
    Dictionary result = memnew(Dictionary);
    result["init"] = frame.init;
    result["time"] = frame.time;
    result["gradient"] = frame.gradient;
    result["gradientgradient"] = frame.gradientgradient;
	return result;
}

float Keyframe::internal_get_value_frame(float time, int frameNo, float pulsetime) {
	frame_struct frame = keyframes[frameNo];
	if (internal_type == 0 || internal_type == 1) {
        return (frame.init + frame.gradient * (time - frame.time) + frame.gradientgradient * (time - frame.time) * (time - frame.time));
    }
    if (time - frame.time < pulsetime) {
        return frame.gradient;
    } else {
        return frame.init;
    }
}

float Keyframe::integrate_value(float from_time, float to_time) {
    float time_box = 0;
    float total_area = 0;
    for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
		if (from_time <= keyframes[frameNo].time && keyframes[frameNo].time < to_time) {
			if (frameNo == 0) {
				time_box = to_time - keyframes[0].time;
				total_area = total_area + time_box*(keyframes[0].init + time_box * keyframes[0].gradient / 2 + time_box * time_box * keyframes[0].gradientgradient / 3);
            } else if (from_time <= keyframes[frameNo - 1].time && keyframes[frameNo - 1].time < to_time) {
				time_box = keyframes[frameNo - 1].time - keyframes[frameNo].time;
				total_area = total_area + time_box*(keyframes[0].init + time_box * keyframes[0].gradient / 2 + time_box * time_box * keyframes[0].gradientgradient / 3);
            } else {
				time_box = to_time - keyframes[frameNo].time;
                total_area = total_area + time_box*(keyframes[0].init + time_box * keyframes[0].gradient / 2 + time_box * time_box * keyframes[0].gradientgradient / 3);
            }
        } else if (from_time <= keyframes[frameNo - 1].time && keyframes[frameNo - 1].time < to_time) {
			time_box = keyframes[frameNo-1].time - keyframes[frameNo].time;
			total_area = total_area + time_box*(keyframes[0].init + time_box * keyframes[0].gradient / 2 + time_box * time_box * keyframes[0].gradientgradient / 3);
			time_box = from_time - keyframes[frameNo].time;
            total_area = total_area - time_box*(keyframes[0].init + time_box * keyframes[0].gradient / 2 + time_box * time_box * keyframes[0].gradientgradient / 3);
        }
    }
	return total_area;
}

float Keyframe::get_wpulse(float time, float pulsetime) {
	if (keyframes[0].time <= time) {
        return internal_get_value_frame(time, 0, pulsetime);
    } else {
        for (size_t frameNo = 0; frameNo < keyframes.size(); ++frameNo) {
            if (keyframes[frameNo].time <= time) {
                return internal_get_value_frame(time, frameNo, pulsetime);
            }
        }
    }
    return 0;
}

float Keyframe::get(float time) {
    if (time == get_time_cache) {
        return get_cache;
    }
    get_time_cache = time;
    get_cache = get_wpulse(time, 0);
    return get_cache;
}

void Keyframe::add(float init, float time, float gradient, float gradientgradient) {
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

void Keyframe::delete_frame_no(int number) {
    std::vector<frame_struct>::iterator it = keyframes.begin() + number;
	keyframes.erase(it);
}

void Keyframe::delete_frame_time(float target_time) {
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

void Keyframe::amend_frame_no(int number, float init, float time, float gradient, float gradientgradient) {
    frame_struct new_frame;
    new_frame.init = init;
    new_frame.time = time;
    new_frame.gradient = gradient;
    new_frame.gradientgradient = gradientgradient;
    keyframes[number] = new_frame;
}

void Keyframe::amend_frame_time(float target_time, float init, float time, float gradient, float gradientgradient) {
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

Dictionary Keyframe::get_frame(float time1, float pulsetime) {
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

void Keyframe::set_type(int input_type) {
    internal_type = input_type;
}

void Keyframe::_bind_methods() {
    ObjectTypeDB::bind_method("get_frame_number",&Keyframe::get_frame_number);
    ObjectTypeDB::bind_method("internal_get_value_frame",&Keyframe::internal_get_value_frame);
    ObjectTypeDB::bind_method("integrate_value",&Keyframe::integrate_value);
    ObjectTypeDB::bind_method("get_wpulse",&Keyframe::get_wpulse);
    ObjectTypeDB::bind_method("get",&Keyframe::get);
    ObjectTypeDB::bind_method("add",&Keyframe::add);
    ObjectTypeDB::bind_method("delete_frame_no",&Keyframe::delete_frame_no);
    ObjectTypeDB::bind_method("delete_frame_time",&Keyframe::delete_frame_time);
    ObjectTypeDB::bind_method("amend_frame_no",&Keyframe::amend_frame_no);
    ObjectTypeDB::bind_method("amend_frame_time",&Keyframe::amend_frame_time);
    ObjectTypeDB::bind_method("get_frame",&Keyframe::get_frame);
    ObjectTypeDB::bind_method("set_type",&Keyframe::set_type);
}

Keyframe::Keyframe() {
    internal_type = 0;
    keyframes.reserve(100);
}