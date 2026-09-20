// nodo_cmd_vel.cpp
// Manda velocidad lineal (m/s) y velocidad angular de yaw (rad/s) a la
// Pixhawk vía AP_DDS. En modo GUIDED, es ArduPilot el que calcula la
// mezcla de los thrusters a partir de esta velocidad/ángulo.

#include <chrono>   // para poder escribir "100ms", "5s", "1s"
#include <memory>   // para std::make_shared

#include "rclcpp/rclcpp.hpp"                       // API base de nodos ROS 2
#include "geometry_msgs/msg/twist_stamped.hpp"     // el mensaje que espera /ap/cmd_vel
#include "ardupilot_msgs/srv/arm_motors.hpp"       // servicio para armar/desarmar
#include "ardupilot_msgs/srv/mode_switch.hpp"      // servicio para cambiar de modo de vuelo

using namespace std::chrono_literals; // habilita los sufijos "s"/"ms" en los timers

// Número de modo GUIDED en ArduRover. Es una constante "mágica" del firmware,
// por eso la nombramos así en vez de escribir "15" suelto en el código.
constexpr uint8_t ROVER_MODE_GUIDED = 15;

class NodoCmdVel : public rclcpp::Node
{
public:
  NodoCmdVel() : Node("nodo_cmd_vel") // nombre del nodo en el grafo de ROS 2
  {
    // Publisher: publica el TwistStamped en /ap/cmd_vel, que es lo que
    // escucha AP_DDS del lado de la Pixhawk.
    cmd_vel_publisher_ = this->create_publisher<geometry_msgs::msg::TwistStamped>(
      "/ap/cmd_vel", 10);

    // Clients: NO publican datos, sino que hacen "llamadas a función remotas"
    // (servicios) contra la Pixhawk. Uno arma motores, el otro cambia de modo.
    arm_client_ = this->create_client<ardupilot_msgs::srv::ArmMotors>("/ap/arm_motors");
    mode_client_ = this->create_client<ardupilot_msgs::srv::ModeSwitch>("/ap/mode_switch");

    // Llamamos a esta función una sola vez, al construir el nodo, para dejar
    // el vehículo listo (armado + en GUIDED) antes de empezar a mandar velocidad.
    armar_y_cambiar_modo();

    // Timer: dispara enviar_cmd_vel() cada 100ms (10 Hz), igual que en los
    // nodos anteriores.
    timer_ = this->create_wall_timer(
      100ms, std::bind(&NodoCmdVel::enviar_cmd_vel, this));

    RCLCPP_INFO(this->get_logger(), "Enviando velocidad/ángulo vía /ap/cmd_vel...");
  }

private:
  void armar_y_cambiar_modo()
  {
    // wait_for_service(5s): espera hasta 5 segundos a que el servicio exista
    // del otro lado (o sea, a que la Pixhawk esté conectada por DDS).
    // Devuelve false si se cumplió el timeout sin encontrarlo.
    if (!mode_client_->wait_for_service(5s)) {
      RCLCPP_WARN(this->get_logger(), "Servicio /ap/mode_switch no disponible");
      return; // si no está el servicio, no tiene sentido seguir acá
    }

    // Armamos el "paquete" del pedido: un Request es una estructura propia
    // de cada servicio, generada a partir de su definición .srv.
    auto mode_req = std::make_shared<ardupilot_msgs::srv::ModeSwitch::Request>();
    mode_req->mode = ROVER_MODE_GUIDED; // le decimos qué modo queremos

    // async_send_request: manda el pedido sin bloquear el hilo esperando
    // la respuesta. No estamos revisando la respuesta acá
    // por simplicidad; en un nodo de producción convendría chequearla.
    mode_client_->async_send_request(mode_req);

    // Mismo patrón para armar motores.
    if (!arm_client_->wait_for_service(5s)) {
      RCLCPP_WARN(this->get_logger(), "Servicio /ap/arm_motors no disponible");
      return;
    }
    auto arm_req = std::make_shared<ardupilot_msgs::srv::ArmMotors::Request>();
    arm_req->arm = true; // true = armar, false = desarmar
    arm_client_->async_send_request(arm_req);
  }

  void enviar_cmd_vel()
  {
    // Mensaje vacío que vamos a ir completando.
    auto mensaje = geometry_msgs::msg::TwistStamped();

    // header.stamp: timestamp de cuándo se generó el dato.
    mensaje.header.stamp = this->now();

    // header.frame_id: en qué marco de referencia están las velocidades.
    // "base_link" = marco del propio cuerpo del vehículo (adelante/atrás,
    // giro sobre su propio eje), en vez de un marco fijo del mundo.
    mensaje.header.frame_id = "base_link";

    // twist.linear.x: velocidad de avance hacia adelante, en metros/segundo.
    mensaje.twist.linear.x = 0.5;

    // twist.angular.z: velocidad de giro alrededor del eje vertical (yaw
    // rate), en radianes/segundo. 0.0 = ir derecho, sin girar.
    mensaje.twist.angular.z = 0.0;

    cmd_vel_publisher_->publish(mensaje);
  }

  // Todas las variables que necesitan "vivir" mientras el nodo esté corriendo
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_vel_publisher_;
  rclcpp::Client<ardupilot_msgs::srv::ArmMotors>::SharedPtr arm_client_;
  rclcpp::Client<ardupilot_msgs::srv::ModeSwitch>::SharedPtr mode_client_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);                        // arranca la infraestructura de ROS 2
  rclcpp::spin(std::make_shared<NodoCmdVel>());     // crea el nodo y lo deja corriendo
  rclcpp::shutdown();                               // limpieza al hacer Ctrl+C
  return 0;
}