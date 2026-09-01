# copied from ros2_objectpose/environment/gz_sim_system_plugin_path.sh

if [ -d "$AMENT_CURRENT_PREFIX/lib" ]; then
  ament_prepend_unique_value GZ_SIM_SYSTEM_PLUGIN_PATH "$AMENT_CURRENT_PREFIX/lib"
fi
