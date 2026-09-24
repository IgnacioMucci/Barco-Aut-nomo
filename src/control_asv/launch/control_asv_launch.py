# Importaciones básicas del framework de ROS 2 para manejar descripciones de lanzamiento
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration

# funcion principal de ros2 para estructurar y retornar acciones y nodos que se van a ejecutar
def generate_launch_description():

    # configuramos argumentos de conexión con la pixhawk (de mavros)
    # Definimos la ruta del puerto de la placa (fcu_url) por USB/Serial con su baudrate,
    # permitiendo cambiarlo fácilmente sin modificar código duro en los ejecutables.
    fcu_url = LaunchConfiguration('fcu_url', default='/dev/ttyACM0:57600')
    gcs_url = LaunchConfiguration('gcs_url', default='')
    tgt_system = LaunchConfiguration('tgt_system', default='1')
    tgt_component = LaunchConfiguration('tgt_component', default='1')

    # Lanzamiento, nodos a ejecutar:
    return LaunchDescription([
        # primero lanzamos la capa de comunicación MAVROS con la pixhawk
        Node(
            package='mavros',
            executable='mavros_node',
            name='mavros',
            parameters=[{
                'fcu_url': fcu_url,
                'gcs_url': gcs_url,
                'target_system_id': tgt_system,
                'target_component_id': tgt_component
            }],
            output='screen'
        ),

        # desp nodo personalizado para mandar señales PWM crudas en microsegundos 
        # a través del tópico /mavros/rc/override
        Node(
            package='control_asv',
            executable='nodo_mavros',
            name='mavros_override_node',
            output='screen'  # Redirige los logs y RCLCPP_INFO directamente a la terminal
        ),

        # nodo secundario para el control de velocidad y rumbo intermedio (cmd_vel)
        Node(
            package='control_asv',
            executable='nodo_cmd_vel',
            name='cmd_vel_node',
            output='screen'
        ),

        # nodo de alto nivel para gestión de misiones y waypoints basados en GPS
        Node(
            package='control_asv',
            executable='nodo_gps_waypoint',
            name='gps_node',
            output='screen'
        )
        
        # acá seguiremos agregando los nodos q vayamos haciendo y queramos q arranquen al toque
    ])
