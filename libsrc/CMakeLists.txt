add_library(world_engine STATIC
    world/world.c
    world/transitions.c
    minimap/state.c
    minimap/render.c
    minimap/minimap.c
)

set_target_properties(world_engine PROPERTIES OUTPUT_NAME world)
target_include_directories(world_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(world_engine PUBLIC zgdk zvb_gfx)
target_compile_options(world_engine PRIVATE
    $<$<COMPILE_LANGUAGE:C>:--nostdlib>
)
