/* register_types.cpp */

#include "register_types.h"
#include "object_type_db.h"
#include "keyframe.h"

void register_keyframe_types() {
        ObjectTypeDB::register_type<Keyframe>();
}

void unregister_keyframe_types() {
   //nothing to do here
}