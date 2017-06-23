/* register_types.cpp */

#include "register_types.h"
#include "object_type_db.h"
#include "vector_keyframe.h"

void register_vector_keyframe_types() {
        ObjectTypeDB::register_type<VectorKeyframe>();
}

void unregister_vector_keyframe_types() {
   //nothing to do here
}