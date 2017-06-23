/* register_types.cpp */

#include "register_types.h"
#include "object_type_db.h"
#include "pathfinder.h"

void register_grid_pathfinding_types() {
        ObjectTypeDB::register_type<Pathfinding>();
}

void unregister_grid_pathfinding_types() {
   //nothing to do here
}