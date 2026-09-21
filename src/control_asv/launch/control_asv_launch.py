from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='control_asv',
            executable='nodo_mavros',
            name='mavros_node',
            output='screen' # Muestra los print/logs del nodo en la terminal
        ),
        Node(
            package='control_asv',
            executable='nodo_cmd_vel',
            name='cmd_vel_node',
            output='screen'
        ),
        Node(
            package='control_asv',
            executable='nodo_gps_waypoint',
            name='gps_node',
            output='screen'
        )
        # acá seguiremos agregando los nodos q vayamos haciendo y queramos q arranquen al toque
    ])